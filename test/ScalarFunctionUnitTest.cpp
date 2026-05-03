/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "ScalarFunction.h"
#include "TestTools.h"

#include "gtest/gtest.h"

TEST(ScalarFunctionUnitTest, TestLoadingAndApplying)
{
    ScalarFunction f(3);

    //        1,A,B,C,AB,AC,BC,ABC
    f.Load_ForTest({ 1,0,0,1, 1, 0, 0,  0 });
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

    for (size_t i = 0; i < truth_table.size(); i++)
    {
        std::vector<Dot> args = { truth_table[i].begin(), truth_table[i].end() - 1 };
        Dot rightResult = truth_table[i].back();

        Dot functionResult = f(args);
        EXPECT_EQ(rightResult, functionResult);
    }
}

TEST(ScalarFunctionUnitTest, TestOperationsWithFunctions)
{
    ScalarFunction vf[] = { 3,3, 3 };

    //        1,A,B,C,AB,AC,BC,ABC
    vf[0].Load_ForTest({ 1,0,0,1, 1, 0, 0,  0 });
    vf[1].Load_ForTest({ 0,1,0,1, 1, 1, 0,  0 });

    for (size_t i = 0; i < 3; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            ScalarFunction const& f1 = vf[i];
            ScalarFunction const& f2 = vf[j];

            std::vector<std::vector<Dot>> args_permutations = TestTools::GenerateAllPermulations<std::vector<Dot>>(3);

            auto fAnd2 = f1.bitwiseAnd_ForTest(f2);
            ScalarFunction fAnd = f1 & f2;
            ScalarFunction fXor = f1 ^ f2;
            auto fXor2 = f1.bitwiseXor_ForTest(f2);

            for (auto&& args : args_permutations)
            {
                Dot res1 = f1(args);
                Dot res2 = f2(args);
                auto resAnd = fAnd(args);
                auto resXor = fXor(args);
                auto resAnd2 = fAnd2(args);
                auto resXor2 = fXor2(args);


                EXPECT_EQ(res1 & res2, resAnd);
                EXPECT_EQ(res1 & res2, resAnd2);
                EXPECT_EQ(res1 ^ res2, resXor);
                EXPECT_EQ(res1 ^ res2, resXor2);
            }
        }
    }
}

TEST(ScalarFunctionUnitTest, TestOperationsWithDots)
{
    ScalarFunction f(3);

    //        1,A,B,C,AB,AC,BC,ABC
    f.Load_ForTest({ 1,0,0,1, 1, 0, 0,  0 });

    std::vector<std::vector<Dot>> args_permutations = TestTools::GenerateAllPermulations<std::vector<Dot>>(3);

    std::vector<Dot> dots = { 0, 1 };

    for (auto d : dots)
    {
        ScalarFunction fAnd1 = f.Copy_ForTest();
        ScalarFunction fXor1 = f.Copy_ForTest();

        fAnd1 &= d;
        fXor1 ^= d;

        for (auto&& args : args_permutations)
        {
            Dot res = f(args);

            EXPECT_EQ(res & d, fAnd1(args));
            EXPECT_EQ(res ^ d, fXor1(args));
        }
    }
}


TEST(ScalarFunctionUnitTest, TestApplyOnBitfields)
{
    ScalarFunction f(3);
    //        1,A,B,C,AB,AC,BC,ABC
    f.Load_ForTest({ 1,0,0,1, 1, 0, 0,  0 });

    std::vector<std::vector<Dot>> args_permutations = TestTools::GenerateAllPermulations<std::vector<Dot>>(3);

    for (auto&& args : args_permutations)
    {
        Dot res = f(args);

        uint64_t bitfieldArgs = TestTools::DotsToBits(args);

        auto resBitfield = f(bitfieldArgs);

        EXPECT_EQ(res, resBitfield);
    }
}

TEST(ScalarFunctionUnitTest, TestFunctionsOverFunctions)
{
    ScalarFunction f[2] = { 3,3 };
    ScalarFunction vecF[] = { 3,3,3 };


    //             1,A,B,C,AB,AC,BC,ABC
    f[0].Load_ForTest({ 1,0,0,1, 1, 0, 0,  0 });
    vecF[0].Load_ForTest({ 0,0,0,1, 0, 1, 0,  1 });
    vecF[1].Load_ForTest({ 0,1,0,1, 0, 0, 0,  1 });

    std::vector<std::vector<Dot>> args_permutations = TestTools::GenerateAllPermulations<std::vector<Dot>>(3);

    for (size_t i = 0; i < 2; i++)
    {
        auto megaF = f[i](vecF);
        EXPECT_EQ(3LL, megaF.Arity());

        for (auto&& args : args_permutations)
        {
            std::vector<Dot> vecRes(3);
            for (size_t i = 0; i < 3; i++)
            {
                vecRes[i] = vecF[i](args);
            }

            Dot finalVecRes = f[i](vecRes);

            Dot megaRes = megaF(args);

            EXPECT_EQ(finalVecRes, megaRes);
        }

    }
}

