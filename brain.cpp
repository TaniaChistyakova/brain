/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */


#include "BMath.h"
#include "Dot.h"
#include "ScalarFunction.h"
#include "Mask.h"
#include "VectorFunction.h"
#include "Neocortex.h"
#include "HashCalculator.h"


#include <iostream>
#include <deque>

/*
            NOTE: THIS IS PSEUDOCODE, NOT SOMETHING REAL
*/


bool DoIHaveEnergy()
{
    return true; // not always true of course
}

std::deque<Word> GetCurrentFeelings()
{
    return std::deque<Word>();
}

std::deque<Word> GetCurrentActions()
{
    return std::deque<Word>();
}

void SetCurrentActions(const std::deque<Word>& actions)
{

}

std::deque<Word> SelectProperOutput(std::vector<std::deque<Word> > const& potentialOutputs, std::deque<Word> const& currentOutput)
{
    return potentialOutputs.front();
}

Context::CPtr GetLongTermMemory()
{
    return std::make_shared<Context>();
}

Word currentAgitationLevel = Word::FromBits(3);
Word currentFrustrationLevel = Word::FromBits(0);

int main()
{
    // waking up...
    std::cout << "Good morning!\n";

    HashCalculator calc;
    calc.InitRandom(); // it should be not entirely random, loaded from memory

    TIM tim;
    tim.functionOp = OpType::XOR;
    tim.projectorOp = OpType::AND;
    tim.root = RootType::STATIC;
    tim.version = VersionType::INTRA;

    Neocortex neocortex(6, 10000, 200); // kinda human...
    neocortex.SetTIM(tim);
    neocortex.SetHashCalculator(&calc);
    neocortex.SetGlobalContext(GetLongTermMemory());


    while (DoIHaveEnergy())
    {
        std::deque<Word> input = GetCurrentFeelings();
        input.push_front(currentFrustrationLevel);
        input.push_front(currentAgitationLevel);
        
        neocortex.SetInput(input);
        neocortex.Listen();

        if (!neocortex.IsListenComplete())
        {
            continue;
        }

        uint16_t i = 0;
        for (; i < currentAgitationLevel.ToBits()+1; i++) //+1 needed here
        {
            auto output = GetCurrentActions();
            output.push_front(currentAgitationLevel);

            std::vector<std::deque<Word> > potentialOutputs;

            if (neocortex.TryToSay(potentialOutputs))
            {
                auto newOutput = SelectProperOutput(potentialOutputs, output);
                currentAgitationLevel = newOutput.front();
                newOutput.pop_front();
                SetCurrentActions(newOutput);
                break;
            }

            neocortex.Think();
        }

        currentFrustrationLevel = Word::FromBits(i);
    }

    std::cout << "Bye bye!\n";

    // sleeping

    return 0;
}

