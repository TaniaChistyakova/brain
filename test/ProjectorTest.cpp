/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "Projector.h"

#include "TestTools.h"

#include "gtest/gtest.h"

TEST(ProjectorTest, DotArraySmokeTest)
{
    Projector tree;
    Mask m[] = { 5, 5, 5 };

    m[0].Load_ForTest({ 1,1,1,1,1 }, { 1,1,1,1,1 });
    m[1].Load_ForTest({ 1,0,1,1,1 }, { 1,1,1,1,1 });
    m[2].Load_ForTest({ 1,1,0,0,1 }, { 1,1,1,1,1 });

    for (size_t i = 0; i < 3; i++)
    {
        tree.Insert_ForTest(m[i]);
    }

    EXPECT_EQ(ContextObject::Type::Projector, tree.GetType());

    DotArray dataToSearch = { 0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,/*begin of pattern*/1,0,1,1,1,0,1,0,1,0,0 };

    size_t i;
    const Mask* mRes = nullptr;
    for (i = 0; i < dataToSearch.size(); ++i)
    {
        mRes = tree.Search(DotArrayView(dataToSearch, i));
        if (mRes)
        {
            break;
        }
    }

    EXPECT_EQ(19LL, i);
    EXPECT_EQ(m[1], *mRes);
}

TEST(ProjectorTest, DotArrayDirectTest)
{
    Projector tree;
    Mask m[] = { 5, 5, 5 };

    m[0].Load_ForTest({ 1,1,1,1,1 }, { 1,1,1,1,1 });
    m[1].Load_ForTest({ 1,0,1,1,1 }, { 1,1,1,1,1 });
    m[2].Load_ForTest({ 1,1,0,0,1 }, { 1,1,1,1,1 });

    for (size_t i = 0; i < 3; i++)
    {
        tree.Insert_ForTest(m[i]);
    }

    DotArray dataToSearch = { 1,0,1,1,1,0,1,0,1,0,0 };


    const Mask* mRes = tree.Search(dataToSearch);

    EXPECT_EQ(m[1], *mRes);
}

TEST(ProjectorTest, ShortMaskWinsTest)
{
    Projector tree;
    Mask m[] = { 3, 5, 7 };

    m[0].Load_ForTest({ 1,1,1 }, { 1,1,1 });
    m[1].Load_ForTest({ 1,1,1,1,1 }, { 1,1,1,1,1 });
    m[2].Load_ForTest({ 1,1,1,1,1,1,1 }, { 1,1,1,1,1,1,1 });

    for (size_t i = 0; i < 3; i++)
    {
        tree.Insert_ForTest(m[i]);
    }

    DotArray dataToSearch = { 0,0,0,0,0,0,1,1,1,1,1,1,1,0,0 };

    size_t i;
    const Mask* mRes = nullptr;
    for (i = 0; i < dataToSearch.size(); ++i)
    {
        mRes = tree.Search(DotArrayView(dataToSearch, i));
        if (mRes)
        {
            break;
        }
    }

    EXPECT_EQ(6LL, i);
    EXPECT_EQ(m[0], *mRes);
}

TEST(ProjectorTest, ButShorterMaskLosesTest)
{
    Projector tree;
    Mask m[] = { 5 };
    m[0].Load_ForTest({ 1,1,1,1,1 }, { 1,1,1,1,1 });

    for (size_t i = 0; i < 1; i++)
    {
        tree.Insert_ForTest(m[i]);
    }

    DotArray dataToSearch = { 0,0,0,0,0,0,1,1,1,1,0,0 };

    size_t i;
    const Mask* mRes = nullptr;
    for (i = 0; i < dataToSearch.size(); ++i)
    {
        mRes = tree.Search(DotArrayView(dataToSearch, i));
        if (mRes)
        {
            break;
        }
    }

    EXPECT_EQ(dataToSearch.size(), i);
    EXPECT_EQ(mRes, nullptr);
}


TEST(ProjectorTest, CareTest)
{
    Projector tree;
    Mask m[] = { 5, 5, 5 };

    m[0].Load_ForTest({ 1,1,1,1,1 }, { 1,0,1,1,1 });
    m[1].Load_ForTest({ 1,1,1,1,1 }, { 1,1,0,1,1 });
    m[2].Load_ForTest({ 1,1,1,1,1 }, { 1,1,1,0,1 });

    for (size_t i = 0; i < 3; i++)
    {
        tree.Insert_ForTest(m[i]);
    }

    DotArray dataToSearch = { 0,0,0,0,0,0,1,1,0,1,1,1,1,0,0 };

    size_t i;
    const Mask* mRes = nullptr;
    for (i = 0; i < dataToSearch.size(); ++i)
    {
        mRes = tree.Search(DotArrayView(dataToSearch, i));
        if (mRes)
        {
            break;
        }
    }

    EXPECT_EQ(6LL, i);
    EXPECT_EQ(m[1], *mRes);
}


