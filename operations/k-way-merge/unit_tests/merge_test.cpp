#include <gtest/gtest.h>

#include <algorithm>
#include <random>
#include <stdexcept>
#include <vector>

#include "../KwayMerge.h"

TEST(KWayMerge, EmptyInput) {
    const std::vector<int> v;
    const auto out = KwayMergeSort(v);

    EXPECT_TRUE(out.empty());
}

TEST(KWayMerge, BasicSort) {
    const std::vector<int> v{5, 1, 3, 2, 4};
    const auto out = KwayMergeSort(v, /*k=*/3, /*block_size=*/2);

    EXPECT_EQ(out, (std::vector<int>{1, 2, 3, 4, 5}));
}

TEST(KWayMerge, HandlesDuplicates) {
    const std::vector<int> v{3, 1, 2, 1, 3, 2, 2, 1};
    const auto out = KwayMergeSort(v, 4, 3);

    ASSERT_EQ(out.size(), v.size());
    EXPECT_TRUE(std::is_sorted(out.begin(), out.end()));

    EXPECT_EQ(std::count(out.begin(), out.end(), 1), 3);
    EXPECT_EQ(std::count(out.begin(), out.end(), 2), 3);
    EXPECT_EQ(std::count(out.begin(), out.end(), 3), 2);
}

TEST(KWayMerge, DifferentParametersStillCorrect) {
    const std::vector<int> v{10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    const auto out = KwayMergeSort(v, 2, 1);

    ASSERT_FALSE(out.empty());
    EXPECT_TRUE(std::is_sorted(out.begin(), out.end()));
    EXPECT_EQ(out.front(), 1);
    EXPECT_EQ(out.back(), 10);
}

TEST(KWayMerge, MatchesStdSortOnRandomData) {
    std::mt19937 rng{42};
    std::uniform_int_distribution<int> dist{-1000, 1000};

    std::vector<int> v(200);
    for (int& x : v) {
        x = dist(rng);
    }

    auto expected = v;
    std::sort(expected.begin(), expected.end());

    const auto out = KwayMergeSort(v, 5, 17);
    EXPECT_EQ(out, expected);
}

TEST(KWayMerge, InvalidArgumentsThrow) {
    const std::vector<int> v{2, 1};

    EXPECT_THROW(KwayMergeSort(v, /*k=*/1, /*block_size=*/2), std::invalid_argument);
    EXPECT_THROW(KwayMergeSort(v, 2, 0), std::invalid_argument);
}
