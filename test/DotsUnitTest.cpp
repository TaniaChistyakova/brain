/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "DotArray.h"

#include "TestTools.h"

#include "gtest/gtest.h"

TEST(DotsUnitTest, SimpleTest)
{
    DotArray ar;
    EXPECT_EQ(0LL, ar.size());
    EXPECT_TRUE(ar.empty());

    ar.resize(1);

    EXPECT_EQ(1LL, ar.size());
    EXPECT_FALSE(ar.empty());
}

TEST(DotsUnitTest, CtorTest)
{
    std::vector<Dot> vec(1000, 1);
    for (size_t i = 0; i < vec.size(); i++)
    {
        if (i % 3)
        {
            vec[i] = 0;
        }
    }


    DotArray ar(vec);
    EXPECT_EQ(vec.size(), ar.size());

    for (size_t i = 0; i < ar.size(); i++)
    {
        Dot val = ar[i];
        if (i % 3)
        {
            EXPECT_FALSE((bool)val);
        }
        else
        {
            EXPECT_TRUE((bool)val);
        }
    }
}

TEST(DotsUnitTest, Ctor2Test)
{
    DotArray da0(Dot(0), 10);
    DotArray da1(Dot(1), 10);

    for (size_t i = 0; i < 10; i++)
    {
        EXPECT_EQ(Dot(0), Dot(da0[i]));
        EXPECT_EQ(Dot(1), Dot(da1[i]));
    }
}

TEST(DotsUnitTest, Ctor3Test)
{
    DotArray da0 = { 0,1,0,1, 0,1, 0,1, 0,1, 0,1, 0,1, 0,1, 0,1, 0,1, };
    DotArray da1(da0, 5);

    EXPECT_EQ(DotArray({ 0,1,0,1, 0 }), da1);
}

TEST(DotsUnitTest, SettingAndGettingTest)
{
    DotArray ar;
    ar.resize(1000);

    for (size_t i = 0; i < ar.size(); i++)
    {
        Dot val = ar[i];
        EXPECT_FALSE((bool)val);
    }

    for (size_t i = 0; i < ar.size(); i++)
    {
        ar[i] = 1;
    }

    for (size_t i = 0; i < ar.size(); i++)
    {
        Dot val = ar[i];
        EXPECT_TRUE((bool)val);
    }

    for (size_t i = 0; i < ar.size(); i++)
    {
        if (i % 3)
        {
            ar[i] = 0;
        }
    }

    for (size_t i = 0; i < ar.size(); i++)
    {
        Dot val = ar[i];
        if (i % 3)
        {
            EXPECT_FALSE((bool)val);
        }
        else
        {
            EXPECT_TRUE((bool)val);
        }
    }
}

TEST(DotsUnitTest, ResizeTest)
{
    DotArray ar;
    ar.resize(1000);

    for (size_t i = 0; i < ar.size(); i++)
    {
        ar[i] = 1;
    }

    ar.resize(700);

    EXPECT_EQ(700LL, ar.size());

    for (size_t i = 0; i < ar.size(); i++)
    {
        Dot val = ar[i];
        EXPECT_TRUE((bool)val);
    }

    ar.resize(1100);

    EXPECT_EQ(1100LL, ar.size());

    for (size_t i = 0; i < ar.size(); i++)
    {
        Dot val = ar[i];
        if (i < 700)
        {
            EXPECT_TRUE((bool)val);
        }
        else
        {
            EXPECT_FALSE((bool)val);
        }
    }


}


TEST(DotsUnitTest, OperationsPerElementTest)
{
    DotArray ar;
    ar.resize(1000);

    for (size_t i = 0; i < ar.size(); i++)
    {
        ar[i] ^= 1;
    }

    for (size_t i = 0; i < ar.size(); i++)
    {
        Dot val = ar[i];
        EXPECT_TRUE((bool)val);
    }

    for (size_t i = 0; i < ar.size(); i++)
    {
        if (i % 3)
        {
            ar[i] &= 0;
        }
    }

    for (size_t i = 0; i < ar.size(); i++)
    {
        Dot val = ar[i];
        if (i % 3)
        {
            EXPECT_FALSE((bool)val);
        }
        else
        {
            EXPECT_TRUE((bool)val);
        }
    }
}

