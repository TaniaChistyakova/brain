/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "Functor.h"

#include "TestTools.h"

#include "gtest/gtest.h"


TEST(FunctorTest, MathUnitTest)
{
    Functor f = TestTools::GenerateRandomScalarFunction(3);
    std::vector<std::vector<Dot> > vDots;
    std::vector<DotArray> da;

    for (size_t i = 0; i < 3; i++)
    {
        std::vector<Dot> rand = TestTools::GenerateRandomDotVector(10);
        vDots.push_back(rand);
        da.push_back(rand);
    }

    DotArray daRes = f(da);

    EXPECT_EQ(10ull, daRes.size());

    for (size_t i = 0; i < daRes.size(); i++)
    {
        std::vector<Dot> args;
        for (size_t j = 0; j < 3; j++)
        {
            args.push_back(vDots[j][i]);
        }

        Dot dotRes = f(args);

        EXPECT_EQ(dotRes, Dot(daRes[i]));
    }
}

TEST(FunctorTest, ProjectorTest)
{
    Functor functorVec[] = { ScalarFunction(3), TestTools::GenerateRandomScalarFunction(3) };

    for (size_t functorIdx = 1; functorIdx < 2; ++functorIdx)
    {
        const auto& functor = functorVec[functorIdx];

        std::vector<Projector::CPtr> functorArgs;
        functorArgs.reserve(3);
        for (size_t i = 0; i < 3; i++)
        {
            functorArgs.push_back(std::make_shared<Projector>(TestTools::GenerateRandomProjector(5, 3)));
        }

        Projector::Ptr functorRes = functor(functorArgs);

        auto args_permutations = TestTools::GenerateAllPermulations<DotArray>(5);
        for (auto&& args : args_permutations)
        {
            DotArray naiveArgs(functorArgs.size());
            for (size_t i = 0; i < functorArgs.size(); i++)
            {
                if (functorArgs[i]->Search(args) != nullptr)
                {
                    naiveArgs[i] = Dot(1);
                }
            }
            Dot naiveRes = functor(naiveArgs);
            const Mask* foundMask = functorRes->Search(args);

            if (naiveRes)
            {
                EXPECT_NE(foundMask, nullptr);
            }
            else
            {
                EXPECT_EQ(foundMask, nullptr);
            }
        }
    }
}

TEST(FunctorTest, VectorFunctionUnitTest)
{
    Functor functorVec[] = { ScalarFunction(5), TestTools::GenerateRandomScalarFunction(5) };

    for (size_t functorIdx = 0; functorIdx < 2; ++functorIdx)
    {
        const auto& functor = functorVec[functorIdx];

        std::vector<VectorFunction::CPtr> functorArgs;
        functorArgs.reserve(5);
        for (size_t i = 0; i < 5; i++)
        {
            functorArgs.push_back(std::make_shared<VectorFunction>(TestTools::GenerateRandomVectorFunction(3, 4)));
        }

        VectorFunction::Ptr functorRes = functor(functorArgs);

        EXPECT_EQ(3LL, functorRes->Dimensions());
        EXPECT_EQ(4LL, functorRes->Arity());

        auto args_permutations = TestTools::GenerateAllPermulations<DotArray>(4);
        for (auto&& args : args_permutations)
        {
            DotArray functorDots = (*functorRes)(args);

            EXPECT_EQ(3LL, functorDots.size());

            std::vector<DotArray> vDotArray(5);

            for (size_t i = 0; i < 5; i++)
            {
                vDotArray[i] = (*functorArgs[i])(args);
            }

            DotArray origDots(3);
            for (size_t i = 0; i < 3; i++)
            {
                std::vector<Dot> origDotsToFunctor(5);
                for (size_t j = 0; j < 5; ++j)
                {
                    origDotsToFunctor[j] = (Dot)vDotArray[j][i];
                }

                origDots[i] = functor(origDotsToFunctor);
            }

            EXPECT_EQ(origDots, functorDots);
        }
    }
}

