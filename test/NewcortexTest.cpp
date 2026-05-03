/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "Neocortex.h"
#include "HashCalculator.h"
#include "FunctionGenerator.h"

#include "TestTools.h"

#include "gtest/gtest.h"


TEST(NewcortexTest, SmokeTest)
{
    HashCalculator calc;
    calc.InitRandom();

    Context::Ptr global = std::make_shared<Context>();

    auto PutIntoGlobal = [global, &calc](std::shared_ptr<const ContextObject> pObj)
        {
            std::const_pointer_cast<ContextObject>(pObj)->CalcHash(calc);
            return LetterTools::PutWord(global->AddNew(pObj).ToBits());
        };

    std::vector<Dot> d1 = { 1,0,0,1,0,0,1 };
    std::vector<Dot> d2 = { 0,1,1,0,1,1,0 };

    auto pProj1 = TestTools::MakeProjectorFromBits(d1, { 1,1,1,1,1,1,1 });
    auto pProj2 = TestTools::MakeProjectorFromBits(d2, { 1,1,1,1,1,1,1 });
    std::vector<Letter> data =
    {
        PutIntoGlobal(pProj1),
        PutIntoGlobal(pProj2),
    };

    Letter code1 = (Letter)TestTools::DotsToBits(d1);
    Letter code2 = (Letter)TestTools::DotsToBits(d2);

    Neocortex core(2, 4, 3);
    core.SetGlobalContext(global);
    core.SetHashCalculator(&calc);

    EXPECT_EQ(2ull, core.Size());

    std::deque<Letter> input;
    input.push_back(code1);


    core.SetInput_ForTest(input);
    core.Listen();

    {
        auto out = core[1].GetInput();
        EXPECT_EQ(1ull, out.size());
        EXPECT_EQ(data[0], out[0]);
    }

    input.clear();
    input.push_back(code1);

    core.SetInput_ForTest(input);
    core.Listen();

    {
        auto out = core[1].GetInput();
        EXPECT_EQ(1ull, out.size());
        EXPECT_EQ(data[0], out[0]);
    }

    input.clear();
    input.push_back(code2);

    core.SetInput_ForTest(input);
    core.Listen();

    {
        auto out = core[1].GetInput();
        EXPECT_EQ(2ull, out.size());
        EXPECT_EQ(data[0], out[0]);
        EXPECT_EQ(data[1], out[1]);
    }

    input.clear();
    input.push_back(code1);
    input.push_back(code2);

    core.SetInput_ForTest(input);
    core.Listen();

    {
        auto out = core[1].GetInput();
        EXPECT_EQ(4ull, out.size());
        EXPECT_EQ(data[0], out[0]);
        EXPECT_EQ(data[1], out[1]);
        EXPECT_EQ(data[0], out[2]);
        EXPECT_EQ(data[1], out[3]);
    }

    input.clear();
    input.push_back(code2);
    input.push_back(code1);

    core.SetInput_ForTest(input);
    core.Listen();

    {
        auto out = core[1].GetInput();
        EXPECT_EQ(4ull, out.size());
        EXPECT_EQ(data[1], out[0]);
        EXPECT_EQ(data[0], out[1]);
        EXPECT_EQ(data[1], out[2]);
        EXPECT_EQ(data[0], out[3]);
    }

}

TEST(NewcortexTest, PreSayTest)
{
    Sentence::CPtr sent1, sent2;

    {
        Sentence sentense;
        Mask::CPtr nounMask;

        {
            Mask mask1(3);
            mask1.Load_ForTest({ 1,1,1 }, { 1,1,1 });
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
            VectorFunctionGenerator gen(3, 2);
            gen.Begin();

            gen.InsertTruthTableValue(DotArray({ 0,1,0 }), DotArray({ 1,0 }));

            VectorFunction::Ptr func = gen.End();
            sentense.verb = func;

            sent1 = std::make_shared<Sentence>(sentense);
        }

    }

    {
        Sentence sentense;
        Mask::CPtr nounMask;

        {
            Mask mask1(3);
            mask1.Load_ForTest({ 0,0,1 }, { 1,1,1 });
            nounMask = std::make_shared<Mask>(mask1);

            Projector searcher1;
            searcher1.Insert(nounMask);
            sentense.noun = std::make_shared<Projector>(searcher1);
        }

        {
            Mask mask1(3);
            mask1.Load_ForTest({ 0,0,1 }, { 1,1,1 });
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
            VectorFunctionGenerator gen(3, 2);
            gen.Begin();
            gen.InsertTruthTableValue(DotArray({ 1,1,1 }), DotArray({ 1,0 }));

            VectorFunction::Ptr func = gen.End();
            sentense.verb = func;

            sent2 = std::make_shared<Sentence>(sentense);
        }
    }


    HashCalculator calc;
    calc.InitRandom();

    Neocortex core(2, 10, 3);

    Context::Ptr global = std::make_shared<Context>();

    auto PutIntoGlobal = [global, &calc](std::shared_ptr<const ContextObject> pObj)
        {
            std::const_pointer_cast<ContextObject>(pObj)->CalcHash(calc);
            return LetterTools::PutWord(global->AddNew(pObj).ToBits());
        };

    PutIntoGlobal(sent1);
    PutIntoGlobal(sent2);

    Layer left(&core);
    Layer right(&core);

    left.AddLetter(0b100);
    left.AddLetter(0b111);

    left.PreSay(global, &right);

    EXPECT_EQ(2ull, right.GetInput().size());

    EXPECT_EQ((Letter)0b111, right.GetInput()[0]); // from sent2
    EXPECT_EQ((Letter)0b010, right.GetInput()[1]); // from sent1
}
