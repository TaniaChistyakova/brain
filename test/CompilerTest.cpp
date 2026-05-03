/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "ContextCompiler.h"
#include "HashCalculator.h"

#include "TestTools.h"

#include "gtest/gtest.h"


class CompilerTest : public testing::Test
{
public:

    HashCalculator calc;
    Context::Ptr global;
    Context::Ptr pCtx;
    std::unique_ptr<ContextCompiler> compiler;

    CompilerTest()
    {
        calc.InitRandom();
        compiler = std::make_unique<ContextCompiler>(&calc);
        global = std::make_shared<Context>();
        pCtx = std::make_shared<Context>(global);
    }

    Letter PutIntoGlobal(ContextObject::CPtr pObj)
    {
        std::const_pointer_cast<ContextObject>(pObj)->CalcHash(calc);
        return LetterTools::PutWord(global->AddNew(pObj).ToBits());
    }
};

TEST_F(CompilerTest, SimpleSentence)
{
    Projector::CPtr pProj1 = std::make_shared<Projector>(TestTools::GenerateRandomProjector(4, 3));
    VectorFunction::CPtr pFunc = std::make_shared<VectorFunction>(TestTools::GenerateRandomVectorFunction(4, 3));
    Projector::CPtr pProj2 = std::make_shared<Projector>(TestTools::GenerateRandomProjector(4, 3));

    std::vector<Letter> data = {
        PutIntoGlobal(pProj1),
        PutIntoGlobal(pFunc),
        PutIntoGlobal(pProj2),
        LetterTools::PutSentenceObjLength(1),
    };

    bool res = compiler->Compile(LetterView(data, 0), pCtx);

    EXPECT_TRUE(res);

    auto map = pCtx->GetMap_ForTest();

    EXPECT_EQ(1ull, map.size());
    auto pObj = map.cbegin()->second;

    EXPECT_EQ(ContextObject::Type::Sentence, pObj->GetType());

    Sentence::CPtr pSent = std::static_pointer_cast<const Sentence>(pObj);

    EXPECT_TRUE(pSent->noun == pProj1);
    EXPECT_TRUE(pSent->verb == pFunc);
    EXPECT_EQ(1ull, pSent->adjectives.size());
    EXPECT_TRUE(pSent->adjectives[0] == pProj2);

    for (size_t i = 0; i < 3; i++)
    {
        EXPECT_EQ((Letter)0, data[i]);
    }

    EXPECT_TRUE(pSent->noun->GetHash().ToBits() != 0);
    EXPECT_TRUE(pSent->verb->GetHash().ToBits() != 0);
    for (auto adj : pSent->adjectives)
    {
        EXPECT_TRUE(adj->GetHash().ToBits() != 0);
    }

    Letter sentLtr = data[3];
    Letter sentLtrRes = LetterTools::PutWord(map.cbegin()->first.ToBits());

    EXPECT_EQ(sentLtrRes, sentLtr);
}


TEST_F(CompilerTest, SimpleProjectFunctor)
{
    Projector::CPtr pProj1 = std::make_shared<Projector>(TestTools::GenerateRandomProjector(3, 4));
    Projector::CPtr pProj2 = std::make_shared<Projector>(TestTools::GenerateRandomProjector(3, 4));

    Projector xorProj = *pProj1 ^ *pProj2;

    std::vector<Letter> data = {
        PutIntoGlobal(pProj1),
        PutIntoGlobal(pProj2),
        LetterTools::PutFunctor(2,0b110), //xor
    };

    bool res = compiler->Compile(LetterView(data, 0), pCtx);

    EXPECT_TRUE(res);

    auto map = pCtx->GetMap_ForTest();

    EXPECT_EQ(1ull, map.size());
    auto pObj = map.cbegin()->second;

    EXPECT_EQ(ContextObject::Type::Projector, pObj->GetType());

    Projector::CPtr pProjRes = std::static_pointer_cast<const Projector>(pObj);

    // projector's hash can change
    auto args_permutations = TestTools::GenerateAllPermulations<DotArray>(3);
    for (auto&& args : args_permutations)
    {
        bool resA = pProjRes->Search(args) != nullptr;
        bool resB = xorProj.Search(args) != nullptr;

        EXPECT_EQ(resB, resA);
    }

    for (size_t i = 0; i < 2; i++)
    {
        EXPECT_EQ((Letter)0, data[i]);
    }

    Letter sentLtr = data[2];
    Letter sentLtrRes = LetterTools::PutWord(map.cbegin()->first.ToBits());

    EXPECT_EQ(sentLtrRes, sentLtr);

}

