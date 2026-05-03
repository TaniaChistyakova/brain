/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#pragma once
#include "VectorFunction.h"
#include "Projector.h"

/*
This class is key element of the theory. We can construct sentences using the exiting elements and
turn the result into a very new element. We can all use all possible operation while we listen but
in memory we can use only specified ones. It reduced our ability to think and not limits our ability
to speak and listen. Reduction in thinking is important because it allows us to avoid problem with
the Gödel theorems.
*/
class Sentence: public ContextObject
{
public:
	template<class DotsView>
	Mask const* Search(const DotsView& inDots) const;

	template<class DotsView>
	DotArray Act(const DotsView& inDots) const;

	void AddToHash(class HashCalculator& calc) const override;
	virtual Type GetType() const override;


	typedef std::shared_ptr<Sentence> Ptr;
	typedef std::shared_ptr<const Sentence> CPtr;

	/*
	This function converts the sentence into a projector. This projectors returns non-zero mask if the argument is 
	in set of the sentence's output.
	*/
	Projector::CPtr ConvertToProjector() const;

	/*
	This function return a vector function. This functions acts the very same way as the whole sentence.
	*/
	VectorFunction::CPtr ConvertToFunction() const;

	Projector::CPtr noun;

	VectorFunction::CPtr verb;

	std::vector < Projector::CPtr > adjectives;
};

template<class DotsView>
Mask const* Sentence::Search(const DotsView& inDots) const
{ 
	assert(noun != nullptr);

	return noun->Search(inDots);
}

template<class DotsView>
DotArray Sentence::Act(const DotsView& inOutDots) const
{
	assert(noun != nullptr);
	assert(verb != nullptr);

	DotArray args(verb->Arity());

	const size_t sz = std::min(verb->Arity(), adjectives.size());

	for (size_t i = 0; i < sz; i++)
	{
		auto object = adjectives[i];
		assert(object != nullptr);

		if (object->Search(inOutDots) != nullptr)
		{
			args[i] = 1;
		}
	}

	return (*verb)(args);
}


