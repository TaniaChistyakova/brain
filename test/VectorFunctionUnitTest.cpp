/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "VectorFunction.h"

#include "TestTools.h"

#include "gtest/gtest.h"

TEST(VectorFunctionUnitTest, TestLoadingAndApplying)
{
    std::vector<ScalarFunction> scalars;
    scalars.reserve(4);
    for (size_t i = 0; i < 4; i++)
    {
        scalars.push_back(TestTools::GenerateRandomScalarFunction(3));
    }

    VectorFunction vecF(4, 3);
    vecF.Load_ForTest(scalars);

    EXPECT_EQ(ContextObject::Type::Function, vecF.GetType());

    std::vector<std::vector<Dot>> args_permutations = TestTools::GenerateAllPermulations<std::vector<Dot>>(3);
    for (auto&& args : args_permutations)
    {
        std::vector<Dot> vecRes(4);
        for (size_t i = 0; i < 4; i++)
        {
            vecRes[i] = (scalars[i])(args);
        }

        DotArray res_vecFun = vecF(TestTools::VectorToDotArray(args));
        DotArray res_orig = vecRes;

        EXPECT_EQ(res_orig.size(), res_vecFun.size());
        EXPECT_EQ(res_orig, res_vecFun);
    }
}

TEST(VectorFunctionUnitTest, BinaryOperationsTest)
{
    VectorFunction f1 = TestTools::GenerateRandomVectorFunction(3, 5);
    VectorFunction f2 = TestTools::GenerateRandomVectorFunction(3, 5);
    VectorFunction fAnd = f1 & f2;
    VectorFunction fXor = f1 ^ f2;

    EXPECT_EQ(3LL, fAnd.Dimensions());
    EXPECT_EQ(3LL, fXor.Dimensions());

    EXPECT_EQ(5LL, fAnd.Arity());
    EXPECT_EQ(5LL, fXor.Arity());

    auto args_permutations = TestTools::GenerateAllPermulations<DotArray>(5);
    for (auto&& args : args_permutations)
    {
        auto res1 = f1(args);
        auto res2 = f2(args);
        auto resAnd = fAnd(args);
        auto resXor = fXor(args);

        EXPECT_EQ(3LL, resAnd.size());
        EXPECT_EQ(3LL, resXor.size());

        auto resAndOrig = res1 & res2;
        auto resXorOrig = res1 ^ res2;

        EXPECT_EQ(resAndOrig, resAnd);
        EXPECT_EQ(resXorOrig, resXor);
    }
}

TEST(VectorFunctionUnitTest, ReduceArityTest)
{
    VectorFunction fLong = TestTools::GenerateRandomVectorFunction(4, 5);
    VectorFunction fShort = fLong;
    fShort.ReduceArity_ForTest(3);

    EXPECT_EQ(3LL, fShort.Arity());
    std::vector<std::vector<Dot>> args_permutations = TestTools::GenerateAllPermulations<std::vector<Dot>>(3);
    for (auto&& args : args_permutations)
    {
        std::vector<Dot> resizedArgs(args);
        for (size_t i = 0; i < 2; i++)
        {
            resizedArgs.push_back(Dot(0));
        }

        auto resShort = fShort(TestTools::VectorToDotArray(args));
        auto resLong = fLong(TestTools::VectorToDotArray(resizedArgs));

        EXPECT_EQ(resLong, resShort);
    }
}

TEST(VectorFunctionUnitTest, ReduceDimensionsTest)
{
    VectorFunction fLong = TestTools::GenerateRandomVectorFunction(5, 4);
    VectorFunction fShort = fLong;
    fShort.ReduceDimensions_ForTest(3);

    EXPECT_EQ(3LL, fShort.Dimensions());
    auto args_permutations = TestTools::GenerateAllPermulations<DotArray>(4);
    for (auto&& args : args_permutations)
    {
        auto resShort = fShort(args);
        auto resLong = fLong(args);

        for (size_t i = 0; i < resShort.size(); ++i)
        {
            EXPECT_EQ((Dot)resLong[i], (Dot)resShort[i]);
        }

    }
}

