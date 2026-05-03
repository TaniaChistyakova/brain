/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "Sentence.h"
#include "FunctionGenerator.h"
#include "HashCalculator.h"

Projector::CPtr Sentence::ConvertToProjector() const
{
	DotArray arg(noun->Size());
	Projector::Ptr projector = std::make_shared<Projector>();

	do
	{
		const Mask* noun = Search(arg);
		if (noun == nullptr)
		{
			continue;
		}

		DotArray res = Act(arg);

		Mask::Ptr mask = std::make_shared<Mask>(res, res.Size());
		projector->Insert(mask);

	} while (arg.Inc());

	projector->Pack();

	return projector;
}


VectorFunction::CPtr Sentence::ConvertToFunction() const
{
	DotArray arg(noun->Size());

	VectorFunctionGenerator gen(verb->Dimensions(), noun->Size());

	gen.Begin();
	do
	{
		const Mask* noun = Search(arg);
		if (noun == nullptr)
		{
			continue;
		}

		DotArray res = Act(arg);

		gen.InsertTruthTableValue(res, arg);
	} while (arg.Inc());
	VectorFunction::Ptr func = gen.End();

	return func;
}


void Sentence::AddToHash(class HashCalculator& calc) const
{
	assert(noun != nullptr);
	assert(verb != nullptr);

	noun->AddToHash(calc);
	verb->AddToHash(calc);

	for (size_t i = 0; i < adjectives.size(); i++)
	{
		auto object = adjectives[i];
		assert(object != nullptr);

		object->AddToHash(calc);
	}
}

ContextObject::Type Sentence::GetType() const
{
	return ContextObject::Type::Sentence;
}

