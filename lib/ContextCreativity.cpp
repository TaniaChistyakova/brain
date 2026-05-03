/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "ContextCreativity.h"

ContextCreativity::ContextCreativity(HashCalculator* inHashCalculator, const TIM& inTim)
	: hashCalculator(inHashCalculator)
	, tim(inTim)
{

}

namespace
{
	template <class T>
	void ApplyBasicOp(std::shared_ptr<T>& a, std::shared_ptr<const T> b, OpType op)
	{
		if (!a)
		{
			a = std::const_pointer_cast<T>(b);
		}
		else if (op == OpType::AND)
		{
			a = std::make_shared<T>(std::move((*a) & (*b)));
		}
		else if (op == OpType::XOR)
		{
			a = std::make_shared<T>((*a) ^ (*b));
		}
		else
		{
			assert(0);
		}
	}


	bool IncIndex(std::vector<size_t>& idx, size_t max)
	{
		for (size_t i = 0; i < idx.size(); i++)
		{
			if (++idx[i] < max)
			{
				return true;
			}
			idx[i] = 0;
		}

		return false;
	}
}

template<class Iter>
auto ContextCreativity::GetExitingObjects(Context::CPtr ctx)
{
	std::vector<typename Iter::retval> fullSet;
	for (Iter it(ctx); it; ++it)
	{
		fullSet.push_back(*it);
	}
	return fullSet;
}

template<class Iter>
auto ContextCreativity::CreateNewObjects(Context::CPtr ctx, OpType op)
{
	std::vector<typename Iter::retval> out;
	std::vector<typename Iter::retval> fullSet;
	DotArray mask;

	fullSet = GetExitingObjects<Iter>(ctx);

	mask.resize(fullSet.size());

	while (mask.Inc()) // we don't need an empty mask here
	{
		if (mask.onesNum() == 1)
		{
			continue;
		}

		std::shared_ptr<typename Iter::obj_type> a;

		for (size_t i = 0; i < fullSet.size(); ++i)
		{
			if (mask[i])
			{
				typename Iter::retval b = fullSet[i];

				ApplyBasicOp(a, b, op);
			}
		}

		a->CalcHash(*hashCalculator);
		out.push_back(a);
	} 

	return out;
}


std::vector<Sentence::CPtr> ContextCreativity::CreateNewSentences(std::vector<VectorFunction::CPtr> const& functions, std::vector<Projector::CPtr> const& projectors)
{
	std::vector<Sentence::CPtr> out;

	for (auto f : functions)
	{
		for (size_t i = 0; i < projectors.size(); i++)
		{
			for (size_t j = 1; j <= f->Arity(); j++)
			{
				std::vector<size_t> idx;
				idx.resize(j, 0);

				do
				{
					Sentence::Ptr pS = std::make_shared<Sentence>();
					pS->verb = f;
					pS->noun = projectors[i];

					for (auto idxVal : idx)
					{
						pS->adjectives.push_back(projectors[idxVal < i ? idxVal : idxVal + 1]);
					}

					pS->CalcHash(*hashCalculator);
					out.push_back(pS);

				} while (IncIndex(idx, projectors.size() - 1));
			}
		}
	}

	return out;
}

void ContextCreativity::AddToContext(Context::Ptr ctx, std::vector<ContextObject::CPtr> const& input)
{
	for (size_t i = 0; i < input.size(); i++)
	{
		ctx->AddNew(input[i]);
	}
}

std::vector<Context::Ptr> ContextCreativity::MultiplyContext(Context::Ptr ctx, std::vector<ContextObject::CPtr> const& input)
{
	std::vector<Context::Ptr> out;
	if (input.size() > 0)
	{
		for (size_t i = 1; i < input.size(); i++)
		{
			Context::Ptr newCtx = ctx->Clone();
			newCtx->AddNew(input[i]);
			out.push_back(newCtx);
		}
		ctx->AddNew(input[0]);
	}
	return out;
}


size_t ContextCreativity::Imagine(Context::Ptr ctx, bool isRoot, std::vector<Context::Ptr>& newCtx)
{
	if (isRoot)
	{
		std::vector<ContextObject::CPtr> newObj;
		if (tim.root == RootType::STATIC)
		{
			newObj = ContextObject::Downcast(CreateNewObjects<ProjectorIter>(ctx, tim.projectorOp));
		}
		else
		{
			newObj = ContextObject::Downcast(CreateNewObjects<FunctionIter>(ctx, tim.functionOp));
		}

		if (tim.version == VersionType::EXTRA)
		{
			newCtx = MultiplyContext(ctx, newObj);
		}
		else
		{
			AddToContext(ctx, newObj);
		}

		return 0;
	}
	else
	{
		std::vector<VectorFunction::CPtr> functions;
		std::vector<Projector::CPtr> projectors;

		if (tim.root == RootType::STATIC)
		{
			functions = CreateNewObjects<FunctionIter>(ctx, tim.functionOp);
			projectors = GetExitingObjects<ProjectorIter>(ctx);
		}
		else
		{
			functions = GetExitingObjects<FunctionIter>(ctx);
			projectors = CreateNewObjects<ProjectorIter>(ctx, tim.projectorOp);
		}

		auto sentences = CreateNewSentences(functions, projectors);

		if (tim.version == VersionType::EXTRA)
		{
			AddToContext(ctx, ContextObject::Downcast(sentences));
			return sentences.size();
		}
		else
		{
			newCtx = MultiplyContext(ctx, ContextObject::Downcast(sentences));
			return 1;
		}
	}
}

