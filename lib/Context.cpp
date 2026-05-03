/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "Context.h"
#include "ShuffleIterator.h"


Context::Context(Context::CPtr inParent)
{
	pParent = inParent;
}

Word Context::AddNew(ContextObject::CPtr pObj)
{
	assert(pObj);

	Word code = pObj->GetHash();

	assert(!code.empty());

	contextMap[code] = pObj;
	if (pObj->GetType() == ContextObject::Type::Sentence)
	{
		Sentence::CPtr sentence = std::static_pointer_cast<const Sentence>(pObj);
		sentences.push_front(sentence);
	}
	else if (pObj->GetType() == ContextObject::Type::Projector)
	{
		Projector::CPtr proj = std::static_pointer_cast<const Projector>(pObj);
		projectors.push_front(proj);
	}
	else if (pObj->GetType() == ContextObject::Type::Function)
	{
		VectorFunction::CPtr func = std::static_pointer_cast<const VectorFunction>(pObj);
		functions.push_front(func);
	}
	return code;
}

ContextObject::CPtr Context::Get(Word code) const
{
	Context const* pCurrentCtx = this;
	while (pCurrentCtx)
	{
		auto it = pCurrentCtx->contextMap.find(code);
		if (it != pCurrentCtx->contextMap.end())
		{
			return it->second;
		}

		pCurrentCtx = pCurrentCtx->pParent.get();
	}
	return nullptr;
}

Context::Ptr Context::Clone() const
{
	return std::make_shared<Context>(*this);
}


Context::Ptr Context::CreateEmptyChild() const
{
	Context::Ptr child = std::make_shared<Context>(shared_from_this());
	return child;
}

std::vector<Context::Ptr> Context::CloneAndShuffle(size_t oldSentenceCount) const
{
	size_t newSentenceCount = GetSentences().size() - oldSentenceCount;
	assert(GetSentences().size() >= newSentenceCount);

	if (newSentenceCount == 0)
	{
		return {};
	}

	std::vector<Context::Ptr> res;

	std::deque<Sentence::CPtr> clonedSentences = GetSentences();
	std::sort(clonedSentences.begin(), clonedSentences.begin() + newSentenceCount);

	do
	{
		ShuffleIterator iter(newSentenceCount, clonedSentences.size() - newSentenceCount);

		do
		{
			auto indices = iter.Output();

			std::deque<Sentence::CPtr> shuffledSentences;

			ShuffleIterator::Shuffle(indices, clonedSentences, shuffledSentences);

			if (shuffledSentences != GetSentences() && 
				std::count_if(res.cbegin(), res.cend(), [&shuffledSentences](auto it) {return it->GetSentences() == shuffledSentences;}) == 0)
			{
				auto clone = Clone();
				clone->SetSentences(shuffledSentences);
				res.push_back(clone);
			}
		} while (iter.Advance());
	} while (std::next_permutation(clonedSentences.begin(), clonedSentences.begin() + newSentenceCount));

	return res;
}


