/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "BMath.h"

#include "TestTools.h"

#include "gtest/gtest.h"


TEST(MathUnitTest, TestPow2)
{
    EXPECT_EQ(1024LL, Pow2(10));
    EXPECT_EQ(0x10000LL, Pow2(0x10));
}

TEST(MathUnitTest, TestLog2)
{
    EXPECT_EQ(10LL, Log2(1024LL));
    EXPECT_EQ(0x10LL, Log2(0x10000LL));
}


TEST(MathUnitTest, TestStringToBitfield)
{
    uint64_t out = 0;
    bool res = false;

    EXPECT_TRUE(TestTools::StringToBits("", out));
    EXPECT_EQ(0b0LL, out);

    EXPECT_TRUE(TestTools::StringToBits("111", out));
    EXPECT_EQ(0b111LL, out);

    EXPECT_TRUE(TestTools::StringToBits("11001010", out));
    EXPECT_EQ(0b01010011LL, out);
}

TEST(MathUnitTest, TestDotsToBits)
{
    std::vector<std::vector<Dot> > dots_args =
    {
        {0},
        {1},
        {0, 1, 1, 0, 1, 0, 0, 1, 0},
        {1,1,1,1,0,1,0,1,0,1,0,1,0,0,0,0}
    };

    std::vector<uint64_t> bits_args =
    {
        0,
        1,
        0b010010110,
        0b0000101010101111,
    };

    for (uint64_t i = 0; i < dots_args.size(); ++i)
    {
        const std::vector<Dot>& dots1 = dots_args[i];
        uint64_t bits1 = bits_args[i];

        std::vector<Dot> dots2 = TestTools::BitsToDots(bits1, 20);
        uint64_t bits2 = TestTools::DotsToBits(dots1);
        std::vector<Dot> dots3(dots1);
        dots3.resize(20);

        EXPECT_EQ(bits1, bits2);
        EXPECT_EQ(dots3, dots2);
    }
}

TEST(MathUnitTest, AppendAnotherDotArrayTest)
{
    std::vector<std::vector<Dot> > dots = { { 1,0,1,1,1,0,0,1,1,0,1 } , { 1,0,1,0,0,1,0,1,1 } };
    std::vector<Dot> dotsResult = dots[0];
    dotsResult.insert(dotsResult.end(), dots[1].begin(), dots[1].end());

    DotArray arr[2] = { dots[0], dots[1] };

    DotArray arrResult = arr[0];
    arrResult.Append(arr[1]);

    EXPECT_EQ(dotsResult.size(), arrResult.size());

    for (size_t i = 0; i < dotsResult.size(); i++)
    {
        EXPECT_EQ(dotsResult[i], (Dot)dotsResult[i]);
    }
}



TEST(MathUnitTest, SmokeTest)
{
    DotFixedArray<13> arr;
    EXPECT_EQ(false, arr.empty());
    EXPECT_EQ(13LL, arr.size());

    for (size_t i = 0; i < arr.size(); i++)
    {
        EXPECT_EQ(Dot(0), (Dot)arr[i]);
    }

    for (size_t i = 0; i < arr.size(); i++)
    {
        if (i % 3)
        {
            arr[i] = Dot(1);
        }
    }

    for (size_t i = 0; i < arr.size(); i++)
    {
        if (i % 3)
        {
            EXPECT_EQ(Dot(1), (Dot)arr[i]);;
        }
        else
        {
            EXPECT_EQ(Dot(0), (Dot)arr[i]);;
        }
    }
}

TEST(MathUnitTest, CompareTest)
{
    DotFixedArray<11> arr1 = { 0,1,1,0,0,1,0,0,1,0,0 };
    DotFixedArray<11> arr2 = { 0,1,1,0,0,1,0,0,1,0,0 };
    DotFixedArray<11> arr3 = { 0,1,1,1,0,1,1,0,1,0,0 };

    EXPECT_EQ(true, arr1 == arr2);
    EXPECT_EQ(false, arr1 == arr3);
    EXPECT_EQ(true, arr2 != arr3);
}

TEST(MathUnitTest, AppendFixedDotArrayTest)
{
    std::vector<std::vector<Dot> > dots = { { 1,0,1,1,1,0,0,1,1,0,1 } , { 1,0,1,0,0,1,0,1,1 } };
    std::vector<Dot> dotsResult = dots[0];
    dotsResult.insert(dotsResult.end(), dots[1].begin(), dots[1].end());

    DotArray arrResult = dots[0];
    DotFixedArray<9> arr = dots[1];

    arrResult.Append(arr);

    EXPECT_EQ(dotsResult.size(), arrResult.size());

    for (size_t i = 0; i < dotsResult.size(); i++)
    {
        EXPECT_EQ(dotsResult[i], (Dot)dotsResult[i]);
    }
}


