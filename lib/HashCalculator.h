/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#pragma once

#include "Word.h"
#include "DotArray.h"

#include <memory>

class HashCalculator
{
public:

	HashCalculator()
	{
	}

	void InitRandom();
	Word Hash_ForTest(const class Mask& mask);

	void Begin();
	void AddArray(const DotArray& data);
	Word End();

private:

	static constexpr uint64_t wordSize = Word::bitSize;
	std::shared_ptr<const class VectorFunction> blockCalculator;
	DotArray starter;
	DotArray enderTempl;

	DotArray currentHash;
	DotArray currentData;
	size_t currentIndex = 0;
};


