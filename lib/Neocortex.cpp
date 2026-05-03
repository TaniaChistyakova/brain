/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "Neocortex.h"
#include "ContextCompiler.h"

Layer::Index Layer::AddLetter(Letter ltr)
{
	if (input.empty() || ltr != input.back())
	{
		input.push_back(ltr);
	}
	Index idx = Index(startIdx, input.size() - 1);

	if (input.size() > maxSize)
	{
		size_t trimCount = input.size() - maxSize;
		for (size_t i = 0; i < trimCount; i++)
		{
			input.pop_front();
			++startIdx;
		}
	}

	return idx;
}


void Layer::Listen(Context::CPtr ctx, Layer* upper)
{
	assert(ctx);
	assert(upper);

	for (size_t i = lastListenIdx.pos(startIdx); i < input.size(); i++)
	{
		// if the projectors aren't intersect each other, we can ignore the order of search

		for(ProjectorIter it(ctx); it; ++it)
		{
			auto proj = *it;
			size_t szLtr = proj->LetterSize();
			if (i + szLtr > input.size())
			{
				continue;
			}

			if (proj->Search(LetterView(input, i)))
			{
				Word code = proj->GetHash();
				Letter ltr = LetterTools::PutWord(code.ToBits());

				upper->AddLetter(ltr);

				lastListenIdx = Index(i + szLtr, startIdx);
				i += szLtr - 1; //1 will be added back in the for loop
				break;
			}
		}
	}
}

void Layer::PreSay(Context::CPtr pParentCtx, Layer* right) const
{
	assert(pParentCtx);
	assert(right);

	for (size_t i = right->lastListenIdx.pos(startIdx); i < input.size(); i++)
	{
		// it is very important to mention that this is loop is sensitive to
		// the sentences order because the subjects can intersect. So 
		// the further logic should support multiple possible outcomes of 
		// the noun searching. 

		for(SentenceIter it(pParentCtx); it; ++it)
		{
			auto sentence = *it;
			size_t szLtr = sentence->noun->LetterSize();
			if (i + szLtr > input.size())
			{
				continue;
			}

			if (sentence->Search(LetterView(input, i)))
			{
				DotArray res = sentence->Act(LetterView(input, i));
				for (size_t j = 0; j < res.GetSizeInLetters(); j++)
				{
					right->AddLetter(res.GetLetter(j));
				}

				right->lastListenIdx = Index(i + szLtr, startIdx);
				i += szLtr - 1; //1 will be added back in the for loop
				break;
			}
		}
	}
}


bool Layer::Say(Context::Ptr ctx, Layer* right)
{
	assert(ctx);
	assert(right);

	auto parentCtx = ctx->GetParent();

	assert(parentCtx);

	PreSay(parentCtx, right);

	ContextCompiler compiler(neocortex->GetHashCalculator());
	return compiler.Compile(LetterView(right->input), ctx);
}

AnswerPayloadInit::AnswerPayloadInit(Layer* inLayer)
	: pLayer(inLayer)
{}

bool AnswerPayloadInit::Say(AnswerPayload::Ptr pLeft, std::vector<AnswerPayload::Ptr>& newNodes) 
{
	assert(false);
	return false;
}

void AnswerPayloadInit::Think(bool bRoot, std::vector<AnswerPayload::Ptr>& newNodes)
{
	assert(false);
}

AnswerPayload::Ptr AnswerPayloadInit::Clone() const
{
	assert(false);
	return nullptr;
}

AnswerPayload::Ptr AnswerPayloadInit::CreateChild() const
{
	assert(false);
	return nullptr;
}

bool AnswerPayloadInit::IsEmpty() const
{
	assert(false);
	return true;
}


AnswerPayloadImpl::AnswerPayloadImpl(Neocortex* inNeocortex, Context::CPtr inParentContext)
	: neocortex(inNeocortex)
	, layer(inNeocortex)
{
	pParentContext = inParentContext;
	pChildContext = pParentContext->CreateEmptyChild();
}

AnswerPayloadImpl::AnswerPayloadImpl(Neocortex* inNeocortex, Context::CPtr inParentContext, Context::Ptr inChildContext, Layer const& inLayer)
	: neocortex(inNeocortex)
	, pParentContext(inParentContext)
	, pChildContext(inChildContext)
	, layer(inLayer)
{

}

bool AnswerPayloadImpl::Say(Layer* left, std::vector<Context::Ptr>& newCtxs)
{
	newCtxs.clear();
	size_t inSentenceCount = pChildContext->GetSentences().size();
	bool sayRes = left->Say(pChildContext, &layer);
	if (!sayRes)
	{
		return false; // kill me pls
	}
	size_t outSentenceCount = pChildContext->GetSentences().size();
	if (inSentenceCount == outSentenceCount)
	{
		return true;
	}

	newCtxs = pChildContext->CloneAndShuffle(inSentenceCount);

	return true;
}


