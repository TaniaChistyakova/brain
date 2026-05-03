/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#pragma once

#include "Mask.h"
#include "Word.h"
#include "ContextObject.h"


/*
Projector is a mathematical operator which acts on a binary sequence and defines some set of its subsequences. 
Masks are projector's components, they sum (mod 2, XOR) the results. Shorter masks have advance in search.
If a subsequence falls into two masks altogether, it is thrown away. 

Projectors size N form a group over XOR operation and a monoid over AND. So set of the projectors is a commutative ring.
*/

class Projector : public ContextObject
{

public:

	Projector() = default;

	Projector(Dot d, size_t sz)
	{
		if (d)
		{
			Mask::Ptr m = std::make_shared<Mask>(sz);
			maskSet.push_back(m);
		}
	}

	typedef std::shared_ptr<Projector> Ptr;
	typedef std::shared_ptr<const Projector> CPtr;

    bool Insert_ForTest(Mask const& inMask);

    bool Insert(Mask::CPtr inMask);
	void Pack();

	template<class DotsView>
    const Mask* Search(const DotsView& dots) const;

    Projector& operator ^= (Projector const& proj);
    Projector& operator &= (Projector const& proj);

    bool Empty() const
    {
        return maskSet.empty();
    }

	size_t Size() const
	{
		return bitSize;
	}

	size_t LetterSize() const
	{
		return (bitSize >> BitTraits<Letter>::x3) + (bitSize & BitTraits<Letter>::b111 ? 1 : 0);
	}

	size_t GetMaskCount_ForTest() const
	{
		return maskSet.size();
	}

	Mask::CPtr GetMask_ForTest(size_t i) const 
	{
		return maskSet[i];
	}

	void AddToHash(class HashCalculator& calc) const override;
	virtual Type GetType() const override;

private:

    // This set should be sorted by the length
    std::vector<Mask::CPtr> maskSet;
	size_t bitSize = 0;
};

template<class DotsView>
const Mask* Projector::Search(const DotsView& dots) const
{
	if (maskSet.empty() || bitSize > dots.size())
	{
		return nullptr;
	}

	Mask::CPtr foundMask = nullptr;

	for (size_t i = 0; i < maskSet.size(); i++)
	{
		// we search for a mask in the backward order, starting from longest and
		// go to the shortest. That's needed to find the smallest one as the final
		// result. 

		Mask::CPtr currentMask = maskSet[maskSet.size() - i - 1];
		assert(currentMask);

		if (currentMask->isFoundIn(dots))
		{
			// xor-ing between all the found masks
			if (foundMask)
			{
				foundMask = nullptr;
			}
			else
			{
				foundMask = currentMask;
			}
		}
	}

	return foundMask.get();
}



inline Projector operator ^ (Projector const& projA, Projector const& projB)
{
    Projector projXor(projA);
    projXor ^= projB;
    return projXor;
}

inline Projector operator & (Projector const& projA, Projector const& projB)
{
    Projector projAnd(projA);
	projAnd &= projB;
    return projAnd;
}


