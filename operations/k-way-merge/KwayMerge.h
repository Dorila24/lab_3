#pragma once
#include <algorithm>
#include <cstddef>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

namespace kway_detail {
template <typename T>
std::vector<T> MergeKRuns(const std::vector<std::vector<T>>& runs)
{
    struct Node {
        T value;
        std::size_t run_idx;
        std::size_t pos;};

    struct Cmp {
        bool operator()(const Node& a, const Node& b) const {
            if (a.value != b.value) return a.value > b.value;
            if (a.run_idx != b.run_idx) return a.run_idx > b.run_idx;
            return a.pos > b.pos;}};

    std::priority_queue<Node, std::vector<Node>, Cmp> pq;

    std::size_t total = 0;
    for (std::size_t i = 0; i < runs.size(); ++i) {
        total += runs[i].size();
        if (!runs[i].empty()) {
            pq.push(Node{runs[i][0], i, 0});}}

    std::vector<T> out;
    out.reserve(total);
    while (!pq.empty()) {
        Node cur = pq.top();
        pq.pop();
        out.push_back(cur.value);
        const std::size_t next_pos = cur.pos + 1;
        if (next_pos < runs[cur.run_idx].size()) {
            pq.push(Node{runs[cur.run_idx][next_pos], cur.run_idx, next_pos});}}
    return out;}} 


template<typename T>
std::vector<T> KwayMergeSort(const std::vector<T>& array, std::size_t k = 4, std::size_t block_size = 8)
{
    if (k < 2) {
        throw std::invalid_argument("k must be >= 2");}
    if (block_size == 0) {
        throw std::invalid_argument("block_size must be > 0");}
    if (array.size() <= 1) {
        return array;}

    std::vector<std::vector<T>> runs;
    runs.reserve((array.size() + block_size - 1) / block_size);

    for (std::size_t i = 0; i < array.size(); i += block_size) {
        std::size_t j = std::min(i + block_size, array.size());
        std::vector<T> chunk(array.begin() + static_cast<std::ptrdiff_t>(i), array.begin() + static_cast<std::ptrdiff_t>(j));
        std::sort(chunk.begin(), chunk.end());
        runs.push_back(std::move(chunk));}
    while (runs.size() > 1) {
        std::vector<std::vector<T>> next;
        next.reserve((runs.size() + k - 1) / k);
        for (std::size_t i = 0; i < runs.size(); i += k) {
            std::size_t end = std::min(i + k, runs.size());
            std::vector<std::vector<T>> group;
            group.reserve(end - i);
            for (std::size_t r = i; r < end; ++r) {
                group.push_back(std::move(runs[r]));}
            next.push_back(kway_detail::MergeKRuns(group));}
        runs = std::move(next);}
    return std::move(runs.front());}