TEST(ProjectorTest, ProjectorXorTest)
{
    Projector projA;
    {
        Mask m[] = { 5, 5, 5 };

        m[0].Load_ForTest({ 1,1,1,1,1 }, { 1,1,1,1,1 });
        m[1].Load_ForTest({ 1,0,1,1,1 }, { 1,1,0,1,1 });
        m[2].Load_ForTest({ 1,1,0,0,1 }, { 1,1,1,0,1 });

        for (size_t i = 0; i < 3; i++)
        {
            projA.Insert_ForTest(m[i]);
        }
    }

    Projector projB;
    {
        Mask m[] = { 5, 5, 5 };

        m[0].Load_ForTest({ 1,1,1,1,0 }, { 1,0,1,1,1 });
        m[1].Load_ForTest({ 1,0,1,1,0 }, { 1,1,0,0,1 });
        m[2].Load_ForTest({ 1,1,0,0,1 }, { 0,1,1,0,1 });

        for (size_t i = 0; i < 3; i++)
        {
            projB.Insert_ForTest(m[i]);
        }
    }

    Projector projXor = projA ^ projB;

    auto args_permutations = TestTools::GenerateAllPermulations<DotArray>(5);
    for (auto&& args : args_permutations)
    {
        bool resA = false;
        bool resB = false;
        bool resXor = false;

        {
            auto mRes = projA.Search(DotArrayView(args, 0));
            if (mRes)
            {
                resA = true;
            }
        }

        {
            auto mRes = projB.Search(DotArrayView(args, 0));
            if (mRes)
            {
                resB = true;
            }
        }

        {
            auto mRes = projXor.Search(DotArrayView(args, 0));
            if (mRes)
            {
                resXor = true;
            }
        }

        if ((resA || resB) && !(resA && resB))
        {
            EXPECT_TRUE(resXor);
        }
        else
        {
            EXPECT_FALSE(resXor);
        }
    }

}

TEST(ProjectorTest, ProjectorAndTest)
{
    Projector projA;
    {
        Mask m[] = { 5, 5, 5 };

        m[0].Load_ForTest({ 1,1,1,1,1 }, { 1,1,1,1,1 });
        m[1].Load_ForTest({ 1,0,1,1,1 }, { 1,1,0,1,1 });
        m[2].Load_ForTest({ 1,1,0,0,1 }, { 1,1,1,0,1 });

        for (size_t i = 0; i < 3; i++)
        {
            projA.Insert_ForTest(m[i]);
        }
    }

    Projector projB;
    {
        Mask m[] = { 5, 5, 5 };

        m[0].Load_ForTest({ 1,1,1,1,0 }, { 1,0,1,1,1 });
        m[1].Load_ForTest({ 1,0,1,1,0 }, { 1,1,0,0,1 });
        m[2].Load_ForTest({ 1,1,0,0,1 }, { 0,1,1,0,1 });

        for (size_t i = 0; i < 3; i++)
        {
            projB.Insert_ForTest(m[i]);
        }
    }

    Projector projAnd = projA & projB;

    auto args_permutations = TestTools::GenerateAllPermulations<DotArray>(5);
    for (auto&& args : args_permutations)
    {
        bool resA = false;
        bool resB = false;
        bool resAnd = false;

        {
            auto mRes = projA.Search(DotArrayView(args, 0));
            if (mRes)
            {
                resA = true;
            }
        }

        {
            auto mRes = projB.Search(DotArrayView(args, 0));
            if (mRes)
            {
                resB = true;
            }
        }

        {
            auto mRes = projAnd.Search(DotArrayView(args, 0));
            if (mRes)
            {
                resAnd = true;
            }
        }

        if (resA && resB)
        {
            EXPECT_TRUE(resAnd);
        }
        else
        {
            EXPECT_FALSE(resAnd);
        }
    }

}


TEST(ProjectorTest, ProjectorOperationTest)
{
    Projector projA = TestTools::GenerateRandomProjector(5, 4);
    Projector projB = TestTools::GenerateRandomProjector(5, 4);

    Projector projAnd = projA & projB;
    Projector projXor = projA ^ projB;

    auto args_permutations = TestTools::GenerateAllPermulations<DotArray>(5);
    for (auto&& args : args_permutations)
    {
        bool resA = (projA.Search(args) != nullptr);
        bool resB = (projB.Search(args) != nullptr);
        bool resAnd = (projAnd.Search(args) != nullptr);
        bool resXor = (projXor.Search(args) != nullptr);

        EXPECT_EQ((bool)(resA && resB), resAnd);
        EXPECT_EQ((bool)(!resA != !resB), resXor);
    }

}


