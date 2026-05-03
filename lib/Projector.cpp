/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "Projector.h"
#include "HashCalculator.h"

#include <algorithm>

// maskSet must be sorted by size to have stable search and
// sorted by values if the sizes are same to have stable hash code 
// calculation

auto maskComparator = [](Mask::CPtr inMaskA, Mask::CPtr inMaskB) -> bool
{
	if (inMaskA->Size() > inMaskB->Size())
	{
		return false;
	}
	else if (inMaskA->Size() == inMaskB->Size())
	{
		for (size_t i = 0; i < inMaskA->Size(); i++)
		{
			if (inMaskA->cares[i] == inMaskB->cares[i])
			{
				if (inMaskA->dots[i] == inMaskB->dots[i])
				{
					// just keep searching...
				}
				else if(inMaskB->dots[i])
				{
					return false;
				}
				else // if(inMaskA->dots[i])
				{
					return true;
				}
			}
			else if (inMaskB->cares[i])
			{
				return false;
			}
			else // if(inMaskA->cares[i])
			{
				return true;
			}
		}

		return false; //inMaskA == inMaskB
	}
	else // if (inMaskA->Size() < inMaskB->Size())
	{
		return true;
	}
};


bool Projector::Insert(Mask::CPtr inMask)
{
	assert(inMask);

	size_t maskSize = inMask->Size();
	auto iter = std::find_if(maskSet.begin(), maskSet.end(), [=](auto mask) 
		{ 
			return maskComparator(inMask, mask);
		});

	maskSet.insert(iter, inMask);
	if (maskSize > bitSize)
	{
		bitSize = maskSize;
	}
	return true;
}

void Projector::Pack()
{
	bool isRestartPacking = false;
	size_t minSize = 0;

	do
	{
		isRestartPacking = false;
		for (size_t i = 0; i < maskSet.size() && !isRestartPacking; i++)
		{
			if (maskSet[i] == nullptr)
			{
				continue;
			}
			const Mask& maskA = *maskSet[i];
			assert(maskA.Size() > 0);

			if (maskA.Size() < minSize)
			{
				continue;
			}

			minSize = maskA.Size();

			for (size_t j = i + 1; j < maskSet.size() && !isRestartPacking; j++)
			{
				if (maskSet[j] == nullptr)
				{
					continue;
				}
				// we can combine two mask with one different 1-cares bit into a new one with bit set 0-cares
				const Mask& maskB = *maskSet[j];
				assert(maskB.Size() > 0);

				assert(maskA.Size() <= maskB.Size());

				if (maskA.Size() == maskB.Size())
				{
					auto diffCares = maskA.cares ^ maskB.cares;
					auto diffCaresCount = diffCares.onesNum();
					if(diffCaresCount == 0)
					{
						auto diffDots = maskA.dots ^ maskB.dots;
						auto diffCount = diffDots.onesNum();

						if (diffCount == 1)
						{
							// if we find two masks with same cares and one bit difference in dots,
							// we can replace both with a single mask which has the bit set to 0-cares

							auto newMask = std::make_shared<Mask>(maskA.Size());
							newMask->dots = maskA.dots;
							newMask->cares = maskA.cares ^ diffDots;
							newMask->Normalise();

							maskSet[i] = newMask;
							maskSet[j] = nullptr;
							isRestartPacking = true;
							break;
						}
						else if (diffCount == 0)
						{
							// two same masks can be dropped

							maskSet[i] = nullptr;
							maskSet[j] = nullptr;
							isRestartPacking = true;
							break;
						}
						else
						{
							continue;
						}					
					}
					else if (diffCaresCount == 1)
					{
						auto caresCountA = maskA.cares.onesNum();
						auto caresCountB = maskB.cares.onesNum();

						if (caresCountA > caresCountB)
						{
							if ((maskA.dots & maskB.cares) != maskB.dots)
							{
								continue;
							}
						}
						else
						{
							if ((maskB.dots & maskA.cares) != maskA.dots)
							{
								continue;
							}
						}

						// two masks with different bit in cares can be replaced with the one
						// which has a flipped bit in dots and 1-cares

						auto newMask = std::make_shared<Mask>(maskA.Size());
						if (caresCountA > caresCountB)
						{
							newMask->cares = maskA.cares;
							newMask->dots = maskA.dots;
						}
						else
						{
							newMask->cares = maskB.cares;
							newMask->dots = maskB.dots;
						}

						newMask->dots ^= diffCares;
						newMask->Normalise();

						maskSet[i] = newMask;
						maskSet[j] = nullptr;
						isRestartPacking = true;
						break;
					}
					else
					{
						continue;
					}
				}
				else //if maskA.Size() < maskB.Size()
				{
					// we combining only same size masks

					break;
				}
			}
		}
	} while (isRestartPacking);

	maskSet.erase(std::remove(maskSet.begin(), maskSet.end(), nullptr), maskSet.end());

	std::sort(maskSet.begin(), maskSet.end(), maskComparator);

	if (maskSet.empty())
	{
		bitSize = 0;
	}
	else
	{
		bitSize = maskSet.back()->Size();
	}
}

bool Projector::Insert_ForTest(Mask const& inMask)
{
	return Insert(std::make_shared<Mask const>(inMask));
}

Projector& Projector::operator ^= (Projector const& proj)
{
	// xor operator is trivial because searching applies 
	// xor among the found masks

	for (size_t i = 0; i < proj.maskSet.size(); ++i)
	{
		Insert(proj.maskSet[i]);
	}

	return *this;
}

Projector& Projector::operator &= (Projector const& proj)
{
	// and operator is looking for a sequences which can
	// satisfy masks from the both projectors

	std::vector<Mask::CPtr > newMaskSet;

	for (size_t i = 0; i < proj.maskSet.size(); ++i)
	{
		for (size_t j = 0; j < maskSet.size(); ++j)
		{
			if (proj.maskSet[i] == maskSet[j])
			{
				newMaskSet.push_back(maskSet[j]);
				continue;
			}

			Mask temp;
			if (!Mask::AndOnSets(*maskSet[j], *proj.maskSet[i], temp))
			{
				continue;
			}

			newMaskSet.push_back(std::make_shared<const Mask>(std::move(temp)));
		}
	}

	maskSet.clear();

	for (size_t i = 0; i < newMaskSet.size(); ++i)
	{
		Insert(newMaskSet[i]);
	}

	return *this;
}

void Projector::AddToHash(class HashCalculator& calc) const
{
	for (size_t i = 0; i < maskSet.size(); ++i)
	{
		calc.AddArray(maskSet[i]->cares);
		calc.AddArray(maskSet[i]->dots);
	}
}

ContextObject::Type Projector::GetType() const
{
	return ContextObject::Type::Projector;
}
