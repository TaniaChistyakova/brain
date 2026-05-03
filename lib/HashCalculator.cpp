/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "HashCalculator.h"
#include "FunctionGenerator.h"
#include "Mask.h"
#include "VectorFunction.h"


void HashCalculator::InitRandom()
{
	VectorFunctionGenerator generator(wordSize, wordSize);

	std::mt19937 random;
	std::random_device randDev;
	random.seed(randDev());

	blockCalculator = generator.GenRandom(random);

	starter = DotArray::GenerateRandom(wordSize, random);
	enderTempl = DotArray::GenerateRandom(wordSize, random);
}


Word HashCalculator::Hash_ForTest(const Mask& mask)
{
	Begin();
	AddArray(mask.cares);
	AddArray(mask.dots);
	return End();
}

void HashCalculator::Begin()
{
	assert(blockCalculator);

	currentHash = (*blockCalculator)(starter);
	currentData = enderTempl;
	currentIndex = 0;
}

void HashCalculator::AddArray(const DotArray& data)
{
	assert(blockCalculator);

	for (size_t i = 0; i < data.size(); i++)
	{
		if(currentIndex < currentData.size())
		{
			currentData[currentIndex++] = data[i];
		}
		else
		{
			currentHash ^= (*blockCalculator)(currentData);
			currentData = enderTempl;
			currentIndex = 0;
		}
	}
}

Word HashCalculator::End()
{
	assert(blockCalculator);

	if (currentIndex > 0)
	{
		currentHash ^= (*blockCalculator)(currentData);
	}

	return currentHash.ToBits<Word>();
}

