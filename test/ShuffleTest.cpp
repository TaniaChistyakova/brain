/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "ShuffleIterator.h"

#include "TestTools.h"

#include "gtest/gtest.h"
#include <deque>


TEST(ShuffleTest, SmokeTest)
{
    size_t n = 3, m = 5;
    size_t count = 0;
    ShuffleIterator shuffle(n, m);
    std::vector<std::vector<size_t> > values;
    do
    {
        auto value = shuffle.Output();
        ++count;
        values.push_back(value);
        size_t prevStat = n - 1;

        for (size_t i = 0; i < value.size(); i++)
        {
            size_t idx = value[i];
            EXPECT_LE(idx, n + m);
            if (idx >= n) // static order must be saved
            {
                EXPECT_EQ(prevStat + 1, idx);

                prevStat = idx;
            }
            else if (i > 0) //dynamic order should be saved partially
            {
                size_t prevIdx = value[i - 1];
                if (prevIdx < n)
                {
                    EXPECT_LE(prevIdx, idx);
                }
            }
        }

        for (size_t i = 0; i < n; i++)
        {
            size_t c = std::count(value.cbegin(), value.cend(), i);
            EXPECT_EQ(1, c);
        }

    } while (shuffle.Advance());

    for (auto const& value : values)
    {
        size_t c = std::count(values.cbegin(), values.cend(), value);

        EXPECT_EQ(1, c);
    }

    EXPECT_EQ(std::pow(m + 1, n), count);
}


TEST(ShuffleTest, ShuffleTest)
{
    std::vector<size_t> indices = { 2,1,0 };

    std::vector<std::shared_ptr<int> > in = { std::shared_ptr<int>{new int(1)}, std::shared_ptr<int>{new int(2)}, std::shared_ptr<int>{new int(3)}, };
    std::deque<std::shared_ptr<int> > out;

    ShuffleIterator::Shuffle(indices, in, out);

    EXPECT_EQ(out.size(), 3);

    EXPECT_EQ(*out[0], 3);
    EXPECT_EQ(*out[1], 2);
    EXPECT_EQ(*out[2], 1);
}

TEST(ShuffleTest, BorderCaseTest1)
{
    size_t m = 5;

    ShuffleIterator shuffle(0, m);

    std::vector<std::vector<size_t> > values;
    do
    {
        auto value = shuffle.Output();
        values.push_back(value);

    } while (shuffle.Advance());
    EXPECT_EQ(1, values.size());
    EXPECT_EQ(std::vector<size_t>({ 0,1,2,3,4 }), values[0]);

}

TEST(ShuffleTest, BorderCaseTest2)
{
    size_t n = 5;

    ShuffleIterator shuffle(n, 0);

    std::vector<std::vector<size_t> > values;
    do
    {
        auto value = shuffle.Output();
        values.push_back(value);

    } while (shuffle.Advance());
    EXPECT_EQ(1, values.size());
    EXPECT_EQ(std::vector<size_t>({ 0,1,2,3,4 }), values[0]);

}