TEST_F(CompilerTest, SimpleFunctionFunctor)
{
    VectorFunction::CPtr pObj1 = std::make_shared<VectorFunction>(TestTools::GenerateRandomVectorFunction(3, 4));
    VectorFunction::CPtr pObj2 = std::make_shared<VectorFunction>(TestTools::GenerateRandomVectorFunction(3, 4));

    VectorFunction xorObj = *pObj1 ^ *pObj2;
    Word hashProjXor = xorObj.CalcHash(calc);

    std::vector<Letter> data = {
        PutIntoGlobal(pObj1),
        PutIntoGlobal(pObj2),
        LetterTools::PutFunctor(2,0b110), //xor
    };

    bool res = compiler->Compile(LetterView(data, 0), pCtx);

    EXPECT_TRUE(res);

    auto map = pCtx->GetMap_ForTest();

    EXPECT_EQ(1ull, map.size());
    auto pObjRes = map.cbegin()->second;
    auto hashObjRes = map.cbegin()->first;

    EXPECT_EQ(ContextObject::Type::Function, pObjRes->GetType());

    EXPECT_EQ(hashProjXor, hashObjRes);
}

TEST_F(CompilerTest, SentenceWithObjectFunctor)
{
    Projector::CPtr pProj1 = std::make_shared<Projector>(TestTools::GenerateRandomProjector(4, 3));
    VectorFunction::CPtr pFunc = std::make_shared<VectorFunction>(TestTools::GenerateRandomVectorFunction(4, 3));
    Projector::CPtr pProj2 = std::make_shared<Projector>(TestTools::GenerateRandomProjector(4, 3));

    VectorFunction::CPtr pObj1 = std::make_shared<VectorFunction>(TestTools::GenerateRandomVectorFunction(3, 4));

    std::vector<Letter> data = {
        PutIntoGlobal(pProj1),

        PutIntoGlobal(pFunc),
        PutIntoGlobal(pObj1),
        LetterTools::PutFunctor(2,0b110), //xor

        PutIntoGlobal(pProj2),
        LetterTools::PutSentenceObjLength(1),
    };

    bool res = compiler->Compile(LetterView(data, 0), pCtx);

    EXPECT_TRUE(res);

    auto map = pCtx->GetMap_ForTest();

    EXPECT_EQ(2ull, map.size());

    VectorFunction::CPtr pNewVerb;
    for (auto it = map.cbegin(); it != map.cend(); ++it)
    {
        auto pObj = it->second;
        if (pObj->GetType() == ContextObject::Type::Function)
        {
            pNewVerb = std::static_pointer_cast<const VectorFunction>(pObj);
            break;
        }
    }


    for (auto it = map.cbegin(); it != map.cend(); ++it)
    {
        auto pObj = it->second;
        if (pObj->GetType() == ContextObject::Type::Sentence)
        {
            Sentence::CPtr pSent = std::static_pointer_cast<const Sentence>(pObj);

            EXPECT_TRUE(pSent->noun == pProj1);
            EXPECT_EQ(1ull, pSent->adjectives.size());
            EXPECT_TRUE(pSent->adjectives[0] == pProj2);
            EXPECT_TRUE(pSent->verb == pNewVerb);


            EXPECT_TRUE(pSent->noun->GetHash().ToBits() != 0);
            EXPECT_TRUE(pSent->verb->GetHash().ToBits() != 0);
            for (auto adj : pSent->adjectives)
            {
                EXPECT_TRUE(adj->GetHash().ToBits() != 0);
            }
        }
    }

}


TEST_F(CompilerTest, SentenceWithSubjectFunctor)
{
    Projector::CPtr pProj1 = std::make_shared<Projector>(TestTools::GenerateRandomProjector(4, 3));
    VectorFunction::CPtr pFunc = std::make_shared<VectorFunction>(TestTools::GenerateRandomVectorFunction(4, 3));
    Projector::CPtr pProj2 = std::make_shared<Projector>(TestTools::GenerateRandomProjector(4, 3));

    Projector::CPtr pProj3 = std::make_shared<Projector>(TestTools::GenerateRandomProjector(3, 4));

    std::vector<Letter> data = {
        PutIntoGlobal(pProj1),
        PutIntoGlobal(pProj3),
        LetterTools::PutFunctor(2,0b110), //xor

        PutIntoGlobal(pFunc),

        PutIntoGlobal(pProj2),
        LetterTools::PutSentenceObjLength(1),
    };

    bool res = compiler->Compile(LetterView(data, 0), pCtx);

    EXPECT_TRUE(res);

    auto map = pCtx->GetMap_ForTest();

    EXPECT_EQ(2ull, map.size());

    Projector::CPtr pNewSubj;
    for (auto it = map.cbegin(); it != map.cend(); ++it)
    {
        auto pObj = it->second;
        if (pObj->GetType() == ContextObject::Type::Projector)
        {
            pNewSubj = std::static_pointer_cast<const Projector>(pObj);
            break;
        }
    }


    for (auto it = map.cbegin(); it != map.cend(); ++it)
    {
        auto pObj = it->second;

        EXPECT_TRUE(pObj->GetHash().ToBits() != 0);

        if (pObj->GetType() == ContextObject::Type::Sentence)
        {
            Sentence::CPtr pSent = std::static_pointer_cast<const Sentence>(pObj);

            EXPECT_TRUE(pSent->noun == pNewSubj);
            EXPECT_EQ(1ull, pSent->adjectives.size());
            EXPECT_TRUE(pSent->adjectives[0] == pProj2);
            EXPECT_TRUE(pSent->verb == pFunc);


            EXPECT_TRUE(pSent->noun->GetHash().ToBits() != 0);
            EXPECT_TRUE(pSent->verb->GetHash().ToBits() != 0);
            for (auto adj : pSent->adjectives)
            {
                EXPECT_TRUE(adj->GetHash().ToBits() != 0);
            }
        }
    }

}