TEST(DotsUnitTest, OperationsPerArrayTest)
{
    DotArray ar[] =
    {
        {0,0,0,0,0,0,0,0,0},
        {0,1,0,1,0,0,1,0,0},
        {0,1,0,0,1,1,0,1,0},
        {0,0,0,1,1,0,0,0,1},
    };

    for (size_t i = 0; i < 4; i++)
    {
        const DotArray& ar1 = ar[i];
        if (i == 0)
        {
            EXPECT_TRUE(ar1.none());
            EXPECT_FALSE(ar1.any());
        }
        else
        {
            EXPECT_FALSE(ar1.none());
            EXPECT_TRUE(ar1.any());
        }

        for (size_t j = 0; j < 4; j++)
        {
            const DotArray& ar2 = ar[j];
            DotArray res = ar1, res2 = ar1;
            EXPECT_EQ(ar1.size(), res.size());

            res ^= ar2;
            res2 &= ar2;

            if (i == j)
            {
                EXPECT_TRUE(ar1 == ar2);
                EXPECT_TRUE(res.none());
            }
            else
            {
                EXPECT_FALSE(ar1 == ar2);
                EXPECT_FALSE(res.none());
            }

            for (size_t k = 0; k < res.size(); k++)
            {
                EXPECT_EQ(ar1[k] ^ ar2[k], (Dot)res[k]);
                EXPECT_EQ(ar1[k] & ar2[k], (Dot)res2[k]);
            }

            // no garbage at the end!
            res.resize(res.size() + 1);
            Dot back = res[res.size() - 1];

            EXPECT_EQ(Dot(), back);
        }
    }
}

TEST(DotsUnitTest, OperationsPerLargeArrayTest)
{
    std::vector<DotArray> ar;
    ar.push_back(DotArray(10000));
    for (size_t i = 0; i < 3; i++)
    {
        ar.push_back(TestTools::GenerateRandomDotVector(10000));
    }

    for (size_t i = 0; i < 4; i++)
    {
        const DotArray& ar1 = ar[i];
        if (i == 0)
        {
            EXPECT_TRUE(ar1.none());
            EXPECT_FALSE(ar1.any());
        }
        else
        {
            EXPECT_FALSE(ar1.none());
            EXPECT_TRUE(ar1.any());
        }

        for (size_t j = 0; j < 4; j++)
        {
            const DotArray& ar2 = ar[j];
            DotArray res = ar1, res2 = ar1;
            EXPECT_EQ(ar1.size(), res.size());

            res ^= ar2;
            res2 &= ar2;

            if (i == j)
            {
                EXPECT_TRUE(ar1 == ar2);
                EXPECT_TRUE(res.none());
            }
            else
            {
                EXPECT_FALSE(ar1 == ar2);
                EXPECT_FALSE(res.none());
            }

            for (size_t k = 0; k < res.size(); k++)
            {
                EXPECT_EQ(ar1[k] ^ ar2[k], (Dot)res[k]);
                EXPECT_EQ(ar1[k] & ar2[k], (Dot)res2[k]);
            }

            // no garbage at the end!
            res.resize(res.size() + 1);
            Dot back = res[res.size() - 1];

            EXPECT_EQ(Dot(), back);
        }
    }
}



TEST(DotsUnitTest, MethodsPerArrayTest)
{
    std::vector<DotArray> ar;
    ar.push_back(DotArray(10000));
    for (size_t i = 0; i < 3; i++)
    {
        ar.push_back(TestTools::GenerateRandomDotVector(10000));
    }

    for (size_t i = 0; i < 4; i++)
    {
        const DotArray& ar1 = ar[i];
        DotArray res(ar1);

        res.flip();

        for (size_t k = 0; k < res.size(); k++)
        {
            EXPECT_NE(ar1[k], (Dot)res[k]);
        }

        res = ar1;
        res.reset();

        for (size_t k = 0; k < res.size(); k++)
        {
            EXPECT_EQ(Dot(0), (Dot)res[k]);
        }

        res = ar1;
        res.set();

        for (size_t k = 0; k < res.size(); k++)
        {
            EXPECT_EQ(Dot(1), (Dot)res[k]);
        }
    }
}

TEST(DotsUnitTest, ConversationTest)
{
    std::vector<Dot> init = { 0,1,0,1,0,1,1,0,1,0,1,0,0,1,1,0 };

    DotArray ar = init;

    uint64_t bitfield1 = TestTools::DotsToBits(init);
    uint64_t bitfield2 = ar.ToBits();

    EXPECT_EQ(bitfield1, bitfield2);

    std::vector<Dot> final = ar.ToVector_ForTest();

    EXPECT_EQ(init, final);
}


TEST(DotsUnitTest, Conversation2Test)
{
    std::vector<Dot> init = TestTools::GenerateRandomDotVector(14);

    DotArray ar = init;

    uint64_t bitfield1 = TestTools::DotsToBits(init);
    uint64_t bitfield2 = ar.ToBits<uint16_t>();

    EXPECT_EQ(bitfield1, bitfield2);

    std::vector<Dot> final = ar.ToVector_ForTest();

    EXPECT_EQ(init, final);
}