TEST(ProjectorTest, PackTest)
{
    Projector projA;
    Projector projB;
    {
        Mask m[] = { 5, 5, 5, 5 };

        m[0].Load_ForTest({ 1,1,1,1,1 }, { 1,1,1,1,1 });
        m[1].Load_ForTest({ 1,0,1,1,1 }, { 1,1,1,1,1 });
        m[2].Load_ForTest({ 1,0,0,1,1 }, { 1,1,1,1,1 });
        m[3].Load_ForTest({ 1,1,0,1,1 }, { 1,1,1,1,1 });

        for (size_t i = 0; i < 4; i++)
        {
            projA.Insert_ForTest(m[i]);
            projB.Insert_ForTest(m[i]);
        }
    }

    projB.Pack();

    EXPECT_EQ(1ull, projB.GetMaskCount_ForTest());

    auto args_permutations = TestTools::GenerateAllPermulations<DotArray>(5);
    for (auto&& args : args_permutations)
    {
        bool resA = false;
        bool resB = false;

        {
            auto mRes = projA.Search(DotArrayView(args, 0));
            if (mRes)
            {
                resA = true;
            }
        }

        {
            auto mRes = projB.Search(DotArrayView(args, 0));
            if (mRes)
            {
                resB = true;
            }
        }

        EXPECT_EQ(resA, resB);
    }

}

TEST(ProjectorTest, PackTest1)
{
    Projector projA;
    Projector projB;
    {
        Mask m[] = { 5, 5, 5, 5 };

        m[0].Load_ForTest({ 0,0,0,0,1 }, { 0,0,0,0,1 });
        m[1].Load_ForTest({ 0,0,0,0,1 }, { 0,0,0,1,1 });
        m[2].Load_ForTest({ 0,0,0,0,1 }, { 0,0,1,1,1 });
        m[3].Load_ForTest({ 0,0,0,0,1 }, { 0,1,1,1,1 });

        for (size_t i = 0; i < 4; i++)
        {
            projA.Insert_ForTest(m[i]);
            projB.Insert_ForTest(m[i]);
        }
    }

    projB.Pack();

    auto args_permutations = TestTools::GenerateAllPermulations<DotArray>(5);
    for (auto&& args : args_permutations)
    {
        bool resA = false;
        bool resB = false;

        auto mResA = projA.Search(args);
        if (mResA)
        {
            resA = true;
        }

        auto mResB = projB.Search(args);
        if (mResB)
        {
            resB = true;
        }

        EXPECT_EQ(resA, resB);
    }
}

TEST(ProjectorTest, PackTest2)
{
    Projector projA;
    Projector projB;
    {
        Mask m[] = { 5, 5, 5, 5 };

        m[0].Load_ForTest({ 1,1,1,1,1 }, { 1,1,1,1,1 });
        m[1].Load_ForTest({ 1,1,1,1,1 }, { 1,0,1,1,1 });
        m[2].Load_ForTest({ 1,1,1,1,1 }, { 1,1,1,1,1 });
        m[3].Load_ForTest({ 1,1,1,1,1 }, { 1,1,0,1,1 });

        for (size_t i = 0; i < 4; i++)
        {
            projA.Insert_ForTest(m[i]);
            projB.Insert_ForTest(m[i]);
        }
    }

    projB.Pack();

    EXPECT_EQ(2ull, projB.GetMaskCount_ForTest());

    auto args_permutations = TestTools::GenerateAllPermulations<DotArray>(5);
    for (auto&& args : args_permutations)
    {
        bool resA = false;
        bool resB = false;

        {
            auto mRes = projA.Search(DotArrayView(args, 0));
            if (mRes)
            {
                resA = true;
            }
        }

        {
            auto mRes = projB.Search(DotArrayView(args, 0));
            if (mRes)
            {
                resB = true;
            }
        }

        EXPECT_EQ(resA, resB);
    }

}

TEST(ProjectorTest, PackTest3)
{
    Projector projA;
    Projector projB;
    {
        Mask m[] = { 3, 4, 5, 5 };

        m[0].Load_ForTest({ 1,1,1, }, { 1,1,1, });
        m[1].Load_ForTest({ 1,0,1,1 }, { 1,1,1,1 });
        m[2].Load_ForTest({ 1,1,1,1,1 }, { 1,1,1,1,1 });
        m[3].Load_ForTest({ 1,0,1,1,0 }, { 1,1,1,1,1 });

        for (size_t i = 0; i < 4; i++)
        {
            projA.Insert_ForTest(m[i]);
            projB.Insert_ForTest(m[i]);
        }
    }

    projB.Pack();

    auto args_permutations = TestTools::GenerateAllPermulations<DotArray>(5);
    for (auto&& args : args_permutations)
    {
        bool resA = false;
        bool resB = false;

        auto mResA = projA.Search(args);
        if (mResA)
        {
            resA = true;
        }

        auto mResB = projB.Search(args);
        if (mResB)
        {
            resB = true;
        }

        EXPECT_EQ(resA, resB);
    }
}

TEST(ProjectorTest, PackTest4)
{
    Projector projA = TestTools::GenerateRandomProjector(5, 15, false);
    Projector projB = projA;
    projB.Pack();

    auto args_permutations = TestTools::GenerateAllPermulations<DotArray>(5);
    for (auto&& args : args_permutations)
    {
        bool resA = false;
        bool resB = false;

        auto mResA = projA.Search(args);
        if (mResA)
        {
            resA = true;
        }

        auto mResB = projB.Search(args);
        if (mResB)
        {
            resB = true;
        }

        EXPECT_EQ(resA, resB);
    }
}

