/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#pragma once

#include "Dot.h"
#include "ScalarFunction.h"
#include "DotArray.h"
#include "VectorFunction.h"
#include "Mask.h"
#include "Projector.h"
#include "DotFixedArray.h"
#include "Sentence.h"

#include <vector>


namespace TestTools
{
    int GenerateRandomNumber(int from, int to);
    std::vector<Dot> GenerateRandomDotVector(size_t size);
    Mask GenerateRandomMask(size_t size);
    Projector GenerateRandomProjector(size_t maxBitSize, size_t maskCount, bool pack = true);
    ScalarFunction GenerateRandomScalarFunction(size_t arity);
    VectorFunction GenerateRandomVectorFunction(size_t dimensions, size_t arity);
    Sentence GenerateRandomSentence(size_t length, size_t arity);

    std::vector<Dot> BitsToDots(uint64_t bitfield, uint64_t size);

    template<typename T>
    auto GenerateAllPermulations(size_t arity)
    {
        size_t argsVecSize = Pow2(arity);
        std::vector<T> res(argsVecSize);

        for (size_t i = 0; i < argsVecSize; i++)
        {
            res[i] = TestTools::BitsToDots(i, arity);
        }

        return res;
    }

    DotArray VectorToDotArray(std::vector<Dot> const& vec);

    bool StringToBits(std::string const& str, uint64_t& bitfield);

    uint64_t DotsToBits(std::vector<Dot> const& dots);

    Projector::Ptr MakeProjectorFromBits(std::vector<Dot> const& dots, std::vector<Dot> const& cares);
}


void PrintTo(const Dot& d, std::ostream* os);

void PrintTo(const std::vector<Dot>& dots, std::ostream* os);

void PrintTo(const DotArray& dots, std::ostream* os);
void PrintTo(const Mask& m, std::ostream* os);


void PrintTo(const DotArrayView& dots, std::ostream* os);

void PrintTo(const enum LetterType& l, std::ostream* os);
void PrintTo(const enum ContextObject::Type& l, std::ostream* os);


template<size_t n>
void PrintTo(const class DotFixedArray<n>& dots, std::ostream* os)
{
    PrintTo(dots.ToVector_ForTest(), os);
}