TEST(DotsUnitTest, ViewerTest)
{
    DotArray ar = DotArray(TestTools::GenerateRandomDotVector(20));
    DotArray ar2 = ar;

    for (size_t i = 0; i < 20; i++)
    {
        DotArrayView view(ar, i);

        EXPECT_EQ(ar.size() - i, view.size());

        for (size_t j = 0; j < 20 - i; j++)
        {
            EXPECT_EQ((Dot)ar[j + i], (Dot)view[j]);
        }
    }

    {
        DotArrayView view(ar, 5, 10);

        for (size_t i = 0; i < view.size(); ++i)
        {
            view[i] ^= 1;
        }
    }

    for (size_t i = 0; i < 20; i++)
    {
        if (i < 5 || i >= 15)
        {
            EXPECT_EQ((Dot)ar[i], (Dot)ar2[i]);
        }
        else
        {
            EXPECT_NE((Dot)ar[i], (Dot)ar2[i]);
        }
    }

}

TEST(DotsUnitTest, RandomiserTest)
{
    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());

    DotArray ar = DotArray::GenerateRandom(10, generator);
    EXPECT_EQ(10LL, ar.size());
}

TEST(DotsUnitTest, OperationResizeTest)
{
    DotArray ar1 = DotArray(TestTools::GenerateRandomDotVector(5));
    DotArray ar2 = DotArray(TestTools::GenerateRandomDotVector(10));

    DotArray arAnd = ar1 & ar2;
    EXPECT_EQ(5ull, arAnd.size());

    DotArray arXor = ar1 ^ ar2;
    EXPECT_EQ(5ull, arXor.size());

    arAnd.resize(10);
    arXor.resize(10);
    for (size_t i = 5; i < 10; i++)
    {
        EXPECT_EQ(Dot(0), Dot(arAnd[i]));
        EXPECT_EQ(Dot(0), Dot(arXor[i]));
    }
}

TEST(DotsUnitTest, ValuesCountTest)
{
    std::vector<Dot> vec = TestTools::GenerateRandomDotVector(100);
    size_t zeroesNumRef = 0;
    size_t onesNumRef = 0;
    for (size_t i = 0; i < vec.size(); i++)
    {
        if (vec[i])
        {
            ++onesNumRef;
        }
        else
        {
            ++zeroesNumRef;
        }
    }

    DotArray ar = vec;

    size_t zeroesNumCalc = ar.zeroesNum();
    size_t onesNumCalc = ar.onesNum();

    EXPECT_EQ(zeroesNumRef, zeroesNumCalc);
    EXPECT_EQ(onesNumRef, onesNumCalc);
}


TEST(DotsUnitTest, LongOrTest)
{
    DotArray ar1 = DotArray(TestTools::GenerateRandomDotVector(5));
    DotArray ar2 = DotArray(TestTools::GenerateRandomDotVector(10));

    DotArray arOr = DotArray::LongOr(ar1, ar2);
    EXPECT_EQ(10ull, arOr.size());

    for (size_t i = 0; i < 5; i++)
    {
        if ((bool)ar1[i] || (bool)ar2[i])
        {
            EXPECT_EQ(Dot(1), Dot(arOr[i]));
        }
        else
        {
            EXPECT_EQ(Dot(0), Dot(arOr[i]));
        }
    }

    for (size_t i = 5; i < 10; i++)
    {
        if ((bool)ar2[i])
        {
            EXPECT_EQ(Dot(1), Dot(arOr[i]));
        }
        else
        {
            EXPECT_EQ(Dot(0), Dot(arOr[i]));
        }
    }

}

