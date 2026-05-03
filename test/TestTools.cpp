/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "TestTools.h"
#include "BMath.h"
#include "Neocortex.h"

#include <cassert>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <random>
#include <sstream>

namespace TestTools
{
    int GenerateRandomNumber(int from, int to)
    {
        static std::random_device rand_dev;
        static std::mt19937 generator(rand_dev());
        std::uniform_int_distribution<int>  distr(from, to);
        return distr(generator);
    }

    std::vector<Dot> GenerateRandomDotVector(size_t size)
    {
        std::vector<Dot> res(size);
        const int range_from = 0;
        const int range_to = 1;

        for (size_t i = 0; i < size; ++i)
        {
            auto val = GenerateRandomNumber(range_from, range_to);
            assert(val < 2);
            res[i] = val;
        }

        return res;
    }

    Mask GenerateRandomMask(size_t size)
    {
        Mask result(size);
        result.Load_ForTest(GenerateRandomDotVector(size), GenerateRandomDotVector(size));
        return result;
    }

    Projector GenerateRandomProjector(size_t maxBitSize, size_t maskCount, bool pack)
    {
        Projector proj;

        for (size_t i = 0; i < maskCount; i++)
        {
            size_t maskSize = GenerateRandomNumber((int)maxBitSize / 2, (int)maxBitSize);
            Mask mask = GenerateRandomMask(maskSize);
            proj.Insert_ForTest(mask);
        }
        if (pack)
        {
            proj.Pack();
        }

        return proj;
    }


    ScalarFunction GenerateRandomScalarFunction(size_t arity)
    {
        ScalarFunction res(arity);
        res.Load_ForTest(GenerateRandomDotVector(Pow2(arity)));

        return res;
    }

    VectorFunction GenerateRandomVectorFunction(size_t dimensions, size_t arity)
    {
        VectorFunction vecF(dimensions, arity);
        std::vector<ScalarFunction> scalars;
        scalars.reserve(dimensions);
        for (size_t i = 0; i < dimensions; i++)
        {
            scalars.emplace_back(TestTools::GenerateRandomScalarFunction(arity));
        }

        vecF.Load_ForTest(scalars);
        return vecF;
    }

    Sentence GenerateRandomSentence(size_t length, size_t arity)
    {
        Sentence sentence;
        sentence.noun = std::make_shared<Projector>(GenerateRandomProjector(length, 2 * length));
        sentence.verb = std::make_shared<VectorFunction>(GenerateRandomVectorFunction(length, arity));
        for (size_t i = 0; i < arity; i++)
        {
            sentence.adjectives.push_back(std::make_shared<Projector>(GenerateRandomProjector(length, length)));
        }
        return sentence;
    }

    DotArray VectorToDotArray(std::vector<Dot> const& vec)
    {
        DotArray res(vec.size());

        for (size_t i = 0; i < vec.size(); i++)
        {
            res[i] = vec[i];
        }

        return res;
    }

    bool StringToBits(std::string const& str, uint64_t& bitfield)
    {
        bitfield = 0;

        if (str.length() > std::numeric_limits<uint64_t>::digits)
        {
            return false;
        }

        for (size_t i = 0; i < str.length(); i++)
        {
            auto val = str[i];
            if (val && val != '0')
            {
                bitfield |= 1LL << i;
            }
        }

        return true;
    }

    uint64_t DotsToBits(std::vector<Dot> const& dots)
    {
        assert(dots.size() < std::numeric_limits<uint64_t>::digits);
        uint64_t bitfield = 0;
        for (uint64_t i = 0; i < dots.size(); i++)
        {
            if (dots[i])
            {
                bitfield |= 1LL << i;
            }
        }
        return bitfield;
    }

    std::vector<Dot> BitsToDots(uint64_t bitfield, uint64_t size)
    {
        std::vector<Dot> dots(size);
        uint64_t tmp = bitfield;
        for (size_t i = 0; i < std::numeric_limits<uint64_t>::digits; i++)
        {
            assert(i < dots.size());
            if (tmp & 0x1LL)
            {
                dots[i] = 1;
            }
            tmp = tmp >> 1;
            if (!tmp)
            {
                break;
            }
        }
        return dots;
    }

    Projector::Ptr MakeProjectorFromBits(std::vector<Dot> const& dots, std::vector<Dot> const& cares)
    {
        Mask::Ptr pMask = std::make_shared<Mask>();
        pMask->Load_ForTest(dots, cares);
        Projector::Ptr pProj = std::make_shared<Projector>();
        pProj->Insert(pMask);
        return pProj;
    }

}




void PrintTo(const Dot& d, std::ostream* os)
{
    *os << (d ? "Dot(1)" : "Dot(0)");
}

void PrintTo(const std::vector<Dot>& dots, std::ostream* stream)
{
    *stream << "Dots[";
    for (auto it = dots.crbegin(); it != dots.crend(); ++it)
    {
        if (*it)
        {
            *stream << "1";
        }
        else
        {
            *stream << "0";
        }
    }
    *stream << "]";
}

void PrintTo(const DotArray& dots, std::ostream* stream)
{
    *stream << "Dots[";
    for (size_t i = 0; i < dots.size(); ++i)
    {
        if (dots[i])
        {
            *stream << "1";
        }
        else
        {
            *stream << "0";
        }
    }
    *stream << "]";

}

void PrintTo(const Mask& m, std::ostream* stream)
{
    *stream << "Mask(dots[";
    for (size_t i = 0; i < m.dots.size(); ++i)
    {
        if (m.dots[i])
        {
            *stream << "1";
        }
        else
        {
            *stream << "0";
        }
    }
    *stream << "]), cares[";
    for (size_t i = 0; i < m.cares.size(); ++i)
    {
        if (m.cares[i])
        {
            *stream << "1";
        }
        else
        {
            *stream << "0";
        }
    }
    *stream << "])";
}

void PrintTo(const DotArrayView& dots, std::ostream* stream)
{
    *stream << "Dots[";
    for (size_t i = 0; i < dots.size(); ++i)
    {
        if (dots[i])
        {
            *stream << "1";
        }
        else
        {
            *stream << "0";
        }
    }
    *stream << "]";
}

void PrintTo(const enum LetterType& l, std::ostream* stream)
{
    *stream << "Letter(" << (int)l << ")";
}

void PrintTo(const enum ContextObject::Type& l, std::ostream* stream)
{
    *stream << "Letter(" << (int)l << ")";
}

