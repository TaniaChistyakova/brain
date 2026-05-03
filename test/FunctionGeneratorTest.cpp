/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "FunctionGenerator.h"



#include "TestTools.h"

#include "gtest/gtest.h"


TEST(FunctionGeneratorTest, FullTruthTableTest)
{
    std::random_device randDev;
    std::mt19937 generator(randDev());
    ScalarFunction fOrig(3);

    //        1,A,B,C,AB,AC,BC,ABC
    fOrig.Load_ForTest({ 1,0,0,1, 1, 0, 0,  0 });
    //table of all the values
    std::vector<std::vector<Dot>> truth_table =
    {
        //A, B, C, f
        { 0, 0, 0, 1 },
        { 0, 0, 1, 0 },
        { 0, 1, 0, 1 },
        { 0, 1, 1, 0 },
        { 1, 0, 0, 1 },
        { 1, 0, 1, 0 },
        { 1, 1, 0, 0 },
        { 1, 1, 1, 1 }
    };

    ScalarFunctionGenerator gen(3);

    gen.Begin(generator);
    for (size_t i = 0; i < truth_table.size(); i++)
    {
        std::vector<Dot> args = { truth_table[i].begin(), truth_table[i].end() - 1 };
        Dot rightResult = truth_table[i].back();
        DotArray argsAr = TestTools::VectorToDotArray(args);

        gen.InsertTruthTableValue(rightResult, argsAr);
    }
    ScalarFunction fGen = gen.End();

    EXPECT_EQ(fOrig.Arity(), fGen.Arity());

    auto args_permutations = TestTools::GenerateAllPermulations<std::vector<Dot>>(3);
    for (auto const& args : args_permutations)
    {
        Dot dOrig = fOrig(args);
        Dot dGen = fGen(args);

        EXPECT_EQ(dOrig, dGen);
    }
}


TEST(FunctionGeneratorTest, RelaxedTruthTableTest)
{
    std::random_device randDev;
    std::mt19937 generator(randDev());
    ScalarFunction fOrig(3);

    //        1,A,B,C,AB,AC,BC,ABC
    fOrig.Load_ForTest({ 1,0,0,1, 1, 0, 0,  0 });
    //table of all the values
    std::vector<std::vector<Dot>> truth_table =
    {
        //A, B, C, f
        { 0, 0, 0, 1 },
        { 0, 0, 1, 0 },
        { 0, 1, 0, 1 },
        { 0, 1, 1, 0 },
        { 1, 0, 0, 1 },
        { 1, 0, 1, 0 },
        { 1, 1, 0, 0 },
        { 1, 1, 1, 1 }
    };

    ScalarFunctionGenerator gen(3);

    gen.Begin(generator);
    for (size_t i = 0; i < truth_table.size(); i++)
    {
        if (i % 3)
        {
            continue;
        }

        std::vector<Dot> args = { truth_table[i].begin(), truth_table[i].end() - 1 };
        Dot rightResult = truth_table[i].back();
        DotArray argsAr = TestTools::VectorToDotArray(args);

        gen.InsertTruthTableValue(rightResult, argsAr);
    }
    ScalarFunction fGen = gen.End();

    EXPECT_EQ(fOrig.Arity(), fGen.Arity());

    auto args_permutations = TestTools::GenerateAllPermulations<std::vector<Dot>>(3);
    for (size_t i = 0; i < args_permutations.size(); i++)
    {
        if (i % 3)
        {
            continue;
        }

        auto const& args = args_permutations[i];

        Dot dOrig = fOrig(args);
        Dot dGen = fGen(args);

        EXPECT_EQ(dOrig, dGen);
    }
}


TEST(FunctionGeneratorTest, VectorFunctionTest)
{
    auto args_permutations = TestTools::GenerateAllPermulations<DotArray>(3);
    std::vector<DotArray> results;
    results.reserve(args_permutations.size());
    for (size_t i = 0; i < args_permutations.size(); i++)
    {
        results.emplace_back(TestTools::GenerateRandomDotVector(3));
    }
    std::random_device randDev;
    std::mt19937 generator(randDev());

    VectorFunctionGenerator gen(3, 3);
    gen.Begin(generator);

    for (size_t i = 0; i < args_permutations.size(); i++)
    {
        gen.InsertTruthTableValue(results[i], args_permutations[i]);
    }

    VectorFunction::Ptr f = gen.End();

    EXPECT_EQ(3LL, f->Arity());
    EXPECT_EQ(3LL, f->Dimensions());

    for (size_t i = 0; i < args_permutations.size(); i++)
    {
        DotArray const& args = args_permutations[i];
        DotArray res = (*f)(args);
        DotArray const& orig = results[i];
        EXPECT_EQ(orig, res);
    }
}


TEST(FunctionGeneratorTest, ReluxedVectorFunctionTest)
{
    auto args_permutations = TestTools::GenerateAllPermulations<DotArray>(3);
    std::vector<DotArray> results;
    results.reserve(args_permutations.size());
    for (size_t i = 0; i < args_permutations.size(); i++)
    {
        results.emplace_back(TestTools::GenerateRandomDotVector(3));
    }
    std::random_device randDev;
    std::mt19937 generator(randDev());

    VectorFunctionGenerator gen(3, 3);
    gen.Begin(generator);

    for (size_t i = 0; i < args_permutations.size(); i++)
    {
        if (i % 2)
        {
            continue;
        }
        gen.InsertTruthTableValue(results[i], args_permutations[i]);
    }

    VectorFunction::Ptr f = gen.End();

    EXPECT_EQ(3LL, f->Arity());
    EXPECT_EQ(3LL, f->Dimensions());

    for (size_t i = 0; i < args_permutations.size(); i++)
    {
        if (i % 2)
        {
            continue;
        }
        DotArray const& args = args_permutations[i];
        DotArray res = (*f)(args);
        DotArray const& orig = results[i];
        EXPECT_EQ(orig, res);
    }
}


TEST(FunctionGeneratorTest, RandomiserTest)
{
    std::mt19937 random;
    std::random_device randDev;
    random.seed(randDev());

    VectorFunctionGenerator gen(3, 4);
    VectorFunction::Ptr f = gen.GenRandom(random);

    EXPECT_EQ(4LL, f->Arity());
    EXPECT_EQ(3LL, f->Dimensions());

    auto args_permutations = TestTools::GenerateAllPermulations<DotArray>(4);
    for (size_t i = 0; i < args_permutations.size(); i++)
    {
        DotArray const& args = args_permutations[i];
        DotArray res = (*f)(args);
        EXPECT_EQ(3LL, res.size());
    }
}

