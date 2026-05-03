/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#pragma once

#include "Letter.h"
#include "Context.h"
#include "AnswerTree.h"
#include "ContextCreativity.h"

#include <deque>

class Neocortex;
class HashCalculator;

class Layer
{
public:

	struct Index
	{
		size_t value;

		Index() : value(0) {}

		Index(size_t start, size_t pos)
			: value(start + pos)
		{ }

		size_t pos(size_t newStart) const { return valid(newStart) ? value - newStart : 0; }
		bool valid(size_t newStart) const { return value >= newStart; }
	};

	Layer(Neocortex* inNeocortex);

	// normally we expect to have the global context here,
	// all its projects are not intersect, i.e. if ProjA and ProjB are in
	// the global context, then ProjA & ProjB = null by definition

	void Listen(Context::CPtr pCtx, Layer* upper);

	// I don't know how to test the Say() function so I split it into two parts:
	// Compiler and PreSay. Both suppose to be tested separately.

	bool Say(Context::Ptr pCtx, Layer* right);
	void PreSay(Context::CPtr pParentCtx, Layer* right) const;

	void Reset();
	Index AddLetter(Letter ltr);
	void SetInput(std::deque<Letter> const& in);
	size_t GetMaxSize() const;
	bool IsEmpty() const;

	const std::deque<Letter>& GetInput() const;
private:

	Neocortex* neocortex;
	std::deque<Letter> input;
	size_t startIdx = 0;

	Index lastListenIdx;

	size_t maxSize = 1000;
};


class AnswerPayloadInit : public AnswerPayload
{
public:
	AnswerPayloadInit(Layer* inLayer);

	void Think(bool bRoot, std::vector<AnswerPayload::Ptr>& newNodes) override;
	bool Say(AnswerPayload::Ptr pLeft, std::vector<AnswerPayload::Ptr>& newNodes) override;
	AnswerPayload::Ptr Clone() const override;
	AnswerPayload::Ptr CreateChild() const override;
	bool IsEmpty() const override;

	Layer* pLayer;
};


class AnswerPayloadImpl : public AnswerPayload
{
public:
	AnswerPayloadImpl(Neocortex* inNeocortex, Context::CPtr inParentContext);
	AnswerPayloadImpl(Neocortex* inNeocortex, Context::CPtr inParentContext, Context::Ptr inChildContext, Layer const& inLayer);

	bool Say(Layer* left, std::vector<Context::Ptr>& newCtxs);

	void Think(bool bRoot, std::vector<AnswerPayload::Ptr>& newNodes) override;
	bool Say(AnswerPayload::Ptr pLeft, std::vector<AnswerPayload::Ptr>& newNodes) override;
	AnswerPayload::Ptr Clone() const override;
	AnswerPayload::Ptr CreateChild() const override;
	bool IsEmpty() const override;

	Context::CPtr pParentContext;
	Context::Ptr pChildContext;
	Neocortex* neocortex;

	Layer layer;

};

class Neocortex
{
public:
	Neocortex(size_t layerCount, size_t maxSize, size_t maxTreeHorizontalRank);

	void Listen();
	bool IsListenComplete() const;
	bool TryToSay(std::vector<std::deque<Word> >& out);
	void Think();
	void SetInput_ForTest(std::deque<Letter> const& in);
	void SetInput(std::deque<Word> const& in);
	void SetGlobalContext(Context::CPtr inGlobal);
	void SetHashCalculator(HashCalculator* inHashCalc);
	void SetTIM(const TIM& tim);

	size_t Size() const;
	size_t GetMaxLayerSize() const;
	HashCalculator* GetHashCalculator();
	const TIM& GetTIM() const;

	Layer const& operator[] (size_t i) const;

private:

	TIM tim;
	size_t maxLayerSize;
	std::vector<Layer> layers;
	AnswerTree answerTree;
	Context::CPtr pGlobal;
	HashCalculator* hashCalc;
};

inline Layer::Layer(Neocortex* inNeocortex)
	: neocortex(inNeocortex)
{
	maxSize = neocortex->GetMaxLayerSize();
}


inline void Layer::Reset()
{
	lastListenIdx = Index();
}

inline size_t Layer::GetMaxSize() const
{
	return maxSize;
}

inline bool Layer::IsEmpty() const
{
	return input.empty();
}

inline void Layer::SetInput(std::deque<Letter> const& in)
{
	Reset();
	input = in;
}

inline const std::deque<Letter>& Layer::GetInput() const
{
	return input;
}

inline void Neocortex::SetInput_ForTest(std::deque<Letter> const& in)
{
	layers[0].SetInput(in);
}

inline size_t Neocortex::Size() const
{
	return layers.size();
}

inline size_t Neocortex::GetMaxLayerSize() const
{
	return maxLayerSize;
}

inline void Neocortex::SetHashCalculator(HashCalculator* inHashCalc)
{
	hashCalc = inHashCalc;
}

inline void Neocortex::SetTIM(const TIM& inTim)
{
	tim = inTim;
}


inline HashCalculator* Neocortex::GetHashCalculator()
{
	assert(hashCalc != nullptr);
	return hashCalc;
}

inline const TIM& Neocortex::GetTIM() const
{
	return tim;
}

inline Layer const& Neocortex::operator[] (size_t i) const
{
	return layers[i];
}

inline bool Neocortex::IsListenComplete() const
{
	return !layers.back().IsEmpty();
}

