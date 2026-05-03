/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#pragma once

#include "ContextObject.h"
#include "Sentence.h"

#include <map>
#include <deque>

class Context : public std::enable_shared_from_this<Context>
{
public:

	typedef std::shared_ptr<Context> Ptr;
	typedef std::shared_ptr<const Context> CPtr;

	// initialises the current context, if no parent the current context is global
	Context(Context::CPtr inParent = nullptr);

	// adds a new object into the current context
	Word AddNew(ContextObject::CPtr pObj);

	// there is no remove operation for a context object, all the objects should be destroyed 
	// in the context

	// searches for a object through all the context's hierarchy
	ContextObject::CPtr Get(Word code) const;

	const std::deque<Sentence::CPtr>& GetSentences() const;
	const std::deque<Projector::CPtr>& GetProjectors() const;
	const std::deque<VectorFunction::CPtr>& GetFunctions() const;
	const std::map<Word, ContextObject::CPtr>& GetMap_ForTest() const;

	Context::CPtr GetParent() const;

	Context::Ptr CreateEmptyChild() const;
	Context::Ptr Clone() const;
	void SetSentences(const std::deque<Sentence::CPtr>& inSent);

	std::vector<Context::Ptr> CloneAndShuffle(size_t oldSentenceCount) const;

private:
	std::map<Word, ContextObject::CPtr> contextMap;
	std::deque<Sentence::CPtr> sentences;
	std::deque<VectorFunction::CPtr> functions;
	std::deque<Projector::CPtr> projectors;

	Context::CPtr pParent;
};

template<typename Obj, const std::deque<std::shared_ptr<const Obj> >& (Context::*GetObjects) () const>
class ContextIterator
{
public:
	using obj_type = Obj;
	using retval = std::shared_ptr<const Obj>;
	using deque = std::deque<retval>;
	using deiter = deque::const_iterator;

	ContextIterator()
	{ }

	ContextIterator(Context::CPtr inCtx)
	{
		pCtx = inCtx;
		if (pCtx)
		{
			iter = std::begin((pCtx.get()->*GetObjects)());
			end = std::end((pCtx.get()->*GetObjects)());
		}

		while (pCtx && iter == end)
		{
			pCtx = pCtx->GetParent();
			iter = std::begin((pCtx.get()->*GetObjects)());
			end = std::end((pCtx.get()->*GetObjects)());
		}
	}

	ContextIterator(Context::CPtr inCtx, deiter inIt)
	{
		pCtx = inCtx;
		iter = inIt;
		end = std::end((pCtx.get()->*GetObjects)());

		while (pCtx && iter == end)
		{
			pCtx = pCtx->GetParent();
			iter = std::begin((pCtx.get()->*GetObjects)());
			end = std::end((pCtx.get()->*GetObjects)());
		}
	}

	ContextIterator& operator++() 
	{ 
		if (pCtx)
		{
			++iter;

			while (pCtx && iter == end)
			{
				*this = ContextIterator(pCtx->GetParent());
			}
		}

		return *this; 
	} 

	ContextIterator operator++(int) 
	{ 
		ContextIterator tmp(*this);
		operator++(); 
		return tmp; 
	}

	bool operator==(const ContextIterator& rhs) const
	{ 
		if (pCtx == nullptr && rhs.pCtx == nullptr)
		{
			return true;
		}
		else if (pCtx == nullptr || rhs.pCtx == nullptr)
		{
			return false;
		}

		return pCtx == rhs.pCtx && iter == rhs.iter;
	}

	bool operator!=(const ContextIterator& rhs) const
	{ 
		return !(*this == rhs); 
	}

	operator bool() const
	{
		return pCtx != nullptr;
	}

	retval operator*() 
	{ 
		return *iter; 
	}

private:

	Context::CPtr pCtx;
	deiter iter;
	deiter end;
};

typedef ContextIterator<Sentence, &Context::GetSentences> SentenceIter;
typedef ContextIterator<Projector, &Context::GetProjectors> ProjectorIter;
typedef ContextIterator<VectorFunction, &Context::GetFunctions> FunctionIter;

inline const std::deque<Sentence::CPtr>& Context::GetSentences() const
{
	return sentences;
}

inline const std::map<Word, ContextObject::CPtr>& Context::GetMap_ForTest() const
{
	return contextMap;
}

inline const std::deque<Projector::CPtr>& Context::GetProjectors() const
{
	return projectors;
}

inline const std::deque<VectorFunction::CPtr>& Context::GetFunctions() const
{
	return functions;
}

inline Context::CPtr Context::GetParent() const
{
	return pParent;
}

inline void Context::SetSentences(const std::deque<Sentence::CPtr>& inSent)
{
	sentences = inSent;
}

