/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "Sentence.h"
#include "FunctionGenerator.h"

#include "TestTools.h"

#include "gtest/gtest.h"

TEST(SentenseTest, SmokeTest)
{
    Sentence sentense;
    Mask::CPtr nounMask;

    {
        Mask mask1(3);
        mask1.Load_ForTest({ 1,1,1,0,0 }, { 1,1,1,1,1 });
        nounMask = std::make_shared<Mask>(mask1);

        Projector searcher1;
        searcher1.Insert(nounMask);
        sentense.noun = std::make_shared<Projector>(searcher1);
    }

    {
        Mask mask1(3);
        mask1.Load_ForTest({ 1,1,1 }, { 1,1,1 });
        auto maskPtr1 = std::make_shared<Mask>(mask1);

        Projector searcher1;
        searcher1.Insert(maskPtr1);
        sentense.adjectives.push_back(std::make_shared<Projector>(searcher1));
    }

    {
        Mask mask1(3);
        mask1.Load_ForTest({ 1,0,1 }, { 1,1,1 });
        auto maskPtr1 = std::make_shared<Mask>(mask1);

        Projector searcher1;
        searcher1.Insert(maskPtr1);
        sentense.adjectives.push_back(std::make_shared<Projector>(searcher1));
    }

    {
        auto args_permutations = TestTools::GenerateAllPermulations<DotArray>(2);

        std::random_device randDev;
        std::mt19937 generator(randDev());

        VectorFunctionGenerator gen(3, 2);
        gen.Begin(generator);

        for (size_t i = 0; i < args_permutations.size(); i++)
        {
            gen.InsertTruthTableValue(DotArray({ 0,0,0 }), args_permutations[i]);
        }

        gen.InsertTruthTableValue(DotArray({ 0,1,0 }), DotArray({ 1,0 }));

        VectorFunction::Ptr func = gen.End();
        sentense.verb = func;
    }

    EXPECT_EQ(ContextObject::Type::Sentence, sentense.GetType());

    DotArray data = { 1,1,1,0,0 };
    const Mask* noun = sentense.Search(data);

    EXPECT_EQ(*nounMask.get(), *noun);

    DotArray res = sentense.Act(data);

    EXPECT_EQ(3ull, res.Size());
    EXPECT_EQ(DotArray({ 0,1,0 }), res);
}

TEST(SentenseTest, ConvertToProjectorTest)
{
    Sentence sentense;

    {
        Mask mask1(3);
        mask1.Load_ForTest({ 1,1,1,0,0 }, { 1,1,1,1,1 });
        auto maskPtr1 = std::make_shared<Mask>(mask1);

        Projector searcher1;
        searcher1.Insert(maskPtr1);
        sentense.noun = std::make_shared<Projector>(searcher1);
    }

    {
        Mask mask1(3);
        mask1.Load_ForTest({ 1,1,1 }, { 1,1,1 });
        auto maskPtr1 = std::make_shared<Mask>(mask1);

        Projector searcher1;
        searcher1.Insert(maskPtr1);
        sentense.adjectives.push_back(std::make_shared<Projector>(searcher1));
    }

    {
        Mask mask1(3);
        mask1.Load_ForTest({ 1,0,1 }, { 1,1,1 });
        auto maskPtr1 = std::make_shared<Mask>(mask1);

        Projector searcher1;
        searcher1.Insert(maskPtr1);
        sentense.adjectives.push_back(std::make_shared<Projector>(searcher1));
    }

    {
        auto args_permutations = TestTools::GenerateAllPermulations<DotArray>(2);

        std::random_device randDev;
        std::mt19937 generator(randDev());

        VectorFunctionGenerator gen(3, 2);
        gen.Begin(generator);

        for (size_t i = 0; i < args_permutations.size(); i++)
        {
            gen.InsertTruthTableValue(DotArray({ 0,0,0 }), args_permutations[i]);
        }

        gen.InsertTruthTableValue(DotArray({ 0,1,0 }), DotArray({ 1,0 }));

        VectorFunction::Ptr func = gen.End();
        sentense.verb = func;
    }

    Projector::CPtr projector = sentense.ConvertToProjector();

    EXPECT_EQ(3ull, projector->Size());

    std::vector<DotArray> results;
    auto args_permutations = TestTools::GenerateAllPermulations<DotArray>(5);
    for (size_t i = 0; i < args_permutations.size(); i++)
    {
        DotArray const& args = args_permutations[i];

        if (sentense.Search(args) == nullptr)
        {
            continue;
        }

        DotArray data(sentense.Act(args));

        results.push_back(data);
    }

    args_permutations = TestTools::GenerateAllPermulations<DotArray>(3);
    for (size_t i = 0; i < args_permutations.size(); i++)
    {
        DotArray const& args = args_permutations[i];
        auto mask = projector->Search(args);

        auto it = std::find(results.begin(), results.end(), args);
        if (it == results.end())
        {
            EXPECT_EQ(mask, nullptr);
        }
        else
        {
            EXPECT_NE(mask, nullptr);
        }
    }
}

TEST(SentenseTest, ConvertToFunctionTest)
{
    Sentence sentense;

    {
        Mask mask1(3);
        mask1.Load_ForTest({ 1,1,1,0,0 }, { 1,1,1,1,1 });
        auto maskPtr1 = std::make_shared<Mask>(mask1);

        Projector searcher1;
        searcher1.Insert(maskPtr1);
        sentense.noun = std::make_shared<Projector>(searcher1);
    }

    {
        Mask mask1(3);
        mask1.Load_ForTest({ 1,1,1 }, { 1,1,1 });
        auto maskPtr1 = std::make_shared<Mask>(mask1);

        Projector searcher1;
        searcher1.Insert(maskPtr1);
        sentense.adjectives.push_back(std::make_shared<Projector>(searcher1));
    }

    {
        Mask mask1(3);
        mask1.Load_ForTest({ 1,0,1 }, { 1,1,1 });
        auto maskPtr1 = std::make_shared<Mask>(mask1);

        Projector searcher1;
        searcher1.Insert(maskPtr1);
        sentense.adjectives.push_back(std::make_shared<Projector>(searcher1));
    }

    {
        auto args_permutations = TestTools::GenerateAllPermulations<DotArray>(2);

        std::random_device randDev;
        std::mt19937 generator(randDev());

        VectorFunctionGenerator gen(3, 2);
        gen.Begin(generator);

        for (size_t i = 0; i < args_permutations.size(); i++)
        {
            gen.InsertTruthTableValue(DotArray({ 0,0,0 }), args_permutations[i]);
        }

        gen.InsertTruthTableValue(DotArray({ 0,1,0 }), DotArray({ 1,0 }));

        VectorFunction::Ptr func = gen.End();
        sentense.verb = func;
    }

    VectorFunction::CPtr function = sentense.ConvertToFunction();

    EXPECT_EQ(5ull, function->Arity());
    EXPECT_EQ(3ull, function->Dimensions());

    auto args_permutations = TestTools::GenerateAllPermulations<DotArray>(5);

    DotArray zero(Dot(0), sentense.verb->Dimensions());
    for (size_t i = 0; i < args_permutations.size(); i++)
    {
        DotArray const& args = args_permutations[i];
        auto res = (*function)(args);

        if (sentense.Search(args) == nullptr)
        {
            EXPECT_EQ(zero, res);
        }
        else
        {
            DotArray data(sentense.Act(args));

            EXPECT_EQ(data, res);
        }
    }
}

