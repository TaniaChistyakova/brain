/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "Context.h"
#include "HashCalculator.h"

#include "TestTools.h"

#include "gtest/gtest.h"

TEST(ContextTest, SmokeTest)
{
    ContextObject::Ptr pProj = std::make_shared<Projector>(TestTools::GenerateRandomProjector(4, 3));
    ContextObject::Ptr pFunc = std::make_shared<VectorFunction>(TestTools::GenerateRandomVectorFunction(4, 3));
    Sentence::Ptr pSentence = std::make_shared<Sentence>(TestTools::GenerateRandomSentence(5, 3));

    HashCalculator calc;
    Context ctx;

    calc.InitRandom();

    pProj->CalcHash(calc);
    pFunc->CalcHash(calc);
    pSentence->CalcHash(calc);

    Word projHash = ctx.AddNew(pProj);
    Word funcHash = ctx.AddNew(pFunc);
    Word sentHash = ctx.AddNew(pSentence);

    EXPECT_TRUE(projHash != funcHash);
    EXPECT_TRUE(projHash != sentHash);
    EXPECT_TRUE(funcHash != sentHash);

    auto pProjRes = ctx.Get(projHash);
    auto pFuncRes = ctx.Get(funcHash);
    auto pSentRes = ctx.Get(sentHash);

    EXPECT_TRUE(pProj == pProjRes);
    EXPECT_TRUE(pFunc == pFuncRes);
    EXPECT_TRUE(pSentence == pSentRes);

    auto sentences = ctx.GetSentences();

    EXPECT_EQ(1ull, sentences.size());

    EXPECT_TRUE(pSentence == sentences[0]);
}

TEST(ContextTest, CloneTest)
{
    ContextObject::Ptr pProj = std::make_shared<Projector>(TestTools::GenerateRandomProjector(4, 3));
    ContextObject::Ptr pFunc = std::make_shared<VectorFunction>(TestTools::GenerateRandomVectorFunction(4, 3));
    Sentence::Ptr pSentence = std::make_shared<Sentence>(TestTools::GenerateRandomSentence(5, 3));

    HashCalculator calc;
    Context::Ptr global = std::make_shared<Context>();

    Context::Ptr ctx = std::make_shared<Context>(global);

    calc.InitRandom();

    pProj->CalcHash(calc);
    pFunc->CalcHash(calc);
    pSentence->CalcHash(calc);

    Word projHash = ctx->AddNew(pProj);
    Word funcHash = ctx->AddNew(pFunc);
    Word sentHash = ctx->AddNew(pSentence);

    auto clone = ctx->Clone();

    auto pProjRes = clone->Get(projHash);
    auto pFuncRes = clone->Get(funcHash);
    auto pSentRes = clone->Get(sentHash);

    EXPECT_TRUE(pProj == pProjRes);
    EXPECT_TRUE(pFunc == pFuncRes);
    EXPECT_TRUE(pSentence == pSentRes);
    EXPECT_TRUE(ctx->GetParent() == clone->GetParent());

    auto sentences = ctx->GetSentences();

    EXPECT_EQ(1ull, sentences.size());

    EXPECT_TRUE(pSentence == sentences[0]);

}

TEST(ContextTest, IterTest)
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
        for (int i = 0; i < 4; ++i)
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

    SentenceIter iter(ctx2);
    auto iter1 = ++iter;
    auto iter2 = iter++;

    bool res = (iter1 == iter2);

    EXPECT_TRUE(res);
    int count = 0;

    for (SentenceIter iter3(ctx2); iter3; ++iter3)
    {
        Sentence::CPtr pSentence = *iter3;
        EXPECT_TRUE(pSentence != nullptr);
        EXPECT_TRUE(pSentence == sentences[count]);
        ++count;
    }

    EXPECT_EQ(8, count);
}

TEST(ContextTest, CloneAndShuffleTest)
{
    size_t n = 2, m = 3;
    HashCalculator calc;
    calc.InitRandom();

    Context::Ptr global = std::make_shared<Context>();
    Context::Ptr ctx1 = std::make_shared<Context>(global); // empty ctx in the middle
    Context::Ptr ctx2 = std::make_shared<Context>(ctx1);

    std::vector<Sentence::CPtr> sentences;
    for (size_t i = 0; i < n+m; i++)
    {
        auto pSentence = std::make_shared<Sentence>(TestTools::GenerateRandomSentence(5, 3));
        pSentence->CalcHash(calc);

        sentences.push_back(pSentence);
        ctx2->AddNew(pSentence);
    }

    std::vector<Context::Ptr> clones = ctx2->CloneAndShuffle(m);

    // pow(m + 1, n) - count of all shuffles
    // tgamma(n + 1) = n! - count of all permutations
    // 1 - this context
    EXPECT_LE(clones.size(), pow(m + 1, n) * tgamma(n + 1) - 1);

    for (size_t i = 0; i < clones.size(); i++)
    {
        auto clone = clones[i];

        size_t c = std::count_if(clones.cbegin(), clones.cend(), [clone](auto it) { return it->GetSentences() == clone->GetSentences(); });
        EXPECT_EQ(1, c);
    }
}

