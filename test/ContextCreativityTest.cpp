/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "ContextCreativity.h"
#include "HashCalculator.h"

#include "TestTools.h"

#include "gtest/gtest.h"

TEST(ContextCreativityTest, SmokeTest)
{
    HashCalculator calc;
    calc.InitRandom();

    std::vector<Context::Ptr> ctxHier(2);

    Context::Ptr global = std::make_shared<Context>();
    ctxHier[0] = global;

    Context::Ptr ctx1 = std::make_shared<Context>(global); // empty ctx in the middle

    Context::Ptr ctx2 = std::make_shared<Context>(ctx1);
    ctxHier[1] = ctx2;

    std::deque<Sentence::CPtr> sentences;

    for (int j = 0; j < 2; ++j)
    {
        for (int i = 0; i < 2; ++i)
        {
            ContextObject::Ptr pProj = std::make_shared<Projector>(TestTools::GenerateRandomProjector(4, 3));
            ContextObject::Ptr pFunc = std::make_shared<VectorFunction>(TestTools::GenerateRandomVectorFunction(4, 3));
            Sentence::Ptr pSentence = std::make_shared<Sentence>(TestTools::GenerateRandomSentence(5, 3));

            pProj->CalcHash(calc);
            pFunc->CalcHash(calc);
            pSentence->CalcHash(calc);

            sentences.push_front(pSentence);

            Word projHash = ctxHier[j]->AddNew(pProj);
            Word funcHash = ctxHier[j]->AddNew(pFunc);
            Word sentHash = ctxHier[j]->AddNew(pSentence);
        }
    }

    TIM tim;
    tim.functionOp = OpType::XOR;
    tim.projectorOp = OpType::AND;
    tim.root = RootType::STATIC;
    tim.version = VersionType::INTRA;

    ContextCreativity creativity(&calc, tim);

    {
        std::vector<Context::Ptr> newCtx;

        size_t res = creativity.Imagine(global, true, newCtx);

        EXPECT_EQ(0, res);
        EXPECT_EQ(0, newCtx.size());

        EXPECT_EQ(2, global->GetFunctions().size());
        EXPECT_EQ(3, global->GetProjectors().size());
        EXPECT_EQ(2, global->GetSentences().size());
    }


    {
        std::vector<Context::Ptr> newCtx;

        size_t res = creativity.Imagine(ctx1, false, newCtx);

        EXPECT_EQ(1, res);
        EXPECT_EQ(41, newCtx.size());

        EXPECT_EQ(0, ctx1->GetFunctions().size());
        EXPECT_EQ(0, ctx1->GetProjectors().size());
        EXPECT_EQ(1, ctx1->GetSentences().size());
    }


    // this part is extremely slow, it takes up to 2 minutes
    //{
    //    std::vector<Context::Ptr> newCtx;

    //    size_t res = creativity.Imagine(ctx2, false, newCtx);

    //    EXPECT_EQ(1, res);
    //    EXPECT_EQ(4619, newCtx.size());

    //    EXPECT_EQ(2, ctx2->GetFunctions().size());
    //    EXPECT_EQ(2, ctx2->GetProjectors().size());
    //    EXPECT_EQ(3, ctx2->GetSentences().size());
    //}
}