bool AnswerPayloadImpl::Say(AnswerPayload::Ptr pLeft, std::vector<AnswerPayload::Ptr>& newNodes) 
{
	Layer* leftLayer = nullptr;

	if (auto pImp = std::dynamic_pointer_cast<AnswerPayloadImpl>(pLeft))
	{
		leftLayer = &pImp->layer;
	}
	else if (auto pInit = std::dynamic_pointer_cast<AnswerPayloadInit>(pLeft))
	{
		leftLayer = pInit->pLayer;
	}
	else
	{
		assert(false);
		return false;
	}

	std::vector<Context::Ptr> newCtxs;

	bool res = Say(leftLayer, newCtxs);

	if (!res)
	{
		return false;
	}

	for (auto ctx : newCtxs)
	{
		newNodes.push_back(std::make_shared<AnswerPayloadImpl>(neocortex, pParentContext, ctx, layer));
	}

	return true;
}

void AnswerPayloadImpl::Think(bool bRoot, std::vector<AnswerPayload::Ptr>& newNodes)
{
	std::vector<Context::Ptr> newCtxs;

	size_t inSentenceCount = pChildContext->GetSentences().size();

	ContextCreativity creativity(neocortex->GetHashCalculator(), neocortex->GetTIM());
	size_t newSentenceCount = creativity.Imagine(pChildContext, bRoot, newCtxs);

	for (auto ctx : pChildContext->CloneAndShuffle(inSentenceCount))
	{
		newNodes.push_back(std::make_shared<AnswerPayloadImpl>(neocortex, pParentContext, ctx, layer));
	}

	for (auto ctx : newCtxs)
	{
		for (auto shuffled : ctx->CloneAndShuffle(inSentenceCount))
		{
			newNodes.push_back(std::make_shared<AnswerPayloadImpl>(neocortex, pParentContext, shuffled, layer));
		}
	}
}


AnswerPayload::Ptr AnswerPayloadImpl::Clone() const
{
	return std::make_shared<AnswerPayloadImpl>(neocortex, pParentContext);
}

AnswerPayload::Ptr AnswerPayloadImpl::CreateChild() const
{
	return std::make_shared<AnswerPayloadImpl>(neocortex, pChildContext);
}

bool AnswerPayloadImpl::IsEmpty() const
{
	return layer.IsEmpty();

}



Neocortex::Neocortex(size_t layerCount, size_t maxSize, size_t maxTreeHorizontalRank)
	: layers()
{
	assert(layerCount > 1);
	assert(maxSize > 2);
	assert(maxTreeHorizontalRank > 2);

	maxLayerSize = maxSize;

	answerTree.maxHorizontalRank = maxTreeHorizontalRank;

	layers.resize(layerCount, this);

	for (size_t i = layers.size(); i > 0; i--)
	{
		answerTree.CreateLeftmostNode(std::make_shared<AnswerPayloadInit>(&layers[i - 1]));
	}
}

void Neocortex::SetGlobalContext(Context::CPtr inGlobal)
{
	pGlobal = inGlobal;
	answerTree.CreateNode(nullptr, answerTree.GetRoot(), std::make_shared<AnswerPayloadImpl>(this, pGlobal));
}


void Neocortex::Listen()
{
	for (size_t i = 0; i < layers.size()-1; i++)
	{
		layers[i].Listen(pGlobal, &layers[i + 1]);
	}
}

bool Neocortex::TryToSay(std::vector<std::deque<Word> >& out)
{
	answerTree.pPayload.clear();

	bool res = RunQueue<AnswerTreeNodePayloadSayOp>(answerTree.GetRoot());
	if (!res)
	{
		return false;
	}

	for (auto it = answerTree.pPayload.cbegin(); it != answerTree.pPayload.cend(); ++it)
	{
		auto pImp = std::dynamic_pointer_cast<AnswerPayloadImpl>(*it);
		std::deque<Word> outWords;
		bool isOk = true;

		for (auto l : pImp->layer.GetInput())
		{
			auto type = LetterTools::Type(l);
			if (type == LetterType::Ignored)
			{
				continue;
			}
			else if (type != LetterType::Word)
			{
				isOk = false;
				break;
			}

			outWords.push_back(Word::FromBits(LetterTools::GetWord(l)));
		}

		if (isOk)
		{
			out.push_back(outWords);
		}
	}

	return true;
}

void Neocortex::Think()
{
	RunQueue<AnswerTreeNodePayloadThinkOp>(answerTree.GetRoot());
}

void Neocortex::SetInput(std::deque<Word> const& inWords)
{
	std::deque<Letter> inLetters;

	for (auto w : inWords)
	{
		inLetters.push_back(LetterTools::PutWord(w.ToBits()));
	}

	layers[0].SetInput(inLetters);
}

