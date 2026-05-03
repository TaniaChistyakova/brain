/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#pragma once

#include "DotArray.h"
#include "LetterView.h"

#include <memory>

class Mask : public std::enable_shared_from_this<Mask>
{
public:

    typedef std::shared_ptr<Mask> Ptr;
    typedef std::shared_ptr<const Mask> CPtr;

    Mask(uint64_t sz = 0)
        : dots(sz)
        , cares(sz)
        , actionCount(sz)
    {

    }


    Mask(DotArray const& da, uint64_t sz)
        : dots(da, sz)
        , cares(Dot(1), sz)
        , actionCount(0)
    {

    }

    void Load_ForTest(std::vector<Dot> const& inDots, std::vector<Dot> const& inCares)
    {
        assert(inDots.size() == inCares.size());

        dots = DotArray(inDots);
        cares = DotArray(inCares);
        Normalise();
    }

    // a longer mask consumed by a shorter one if all common cares bits are same
    bool isConsume(Mask const& longerMask) const
    {
        assert(Size() <= longerMask.Size());

        auto bothCares = cares & longerMask.cares;
        if (bothCares == cares)
        {
            auto myCareDots = bothCares & dots;
            auto otherCareDot = bothCares & longerMask.dots;
            if (myCareDots == otherCareDot)
            {
                return true;
            }
        }
        return false;
    }

    bool isFoundIn(const ConstDotArrayView& dotsView) const
    {
        assert(Size() <= dotsView.size());

        for (size_t i = 0; i < Size(); i++)
        {
            if (cares[i])
            {
                if (dots[i] != dotsView[i])
                {
                    return false;
                }
            }
        }

        return true;
    }

    bool isFoundIn(const DotArrayView& dotsView) const
    {
        return isFoundIn(ConstDotArrayView(dotsView));
    }

    bool isFoundIn(const DotArray& dotsView) const
    {
        assert(Size() <= dotsView.size());
        auto filteredView = cares & dotsView;
        return dots == filteredView;
    }

    template<class C>
    bool isFoundIn(const LetterView<C>& dotsView) const
    {
        assert(Size() <= dotsView.size());

        for (size_t i = 0; i < cares.GetSizeInLetters(); i++)
        {
            Letter caresLtr = cares.GetLetter(i);
            Letter dotViewLtr = dotsView[i];
            Letter dotsLtr = dots.GetLetter(i);

            Letter filteredLtr = caresLtr & dotViewLtr;
            if ((caresLtr & dotViewLtr) != dotsLtr)
            {
                return false;
            }
        }

        return true;
    }

    void Normalise()
    {
        dots &= cares; //normalisation, clearing dots with cares == 0
        actionCount = cares.zeroesNum();
    }

    uint64_t Size() const
    {
        assert(dots.size() == cares.size());

        return dots.size();
    }

    friend bool operator == (Mask const& m1, Mask const& m2)
    {
        return m1.Size() == m2.Size() &&
            m1.actionCount == m2.actionCount &&
            m1.cares == m2.cares &&
            m1.dots == m2.dots;
    }

    // this is operations on two subsets, not dot arrays
    // the resulting subset should contains elements that fall into the both masks
    // operation returns false if the result is an empty set
    static bool AndOnSets(Mask const& a, Mask const& b, Mask& mRes)
    {
        auto aImportantDots = b.cares & a.dots;
        auto bImportantDots = a.cares & b.dots;

        if (aImportantDots != bImportantDots)
        {
            return false;
        }

        mRes.cares = DotArray::LongOr(a.cares, b.cares);
        mRes.dots = DotArray::LongOr(a.dots, b.dots);
        mRes.Normalise();

        return true;
    }

    DotArray dots;
    DotArray cares;
    uint64_t actionCount;
};
