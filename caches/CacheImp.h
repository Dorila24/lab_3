#pragma once
#include "ICache.h"
#include <cstddef>
#include <stdexcept>
#include <unordered_map>
#include <vector>

template <typename Key, typename Value>
class ClockCache final : public ICache<Key, Value> {
public:
    explicit ClockCache(std::size_t capacity)
        : capacity_(capacity), frames_(capacity) {
        if (capacity_ == 0) {
            throw std::invalid_argument("ClockCache capacity must be > 0");}}

    void set(const Key& key, const Value& value) override {
        auto it = index_.find(key);
        if (it != index_.end()) {
            Frame& f = frames_[it->second];
            f.value = value;
            f.ref = true;
            f.dirty = true;
            return;}

        if (index_.size() < capacity_) {
            std::size_t free_idx = findFreeSlotOrThrow();
            placeNew(free_idx, key, value);
            return;}

        std::size_t victim = findVictimOrThrow();
        evictAndReplace(victim, key, value);}

    Value get(const Key& key) override {
        auto it = index_.find(key);
        if (it == index_.end()) {
            throw std::out_of_range("ClockCache::get: key not found");}
        Frame& f = frames_[it->second];
        f.ref = true;
        return f.value;}

    bool contains(const Key& key) const override {
        return index_.find(key) != index_.end();}

    void clear() override {
        index_.clear();
        for (auto& f : frames_) {
            f.occupied = false;
            f.ref = false;
            f.dirty = false;
            f.pin_count = 0;}
        hand_ = 0;}

    std::size_t size() const override {
        return index_.size();}

    void pin(const Key& key) {
        auto it = index_.find(key);
        if (it == index_.end()) throw std::out_of_range("pin: key not found");
        frames_[it->second].pin_count++;}

    void unpin(const Key& key) {
        auto it = index_.find(key);
        if (it == index_.end()) throw std::out_of_range("unpin: key not found");
        Frame& f = frames_[it->second];
        if (f.pin_count == 0) throw std::logic_error("unpin: pin_count already 0");
        f.pin_count--;}

    bool isdirty(const Key& key) const {
        auto it = index_.find(key);
        if (it == index_.end()) throw std::out_of_range("isdirty: key not found");
        return frames_[it->second].dirty;}

private:
    struct Frame {
        Key key{};
        Value value{};
        bool ref{false};
        bool dirty{false};
        std::size_t pin_count{0};
        bool occupied{false};};

    std::size_t capacity_{0};
    std::vector<Frame> frames_;
    std::unordered_map<Key, std::size_t> index_;
    std::size_t hand_{0};

    std::size_t findFreeSlotOrThrow() {
        for (std::size_t i = 0; i < frames_.size(); ++i) {
            if (!frames_[i].occupied) {
                return i;}}
        throw std::logic_error("internal error: no free slot but cache not full");}

    void placeNew(std::size_t idx, const Key& key, const Value& value) {
        Frame& f = frames_[idx];
        f.key = key;
        f.value = value;
        f.ref = false;
        f.dirty = true;
        f.pin_count = 0;
        f.occupied = true;
        index_[key] = idx;}

    std::size_t findVictimOrThrow() {
        const std::size_t n = frames_.size();
        if (n == 0) throw std::logic_error("capacity must be > 0");

        std::size_t steps = 0;
        while (steps < 2 * n) {
            Frame& f = frames_[hand_];

            if (f.pin_count > 0) {
                hand_ = (hand_ + 1) % n;
                ++steps;
                continue;}

            if (f.ref) {
                f.ref = false;
                hand_ = (hand_ + 1) % n;
                ++steps;
                continue;}

            return hand_;}

        throw std::runtime_error("ClockCache: cannot evict (all frames are pinned)");}

    void evictAndReplace(std::size_t victim_idx, const Key& key, const Value& value) {
        Frame& victim = frames_[victim_idx];
        if (!victim.occupied) {
            throw std::logic_error("internal error: victim frame is not occupied");}
        index_.erase(victim.key);
        victim.key = key;
        victim.value = value;
        victim.ref = false;
        victim.dirty = true;
        victim.pin_count = 0;
        victim.occupied = true;
        index_[key] = victim_idx;
        hand_ = (victim_idx + 1) % frames_.size();}};
