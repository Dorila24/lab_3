#include <gtest/gtest.h>
#include "../CacheImp.h"

TEST(ClockCache, PutGetAndSize) {
    ClockCache<int, int> cache(3);
    EXPECT_EQ(cache.size(), 0u);
    cache.set(1, 10);
    cache.set(2, 20);
    EXPECT_TRUE(cache.contains(1));
    EXPECT_TRUE(cache.contains(2));
    EXPECT_EQ(cache.get(1), 10);
    EXPECT_EQ(cache.get(2), 20);
    EXPECT_EQ(cache.size(), 2u);}

TEST(ClockCache, EvictsWhenFull) {
    ClockCache<int, int> cache(2);
    cache.set(1, 10);
    cache.set(2, 20);
    cache.set(3, 30);
    EXPECT_FALSE(cache.contains(1));
    EXPECT_TRUE(cache.contains(2));
    EXPECT_TRUE(cache.contains(3));}

TEST(ClockCache, SecondChanceWorks) {
    ClockCache<int, int> cache(2);
    cache.set(1, 10);
    cache.set(2, 20);
    cache.get (1)
    cache.set(3, 30);
    EXPECT_TRUE(cache.contains(1));
    EXPECT_FALSE(cache.contains(2));
    EXPECT_TRUE(cache.contains(3));}

TEST(ClockCache, SetExistingIsHitAndMarksDirty) {
    ClockCache<int, int> cache(2);
    cache.set(1, 10);
    cache.set(2, 20);
    cache.set(1, 111);
    EXPECT_EQ(cache.size(), 2u);
    EXPECT_EQ(cache.get(1), 111);
    EXPECT_TRUE(cache.isdirty(1));}

TEST(ClockCache, PinnedPageIsNotEvicted) {
    ClockCache<int, int> cache(2);
    cache.set(1, 10);
    cache.set(2, 20);
    cache.pin(1);
    cache.set(3, 30);
    EXPECT_TRUE(cache.contains(1));
    EXPECT_FALSE(cache.contains(2));
    EXPECT_TRUE(cache.contains(3));}

TEST(ClockCache, AllPinnedThrowsOnInsert) {
    ClockCache<int, int> cache(2);
    cache.set(1, 10);
    cache.set(2, 20);
    cache.pin(1);
    cache.pin(2);
    EXPECT_THROW(cache.set(3, 30), std::runtime_error);}

TEST(ClockCache, ClearEmptiesCache) {
    ClockCache<int, int> cache(2);
    cache.set(1, 10);
    cache.set(2, 20);
    cache.clear();
    EXPECT_EQ(cache.size(), 0u);
    EXPECT_FALSE(cache.contains(1));}

TEST(ClockCache, GetMissingThrows) {
    ClockCache<int, int> cache(1);
    EXPECT_THROW((void)cache.get(123), std::out_of_range);}
