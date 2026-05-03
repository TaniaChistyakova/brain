/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "HashCalculator.h"
#include "FunctionGenerator.h"

#include "TestTools.h"

#include "gtest/gtest.h"


TEST(HashTest, SmokeTest)
{
    std::random_device randDev;
    std::mt19937 rndGenerator(randDev());

    Mask mask(43LL);
    mask.dots = DotArray::GenerateRandom(43LL, rndGenerator);
    mask.cares = DotArray::GenerateRandom(43LL, rndGenerator);

    HashCalculator calc;
    calc.InitRandom();

    Word hash = calc.Hash_ForTest(mask);
}

TEST(HashTest, CollisionTest)
{
    std::random_device randDev;
    std::mt19937 rndGenerator(randDev());

    Mask mask1(43LL), mask2(43LL);
    mask1.dots = DotArray::GenerateRandom(43LL, rndGenerator);
    mask1.cares = DotArray::GenerateRandom(43LL, rndGenerator);

    mask2.dots = mask1.dots;
    mask2.cares = mask1.cares;

    mask2.dots[13] ^= 1;

    HashCalculator calc;
    calc.InitRandom();

    Word hash1 = calc.Hash_ForTest(mask1);
    Word hash2 = calc.Hash_ForTest(mask2);

    EXPECT_FALSE(hash1 == hash2);
}

TEST(HashTest, FunctionTest)
{
    std::mt19937 random;
    std::random_device randDev;
    random.seed(randDev());

    VectorFunctionGenerator gen(3, 4);
    VectorFunction::Ptr f1 = gen.GenRandom(random);
    VectorFunction::Ptr f2 = gen.GenRandom(random);

    HashCalculator calc;
    calc.InitRandom();

    Word hash1 = f1->CalcHash(calc);
    Word hash2 = f2->CalcHash(calc);

    EXPECT_NE(hash1, hash2);
}

TEST(HashTest, ProjectorTest)
{
    Projector projA = TestTools::GenerateRandomProjector(5, 15, false);
    Projector projC = TestTools::GenerateRandomProjector(5, 15, false);
    Projector projB = projA;
    projB.Pack();

    HashCalculator calc;
    calc.InitRandom();

    Word hash1 = projA.CalcHash(calc);
    Word hash2 = projB.CalcHash(calc);
    Word hash3 = projC.CalcHash(calc);

    EXPECT_NE(hash1, hash2);
    EXPECT_NE(hash1, hash3);
    EXPECT_NE(hash2, hash3);

}

TEST(HashTest, PackedProjectorTest)
{
    Projector projA = TestTools::GenerateRandomProjector(5, 15, false);
    Projector projB = projA;
    Projector projB1 = projA;
    projB.Pack();
    projB1.Pack();

    HashCalculator calc;
    calc.InitRandom();

    Word hash1 = projB.CalcHash(calc);
    Word hash2 = projB1.CalcHash(calc);

    EXPECT_EQ(hash1, hash2);
}

TEST(HashTest, SentenceTest)
{
    Sentence s1 = TestTools::GenerateRandomSentence(10, 3);
    Sentence s2 = TestTools::GenerateRandomSentence(10, 3);

    HashCalculator calc;
    calc.InitRandom();

    Word hash1 = s1.CalcHash(calc);
    Word hash2 = s2.CalcHash(calc);

    EXPECT_NE(hash1, hash2);
}