TEST_F(CompilerTest, ProjectFunctorWithSentence)
{
    Projector::CPtr pProj1 = std::make_shared<Projector>(TestTools::GenerateRandomProjector(4, 3));
    VectorFunction::CPtr pFunc = std::make_shared<VectorFunction>(TestTools::GenerateRandomVectorFunction(4, 3));
    Projector::CPtr pProj2 = std::make_shared<Projector>(TestTools::GenerateRandomProjector(4, 3));
    Projector::CPtr pProj3 = std::make_shared<Projector>(TestTools::GenerateRandomProjector(3, 4));

    std::vector<Letter> data = {
        PutIntoGlobal(pProj1),
        PutIntoGlobal(pFunc),
        PutIntoGlobal(pProj2),
        LetterTools::PutSentenceObjLength(1),
        PutIntoGlobal(pProj3),
        LetterTools::PutFunctor(2,0b110), //xor

    };

    bool res = compiler->Compile(LetterView(data, 0), pCtx);

    EXPECT_TRUE(res);

    auto map = pCtx->GetMap_ForTest();

    EXPECT_EQ(2ull, map.size());

    for (auto it = map.cbegin(); it != map.cend(); ++it)
    {
        auto pObj = it->second;
        EXPECT_TRUE(pObj->GetType() != ContextObject::Type::Function);

        EXPECT_TRUE(pObj->GetHash().ToBits() != 0);

        if (pObj->GetType() == ContextObject::Type::Sentence)
        {
            Sentence::CPtr pSent = std::static_pointer_cast<const Sentence>(pObj);

            EXPECT_TRUE(pSent->noun == pProj1);
            EXPECT_EQ(1ull, pSent->adjectives.size());
            EXPECT_TRUE(pSent->adjectives[0] == pProj2);
            EXPECT_TRUE(pSent->verb == pFunc);

            EXPECT_TRUE(pSent->noun->GetHash().ToBits() != 0);
            EXPECT_TRUE(pSent->verb->GetHash().ToBits() != 0);
            for (auto adj : pSent->adjectives)
            {
                EXPECT_TRUE(adj->GetHash().ToBits() != 0);
            }
        }
    }

}

TEST_F(CompilerTest, FunctorFunctorWithSentence)
{
    Projector::CPtr pProj1 = std::make_shared<Projector>(TestTools::GenerateRandomProjector(4, 3));
    VectorFunction::CPtr pFunc = std::make_shared<VectorFunction>(TestTools::GenerateRandomVectorFunction(4, 3));
    Projector::CPtr pProj2 = std::make_shared<Projector>(TestTools::GenerateRandomProjector(4, 3));
    VectorFunction::CPtr pFunc2 = std::make_shared<VectorFunction>(TestTools::GenerateRandomVectorFunction(4, 3));

    std::vector<Letter> data = {
        PutIntoGlobal(pProj1),
        PutIntoGlobal(pFunc),
        PutIntoGlobal(pProj2),
        LetterTools::PutSentenceObjLength(1),
        PutIntoGlobal(pFunc2),
        LetterTools::PutFunctor(2,0b110), //xor

    };

    bool res = compiler->Compile(LetterView(data, 0), pCtx);

    EXPECT_TRUE(res);

    auto map = pCtx->GetMap_ForTest();

    EXPECT_EQ(2ull, map.size());

    for (auto it = map.cbegin(); it != map.cend(); ++it)
    {
        auto pObj = it->second;
        EXPECT_TRUE(pObj->GetType() != ContextObject::Type::Projector);

        EXPECT_TRUE(pObj->GetHash().ToBits() != 0);

        if (pObj->GetType() == ContextObject::Type::Sentence)
        {
            Sentence::CPtr pSent = std::static_pointer_cast<const Sentence>(pObj);

            EXPECT_TRUE(pSent->noun == pProj1);
            EXPECT_EQ(1ull, pSent->adjectives.size());
            EXPECT_TRUE(pSent->adjectives[0] == pProj2);
            EXPECT_TRUE(pSent->verb == pFunc);

            EXPECT_TRUE(pSent->noun->GetHash().ToBits() != 0);
            EXPECT_TRUE(pSent->verb->GetHash().ToBits() != 0);
            for (auto adj : pSent->adjectives)
            {
                EXPECT_TRUE(adj->GetHash().ToBits() != 0);
            }
        }
    }

}