TEST(ScalarFunctionUnitTest, LargeRandomTestFunctionsOverFunctions)
{
    uint64_t argsCount1 = 5;
    uint64_t argsCount2 = 3;
    ScalarFunction f = TestTools::GenerateRandomScalarFunction(argsCount1);

    std::vector<ScalarFunction> vecF;
    for (size_t i = 0; i < argsCount1; i++)
    {
        vecF.push_back(TestTools::GenerateRandomScalarFunction(argsCount2));
    }

    std::vector<std::vector<Dot>> args_permutations = TestTools::GenerateAllPermulations<std::vector<Dot>>(argsCount2);

    auto superF = f(vecF);
    EXPECT_EQ(argsCount2, superF.Arity());

    for (auto&& args : args_permutations)
    {
        std::vector<Dot> vecRes(argsCount1);
        for (size_t i = 0; i < argsCount1; i++)
        {
            vecRes[i] = vecF[i](args);
        }

        Dot finalVecRes = f(vecRes);

        Dot megaRes = superF(args);

        EXPECT_EQ(finalVecRes, megaRes);
    }
}

TEST(ScalarFunctionUnitTest, ReduceArityOnOperations)
{
    uint64_t argsCount1 = 3;
    uint64_t argsCount2 = 5; // must be more then argsCount1


    ScalarFunction f1 = TestTools::GenerateRandomScalarFunction(argsCount1);
    ScalarFunction f2 = TestTools::GenerateRandomScalarFunction(argsCount2);

    ScalarFunction fXor = f1 ^ f2;
    ScalarFunction fAnd = f1 & f2;
    EXPECT_EQ(argsCount1, fXor.Arity());
    EXPECT_EQ(argsCount1, fAnd.Arity());

    std::vector<std::vector<Dot>> args_permutations = TestTools::GenerateAllPermulations<std::vector<Dot>>(argsCount1);
    for (auto&& args : args_permutations)
    {
        std::vector<Dot> args2 = args;
        for (size_t i = argsCount1; i < argsCount2; i++)
        {
            args2.push_back(Dot(0));
        }

        Dot res1 = f1(args);
        Dot res2 = f2(args2);
        Dot resXor = fXor(args);
        Dot resAnd = fAnd(args);

        EXPECT_EQ(res1 ^ res2, resXor);

        EXPECT_EQ(res1 & res2, resAnd);
    }
}

TEST(ScalarFunctionUnitTest, TestFunctionsOverPointerFunctions)
{
    ScalarFunction f[2] = { 3,3 };
    std::vector<ScalarFunction*> vecF(3);

    ScalarFunction arrayF[3] = { 3,3,3 };

    for (size_t i = 0; i < 3; i++)
    {
        vecF[i] = &arrayF[i];
    }

    //              1,A,B,C,AB,AC,BC,ABC
    f[0].Load_ForTest({ 1,0,0,1, 1, 0, 0,  0 });
    vecF[0]->Load_ForTest({ 0,0,0,1, 0, 1, 0,  1 });
    vecF[1]->Load_ForTest({ 0,1,0,1, 0, 0, 0,  1 });
    vecF[2]->Load_ForTest({ 0,0,1,0, 1, 1, 0,  0 });

    std::vector<std::vector<Dot>> args_permutations = TestTools::GenerateAllPermulations<std::vector<Dot>>(3);

    for (uint64_t i = 0; i < 2; ++i)
    {
        auto megaF = f[i](vecF);
        EXPECT_EQ(3LL, megaF.Arity());

        for (auto&& args : args_permutations)
        {
            std::vector<Dot> vecRes(3);
            for (size_t i = 0; i < 3; i++)
            {
                vecRes[i] = (*vecF[i])(args);
            }

            Dot finalVecRes = f[i](vecRes);

            Dot megaRes = megaF(args);

            EXPECT_EQ(finalVecRes, megaRes);
        }
    }
}

TEST(ScalarFunctionUnitTest, IsConstTest)
{
    ScalarFunction f(3);
    bool isConst = false;

    f.Load_ForTest({ 1,0,0,1, 1, 0, 0,  0 });
    isConst = f.isConstant();

    EXPECT_FALSE(isConst);

    f.Load_ForTest({ 1,0,0,0, 0, 0, 0,  0 });
    isConst = f.isConstant();

    EXPECT_TRUE(isConst);

    f.Load_ForTest({ 0,0,0,0, 0, 0, 0,  0 });
    isConst = f.isConstant();

    EXPECT_TRUE(isConst);

}