TEST(MaskTest, ConsumeTest)
{
    bool res = false;

    Mask m[] = { 5, 5, 5, 5 };

    m[0].Load_ForTest({ 1,1,1,1,1 }, { 1,1,1,1,1 });
    m[1].Load_ForTest({ 1,1,1,1,1 }, { 1,0,1,1,1 });
    m[2].Load_ForTest({ 1,1,1,1,1 }, { 1,1,0,1,1 });
    m[3].Load_ForTest({ 1,1,1,1,1 }, { 1,0,0,1,1 });

    for (size_t i = 1; i < 4; i++)
    {
        res = m[0].isConsume(m[i]);
        EXPECT_FALSE(res);
        res = m[i].isConsume(m[0]);
        EXPECT_TRUE(res);
    }

    res = m[1].isConsume(m[2]);
    EXPECT_FALSE(res);
    res = m[2].isConsume(m[1]);
    EXPECT_FALSE(res);

    for (size_t i = 0; i < 3; i++)
    {
        res = m[3].isConsume(m[i]);
        EXPECT_TRUE(res);
    }
}

TEST(MaskTest, FoundInTest)
{
    Mask m[] = { 5, 5, 5 };
    bool res = false;

    m[0].Load_ForTest({ 1,1,1,1,1 }, { 1,1,1,1,1 });
    m[1].Load_ForTest({ 1,0,1,1,1 }, { 1,1,1,1,1 });
    m[2].Load_ForTest({ 1,1,0,0,1 }, { 1,1,1,1,1 });

    DotArray dataToSearch = { 1,0,1,1,1,0,1,0,1,0,0 };
    res = m[0].isFoundIn(DotArrayView(dataToSearch, 0));
    EXPECT_FALSE(res);
    res = m[1].isFoundIn(DotArrayView(dataToSearch, 0));
    EXPECT_TRUE(res);
    res = m[2].isFoundIn(DotArrayView(dataToSearch, 0));
    EXPECT_FALSE(res);
}

TEST(MaskTest, FoundInDotArrayTest)
{
    Mask m[] = { 5, 5, 5 };
    bool res = false;

    m[0].Load_ForTest({ 1,1,1,1,1 }, { 1,1,1,1,1 });
    m[1].Load_ForTest({ 1,0,1,1,1 }, { 1,1,1,1,1 });
    m[2].Load_ForTest({ 1,1,0,0,1 }, { 1,1,1,1,1 });

    DotArray dataToSearch = { 1,0,1,1,1,0,1,0,1,0,0 };
    res = m[0].isFoundIn(dataToSearch);
    EXPECT_FALSE(res);
    res = m[1].isFoundIn(dataToSearch);
    EXPECT_TRUE(res);
    res = m[2].isFoundIn(dataToSearch);
    EXPECT_FALSE(res);
}

TEST(MaskTest, FoundInLetterArrayTest)
{
    Mask m[] = { 5, 5, 5 };
    bool res = false;

    m[0].Load_ForTest({ 1,1,1,1,1 }, { 1,1,1,1,1 });
    m[1].Load_ForTest({ 1,0,1,1,1 }, { 1,1,1,1,1 });
    m[2].Load_ForTest({ 1,1,0,0,1 }, { 1,1,1,1,1 });

    DotArray dataToSearch = { 1,0,1,1,1,0,1,0,1,0,0 };
    std::vector<Letter> lettersToSearch;

    for (size_t i = 0; i < dataToSearch.GetSizeInLetters(); ++i)
    {
        lettersToSearch.push_back(dataToSearch.GetLetter(i));
    }

    res = m[0].isFoundIn(LetterView(lettersToSearch, 0));
    EXPECT_FALSE(res);
    res = m[1].isFoundIn(LetterView(lettersToSearch, 0));
    EXPECT_TRUE(res);
    res = m[2].isFoundIn(LetterView(lettersToSearch, 0));
    EXPECT_FALSE(res);
}

TEST(MaskTest, AndOnSetTestSuccess)
{
    auto args_permutations = TestTools::GenerateAllPermulations<DotArray>(5);

    Mask mA(5), mB(5);
    mA.Load_ForTest({ 1,1,0,0,1 }, { 1,1,0,0,1 });
    mB.Load_ForTest({ 1,0,0,1,1 }, { 1,0,0,1,1 });

    Mask mAnd(5);
    bool res = Mask::AndOnSets(mA, mB, mAnd);

    EXPECT_TRUE(res);

    for (auto&& args : args_permutations)
    {
        bool isFoundA = mA.isFoundIn(DotArrayView(args, 0));
        bool isFoundB = mB.isFoundIn(DotArrayView(args, 0));

        bool isFoundAnd = mAnd.isFoundIn(DotArrayView(args, 0));
        EXPECT_EQ(isFoundA && isFoundB, isFoundAnd);
    }
}

TEST(MaskTest, AndOnSetTestFailure)
{
    auto args_permutations = TestTools::GenerateAllPermulations<DotArray>(5);

    Mask mA(5), mB(5);
    mA.Load_ForTest({ 1,1,0,0,1 }, { 1,1,0,0,1 });
    mB.Load_ForTest({ 1,0,0,1,0 }, { 1,0,0,1,1 });

    Mask mAnd(5);
    bool res = Mask::AndOnSets(mA, mB, mAnd);

    EXPECT_FALSE(res);
}

