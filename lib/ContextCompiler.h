/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#pragma once

#include "Context.h"

#include <functional>


class ContextCompiler
{
public:
	// We are trying to compile the data into a context. We use Reverse Polish notation for the syntax. 
	// https://en.wikipedia.org/wiki/Reverse_Polish_notation
	// And we interpret the incoming data as stack-machine compatible
	// https://en.wikipedia.org/wiki/Stack-oriented_programming
	// In a case of any error, the function returns null

	template<class LetterView>
	bool Compile(LetterView data, Context::Ptr ctx);

	ContextCompiler(class HashCalculator* inHashCalculator);

private:
	HashCalculator* hashCalculator;

	Letter CompileFunctor(Letter ltr, std::function<ContextObject::CPtr()> PopObjFromStack);
	Letter CompileSentence(Letter ltr, std::function<ContextObject::CPtr()> PopObjFromStack);

	static VectorFunction::CPtr ConvertToFunction(ContextObject::CPtr pObj);
	static Projector::CPtr ConvertToProjector(ContextObject::CPtr pObj);
	Letter PushNewObject(ContextObject::Ptr pObj);
	void Clear();

	Context::Ptr pContext;
};

template<class LetterView>
bool ContextCompiler::Compile(LetterView data, Context::Ptr ctx)
{
	assert(ctx);

	pContext = ctx;
	size_t i = 0;

	auto PopObjFromStack = [&]() -> ContextObject::CPtr
	{
		if (i == 0)
		{
			return nullptr;
		}

		size_t j = i - 1;
		while (LetterTools::Type(data[j]) == LetterType::Ignored)
		{
			if (j == 0)
			{
				return nullptr;
			}
			--j;
		}
		Letter ltr = data[j];

		assert(LetterTools::Type(ltr) == LetterType::Word);
		Word w = (Letter)LetterTools::GetWord(ltr);

		data[j] = (Letter)0;
		ContextObject::CPtr obj = pContext->Get(w); //obj can be null
		return obj;
	};

	for (; i < data.sizeInLetters(); i++)
	{
		Letter ltr = data[i];
		LetterType type = LetterTools::Type(ltr);
		switch (type)
		{
		case LetterType::Sentence:
		{
			Letter newLtr = CompileSentence(ltr, PopObjFromStack);
			if (!newLtr)
			{
				Clear();
				return false;
			}
			data[i] = newLtr;
		}
		break;
		case LetterType::Functor:
		{
			Letter newLtr = CompileFunctor(ltr, PopObjFromStack);
			if (!newLtr)
			{
				Clear();
				return false;
			}
			data[i] = newLtr;
		}
		break;
		case LetterType::Word:
		case LetterType::Ignored:
		default:
			break;
		}
	}

	Clear();
	return true;
}