TEST_F(CompilerTest, ManySubjectsSentence)
{
    Projector::CPtr pProj1 = std::make_shared<Projector>(TestTools::GenerateRandomProjector(4, 3));
    VectorFunction::CPtr pFunc = std::make_shared<VectorFunction>(TestTools::GenerateRandomVectorFunction(4, 3));
    Projector::CPtr pProj2 = std::make_shared<Projector>(TestTools::GenerateRandomProjector(4, 3));
    Projector::CPtr pProj3 = std::make_shared<Projector>(TestTools::GenerateRandomProjector(4, 3));
    Projector::CPtr pProj4 = std::make_shared<Projector>(TestTools::GenerateRandomProjector(4, 3));
    Projector::CPtr pProj5 = std::make_shared<Projector>(TestTools::GenerateRandomProjector(4, 3));
    Projector::CPtr pProj6 = std::make_shared<Projector>(TestTools::GenerateRandomProjector(4, 3));

    std::vector<Letter> data = {
        PutIntoGlobal(pProj1),
        PutIntoGlobal(pFunc),
        PutIntoGlobal(pProj2),
        PutIntoGlobal(pProj3),
        PutIntoGlobal(pProj4),
        PutIntoGlobal(pProj5),
        PutIntoGlobal(pProj6),
        LetterTools::PutSentenceObjLength(5),
    };

    bool res = compiler->Compile(LetterView(data, 0), pCtx);

    EXPECT_TRUE(res);

    auto map = pCtx->GetMap_ForTest();

    EXPECT_EQ(1ull, map.size());
    auto pObj = map.cbegin()->second;

    EXPECT_EQ(ContextObject::Type::Sentence, pObj->GetType());

    Sentence::CPtr pSent = std::static_pointer_cast<const Sentence>(pObj);

    EXPECT_TRUE(pSent->noun == pProj1);
    EXPECT_TRUE(pSent->verb == pFunc);
    EXPECT_EQ(5ull, pSent->adjectives.size());
    EXPECT_TRUE(pSent->adjectives[0] == pProj2);
    EXPECT_TRUE(pSent->adjectives[1] == pProj3);
    EXPECT_TRUE(pSent->adjectives[2] == pProj4);
    EXPECT_TRUE(pSent->adjectives[3] == pProj5);
    EXPECT_TRUE(pSent->adjectives[4] == pProj6);

    EXPECT_TRUE(pSent->noun->GetHash().ToBits() != 0);
    EXPECT_TRUE(pSent->verb->GetHash().ToBits() != 0);
    for (auto adj : pSent->adjectives)
    {
        EXPECT_TRUE(adj->GetHash().ToBits() != 0);
    }

    for (size_t i = 0; i < 2 + 5; i++)
    {
        EXPECT_EQ((Letter)0, data[i]);
    }

    Letter sentLtr = data[2 + 5];
    Letter sentLtrRes = LetterTools::PutWord(map.cbegin()->first.ToBits());

    EXPECT_EQ(sentLtrRes, sentLtr);
}

TEST_F(CompilerTest, IterativeCompilation)
{
    Projector::CPtr pProj1 = std::make_shared<Projector>(TestTools::GenerateRandomProjector(4, 3));
    VectorFunction::CPtr pFunc = std::make_shared<VectorFunction>(TestTools::GenerateRandomVectorFunction(4, 3));
    Projector::CPtr pProj2 = std::make_shared<Projector>(TestTools::GenerateRandomProjector(4, 3));

    Projector::CPtr pProj3 = std::make_shared<Projector>(TestTools::GenerateRandomProjector(3, 4));

    std::vector<Letter> data1 = {
        PutIntoGlobal(pProj1),
        PutIntoGlobal(pProj3),
        LetterTools::PutFunctor(2,0b110), //xor

        PutIntoGlobal(pFunc),

        PutIntoGlobal(pProj2),
        LetterTools::PutSentenceObjLength(1),
    };

    auto data2 = data1;

    bool res = compiler->Compile(LetterView(data1, 0), pCtx);

    EXPECT_TRUE(res);

    Context::Ptr pCtx2 = std::make_shared<Context>(global);

    for (size_t i = 0; i < data2.size(); i++)
    {
        bool res2 = compiler->Compile(LetterView(data2, 0, i + 1), pCtx2);
        EXPECT_TRUE(res2);
    }

    EXPECT_TRUE(data1 == data2);
}