TEST(DotsUnitTest, CopyToTest)
{
    DotArray src1 = { 1,1,1,1,1 };
    DotArray dst1 = { 0,0,0,0,0 };
    DotArray dst2 = { 0,0,0 };
    DotArray dst3 = { 0,0,0,0,0,0,0 };
    size_t sz1 = 0, sz2 = 0, sz3 = 0;

    sz1 = src1.CopyTo(dst1, 5);
    sz2 = src1.CopyTo(dst2, 5);
    sz3 = src1.CopyTo(dst3, 5);

    EXPECT_EQ(5ull, dst1.size());
    EXPECT_EQ(3ull, dst2.size());
    EXPECT_EQ(7ull, dst3.size());

    EXPECT_EQ(5ull, sz1);
    EXPECT_EQ(3ull, sz2);
    EXPECT_EQ(5ull, sz3);

    EXPECT_EQ(DotArray({ 1, 1, 1, 1, 1 }), dst1);
    EXPECT_EQ(DotArray({ 1, 1, 1 }), dst2);
    EXPECT_EQ(DotArray({ 1, 1, 1, 1, 1, 0, 0 }), dst3);


    DotArray src2 = { 1,0,1,0,0 };

    sz1 = src2.CopyTo(dst1, 4);
    sz2 = src2.CopyTo(dst2, 4);
    sz3 = src2.CopyTo(dst3, 4);

    EXPECT_EQ(5ull, dst1.size());
    EXPECT_EQ(3ull, dst2.size());
    EXPECT_EQ(7ull, dst3.size());

    EXPECT_EQ(4ull, sz1);
    EXPECT_EQ(3ull, sz2);
    EXPECT_EQ(4ull, sz3);

    EXPECT_EQ(DotArray({ 1, 0, 1, 0, 1 }), dst1);
    EXPECT_EQ(DotArray({ 1, 0, 1 }), dst2);
    EXPECT_EQ(DotArray({ 1, 0, 1, 0, 1, 0, 0 }), dst3);
}

TEST(DotsUnitTest, IncrementTest)
{
    DotArray ar = { 0,0,0 };
    bool res = false;

    res = ar.Inc();
    EXPECT_TRUE(res);
    EXPECT_EQ(DotArray({ 1, 0, 0 }), ar);
    res = ar.Inc();
    EXPECT_TRUE(res);
    EXPECT_EQ(DotArray({ 0, 1, 0 }), ar);
    res = ar.Inc();
    EXPECT_TRUE(res);
    EXPECT_EQ(DotArray({ 1, 1, 0 }), ar);
    res = ar.Inc();
    EXPECT_TRUE(res);
    EXPECT_EQ(DotArray({ 0, 0, 1 }), ar);
    res = ar.Inc();
    EXPECT_TRUE(res);
    EXPECT_EQ(DotArray({ 1, 0, 1 }), ar);
    res = ar.Inc();
    EXPECT_TRUE(res);
    EXPECT_EQ(DotArray({ 0, 1, 1 }), ar);
    res = ar.Inc();
    EXPECT_TRUE(res);
    EXPECT_EQ(DotArray({ 1, 1, 1 }), ar);
    res = ar.Inc();
    EXPECT_FALSE(res);
    EXPECT_EQ(DotArray({ 1, 1, 1 }), ar);

}

TEST(DotsUnitTest, LetterSizeTest)
{
    DotArray da = TestTools::GenerateRandomDotVector(16 + 3);
    EXPECT_EQ(2ull, da.GetSizeInLetters());

    da = TestTools::GenerateRandomDotVector(16 + 16 + 15);
    EXPECT_EQ(3ull, da.GetSizeInLetters());

    da = TestTools::GenerateRandomDotVector(16 + 16 + 16);
    EXPECT_EQ(3ull, da.GetSizeInLetters());

    da = TestTools::GenerateRandomDotVector(16 + 16 + 16 + 1);
    EXPECT_EQ(4ull, da.GetSizeInLetters());
}

TEST(DotsUnitTest, LetterGetTest)
{
    DotArray da = TestTools::GenerateRandomDotVector(16 + 16 + 16 + 1);
    for (size_t i = 16; i < 16 + 16; i++)
    {
        da[i] = 0;
    }
    da[16] = 1;
    da[16 + 16] = 1;
    Letter res = da.GetLetter(1);

    EXPECT_EQ((unsigned short)1, res);
}

TEST(DotsUnitTest, LetterSetTest)
{
    DotArray da = TestTools::GenerateRandomDotVector(16 + 16 + 16 + 1);
    da.SetLetter(2, 1);

    EXPECT_EQ(Dot(1), (Dot)da[2 * 16]);

    for (size_t i = 2 * 16 + 1; i < 2 * 16 + 16; i++)
    {
        EXPECT_EQ(Dot(0), (Dot)da[i]);
    }

    da.SetLetter(3, BitTraits<Letter>::xFF);
    da.resize(da.size() + 1);
    EXPECT_EQ(Dot(0), (Dot)da[da.size() - 1]);
}

TEST(DotsUnitTest, FromBitsTest)
{
    uint64_t sz = 33;
    uint64_t bits = TestTools::GenerateRandomNumber(0, (int)Pow2(sz));
    DotArray da;
    da.FromBits(bits, sz);

    uint64_t res = da.ToBits();
    EXPECT_EQ(bits, res);

    sz = 4;
    bits = TestTools::GenerateRandomNumber(0, (int)Pow2(sz));

    da.FromBits(bits, sz);
    res = da.ToBits();
    EXPECT_EQ(bits, res);
}