TEST(VectorFunctionUnitTest, FunctionFromViewTest)
{
    VectorFunction f = TestTools::GenerateRandomVectorFunction(3, 4);
    auto begiVec = TestTools::GenerateRandomDotVector(5);
    auto argVec = TestTools::GenerateRandomDotVector(4);
    auto endVec = TestTools::GenerateRandomDotVector(3);


    DotArray arArg = TestTools::VectorToDotArray(argVec);
    std::vector<Dot> bigVec;
    bigVec.reserve(5 + 4 + 3);
    bigVec.insert(bigVec.end(), begiVec.begin(), begiVec.end());
    bigVec.insert(bigVec.end(), argVec.begin(), argVec.end());
    bigVec.insert(bigVec.end(), endVec.begin(), endVec.end());

    DotArray bigAr = TestTools::VectorToDotArray(bigVec);
    DotArrayView view(bigAr, 5, 4);

    DotArray res1 = f(arArg);
    DotArray res2 = f(view);

    EXPECT_EQ(res1.size(), res2.size());
    EXPECT_EQ(res1, res2);

}

TEST(VectorFunctionUnitTest, ReduceArityOnOperations)
{
    uint64_t argsCount1 = 3;
    uint64_t argsCount2 = 5; // must be more then argsCount1
    uint64_t funcDimensions1 = 6;
    uint64_t funcDimensions2 = 8; // must be more then funcDimensions2


    VectorFunction f1 = TestTools::GenerateRandomVectorFunction(funcDimensions1, argsCount1);
    VectorFunction f2 = TestTools::GenerateRandomVectorFunction(funcDimensions2, argsCount2);

    VectorFunction fXor = f1 ^ f2;
    VectorFunction fAnd = f1 & f2;

    EXPECT_EQ(argsCount1, fXor.Arity());
    EXPECT_EQ(argsCount1, fAnd.Arity());

    EXPECT_EQ(funcDimensions1, fXor.Dimensions());
    EXPECT_EQ(funcDimensions1, fAnd.Dimensions());

    std::vector<std::vector<Dot>> args_permutations = TestTools::GenerateAllPermulations<std::vector<Dot>>(argsCount1);
    for (auto&& args : args_permutations)
    {
        std::vector<Dot> args2 = args;
        for (size_t i = argsCount1; i < argsCount2; i++)
        {
            args2.push_back(Dot(0));
        }

        DotArray res1 = f1(TestTools::VectorToDotArray(args));
        DotArray res2 = f2(TestTools::VectorToDotArray(args2));
        DotArray resXor = fXor(TestTools::VectorToDotArray(args));
        DotArray resAnd = fAnd(TestTools::VectorToDotArray(args));

        EXPECT_EQ(res1 ^ res2, resXor);

        EXPECT_EQ(res1 & res2, resAnd);
    }
}

TEST(VectorFunctionUnitTest, ReducedArgsTest)
{
    uint64_t argsCount1 = 3;
    uint64_t argsCount2 = 5; // must be more then argsCount1
    VectorFunction f = TestTools::GenerateRandomVectorFunction(4, argsCount2);

    std::vector<std::vector<Dot>> args_permutations = TestTools::GenerateAllPermulations<std::vector<Dot>>(argsCount1);
    for (auto&& args : args_permutations)
    {
        std::vector<Dot> args2 = args;
        for (size_t i = argsCount1; i < argsCount2; i++)
        {
            args2.push_back(Dot(0));
        }

        DotArray res1 = f(TestTools::VectorToDotArray(args));
        DotArray res2 = f(TestTools::VectorToDotArray(args2));

        EXPECT_EQ(res2, res1);
    }
}

TEST(VectorFunctionUnitTest, IncreasedArgsTest)
{
    uint64_t argsCount1 = 4;
    uint64_t argsCount2 = 6; // must be more then argsCount1
    VectorFunction f = TestTools::GenerateRandomVectorFunction(4, argsCount1);

    std::vector<std::vector<Dot>> args_permutations = TestTools::GenerateAllPermulations<std::vector<Dot>>(argsCount1);
    for (auto&& args : args_permutations)
    {
        std::vector<Dot> args2 = args;
        std::vector<Dot> additionalDots = TestTools::GenerateRandomDotVector(argsCount2 - argsCount1);
        args2.insert(args2.end(), additionalDots.begin(), additionalDots.end());

        DotArray res1 = f(TestTools::VectorToDotArray(args));
        DotArray res2 = f(TestTools::VectorToDotArray(args2));

        EXPECT_EQ(res1, res2);
    }
}

