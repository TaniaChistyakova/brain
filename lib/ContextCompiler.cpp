/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "ContextCompiler.h"
#include "Functor.h"
#include "HashCalculator.h"

ContextCompiler::ContextCompiler(HashCalculator* inHashCalculator)
{
	hashCalculator = inHashCalculator;
}


template<class T>
void arrangeArgs(T& args)
{
	// let's assume the arguments are in the forward order
	std::reverse(std::begin(args), std::end(args));
}

Letter ContextCompiler::CompileFunctor(Letter ltr, std::function<ContextObject::CPtr()> PopObjFromStack)
{
	uint64_t arity = LetterTools::GetFunctorArity(ltr);
	if (arity == 0)
	{
		// the functor arity is invalid
		return 0;
	}

	uint64_t functorCode = LetterTools::GetFunctorCode(ltr, arity);
	Functor functor(arity, functorCode);
	if (functor.isConstant())
	{
		// the functor is a pure constant
		return 0;
	}

	// make sure that the functor has all its arguments
	std::vector<ContextObject::CPtr> rawArgs;
	for (size_t i = 0; i < arity; ++i)
	{
		ContextObject::CPtr obj = PopObjFromStack();
		if (obj == nullptr)
		{
			// the argument obj is not found
			return 0;
		}

		rawArgs.push_back(obj);
	}

	//make sure that the all the arguments are one type
	ContextObject::Type functorType = ContextObject::Type::Unknown;
	for (size_t i = 0; i < arity; ++i)
	{
		ContextObject::Type argType = rawArgs[i]->GetType();
		if (argType == ContextObject::Type::Sentence)
		{
			// we skip sentences here, they can be converted into projectors or functions later
			continue;
		}

		if (functorType == ContextObject::Type::Unknown)
		{
			functorType = argType;
		}
		else
		{
			if (functorType != argType)
			{
				// all the functor arguments must be one type, functions or projectors
				return 0;
			}
		}
	}

	if (functorType == ContextObject::Type::Unknown)
	{
		// we can't find the functor's type
		return 0;
	}

	// apply the functor
	ContextObject::Ptr pRes;
	if (functorType == ContextObject::Type::Function)
	{
		std::vector<VectorFunction::CPtr> args;
		for (size_t i = 0; i < arity; ++i)
		{
			args.push_back(ConvertToFunction(rawArgs[i]));
		}

		arrangeArgs(args);

		VectorFunction::Ptr pVecRes = functor(args);
		pRes = pVecRes;
	}
	else //if(functorType == ContextObject::Type::Projector)
	{
		std::vector<Projector::CPtr> args;
		for (size_t i = 0; i < arity; ++i)
		{
			args.push_back(ConvertToProjector(rawArgs[i]));
		}

		arrangeArgs(args);

		Projector::Ptr pProjRes = functor(args);
		pRes = pProjRes;
	}

	return PushNewObject(pRes);
}

Letter ContextCompiler::CompileSentence(Letter ltr, std::function<ContextObject::CPtr()> PopObjFromStack)
{
	uint64_t sentArity = LetterTools::GetSentenceObjLength(ltr);
	if (sentArity == 0)
	{
		return 0;
	}

	Sentence::Ptr sentence = std::make_shared<Sentence>();

	// load adjectives
	for (size_t i = 0; i < sentArity; ++i)
	{
		ContextObject::CPtr obj = PopObjFromStack();
		if (obj == nullptr)
		{
			// the argument obj is not found
			return 0;
		}

		if (obj->GetType() == ContextObject::Type::Function)
		{
			// we don't expect to have a function as a sentence object
			return 0;
		}

		sentence->adjectives.push_back(ConvertToProjector(obj));
	}

	arrangeArgs(sentence->adjectives);

	// load verb
	{
		ContextObject::CPtr obj = PopObjFromStack();
		if (obj == nullptr)
		{
			// the verb is not found
			return 0;
		}

		if (obj->GetType() == ContextObject::Type::Projector)
		{
			// we don't expect to have a project as a verb
			return 0;
		}

		sentence->verb = ConvertToFunction(obj);
	}

	// load noun
	{
		ContextObject::CPtr obj = PopObjFromStack();
		if (obj == nullptr)
		{
			// the noun is not found
			return 0;
		}

		if (obj->GetType() == ContextObject::Type::Function)
		{
			// we don't expect to have a function as a sentence object
			return 0;
		}

		sentence->noun = ConvertToProjector(obj);
	}

	return PushNewObject(sentence);
}


VectorFunction::CPtr ContextCompiler::ConvertToFunction(ContextObject::CPtr pObj)
{
	assert(pObj);
	ContextObject::Type argType = pObj->GetType();
	VectorFunction::CPtr pFunc;

	if (argType == ContextObject::Type::Sentence)
	{
		Sentence::CPtr pSent = std::static_pointer_cast<const Sentence>(pObj);
		pFunc = pSent->ConvertToFunction();
	}
	else
	{
		assert(argType == ContextObject::Type::Function);
		pFunc = std::static_pointer_cast<const VectorFunction>(pObj);
	}

	assert(pFunc);
	return pFunc;
}

Projector::CPtr ContextCompiler::ConvertToProjector(ContextObject::CPtr pObj)
{
	assert(pObj);
	ContextObject::Type argType = pObj->GetType();
	Projector::CPtr pProj;

	if (argType == ContextObject::Type::Sentence)
	{
		Sentence::CPtr pSent = std::static_pointer_cast<const Sentence>(pObj);
		pProj = pSent->ConvertToProjector();
	}
	else
	{
		assert(argType == ContextObject::Type::Projector);
		pProj = std::static_pointer_cast<const Projector>(pObj);
	}

	assert(pProj);
	return pProj;
}

Letter ContextCompiler::PushNewObject(ContextObject::Ptr pObj)
{
	assert(pObj);
	assert(hashCalculator);
	//we have got a brand new context object, so we need to add it into our context
	pObj->CalcHash(*hashCalculator);
	Word newCode = pContext->AddNew(pObj);

	return LetterTools::PutWord(newCode.ToBits());
}

void ContextCompiler::Clear()
{
	pContext = nullptr;
}
