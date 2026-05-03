/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "AnswerTree.h"

#include <algorithm>
#include <vector>
#include <cassert>


AnswerTree::AnswerTree()
{
	maxVerticalRank = 0;

	maxHorizontalRank = 100;
}

AnswerTree::~AnswerTree()
{
	RunQueue<AnswerTreeNodeRunDeleteOp>(GetRoot());

	pLeftmost.clear();
}


AnswerTreeNode::Ptr AnswerTree::CreateNode(AnswerTreeNode::Ptr inUpParent, AnswerTreeNode::Ptr inLeftParent, AnswerPayload::Ptr InPayload)
{
	AnswerTreeNode::Ptr out = std::make_shared<AnswerTreeNode>(InPayload);
	out->pAnswerTree = this;
	if (inUpParent)
	{
		out->pUpParent = inUpParent;
		inUpParent->pDownChildren.push_back(out);
	}

	if (inLeftParent)
	{
		out->pLeftParent = inLeftParent;
		inLeftParent->pRightChildren.push_back(out);
	}

	{
		AnswerTreeNode::CPtr pNode = out;
		while (pNode->pUpParent)
		{
			out->verticalRank++;
			pNode = pNode->pUpParent;
		}
	}

	{
		AnswerTreeNode::CPtr pNode = out;
		while (pNode->pLeftParent)
		{
			out->horizontalRank++;
			pNode = pNode->pLeftParent;
		}
	}

	if (out->horizontalRank == 0)
	{
		assert(pLeftmost.size() == out->verticalRank);
		pLeftmost.push_back(out);
		maxVerticalRank = pLeftmost.size() - 1;
		out->isNew = false; // don't grow down
	}

	return out;
}

AnswerTreeNodePtr AnswerTree::CreateLeftmostNode(AnswerPayload::Ptr InPayload)
{
	AnswerTreeNode::Ptr out = CreateNode(pLeftmost.empty() ? nullptr : pLeftmost.back(), nullptr, InPayload);
	return out;
}


void AnswerTree::DeleteSingleNode(AnswerTreeNode::Ptr pNode)
{
	if (pNode->pUpParent)
	{
		auto it = std::find(pNode->pUpParent->pDownChildren.begin(), pNode->pUpParent->pDownChildren.end(), pNode);
		assert(it != pNode->pUpParent->pDownChildren.end());
		pNode->pUpParent->pDownChildren.erase(it);
		pNode->pUpParent = nullptr;
	}

	if (pNode->pLeftParent)
	{
		auto it = std::find(pNode->pLeftParent->pRightChildren.begin(), pNode->pLeftParent->pRightChildren.end(), pNode);
		assert(it != pNode->pLeftParent->pRightChildren.end());
		pNode->pLeftParent->pRightChildren.erase(it);
		pNode->pLeftParent = nullptr;
	}
}

AnswerTreeNodePtr AnswerTree::GetRoot()
{
	assert(!pLeftmost.empty());

	return pLeftmost[0];
}


AnswerTreeNode::AnswerTreeNode(AnswerPayload::Ptr InPayload)
	: pPayload(InPayload)
	, verticalRank(0)
	, horizontalRank(0)
	, isNew(true)
{}

AnswerTreeNode::Ptr AnswerTreeNode::GetLeftmost()
{
	return pAnswerTree->pLeftmost[verticalRank];
}

AnswerTreeNode::Ptr AnswerTreeNode::GetTop()
{
	Ptr pNode = shared_from_this();
	while (pNode->pUpParent)
	{
		pNode = pNode->pUpParent;
	}
	return pNode;
}


AnswerTreeNode::Ptr AnswerTreeNode::GetRoot()
{
	return pAnswerTree->GetRoot();
}

bool AnswerTreeNode::IsEmpty() const
{
	return !isNew &&
		horizontalRank != 0 &&
		pDownChildren.empty() &&
		pRightChildren.empty();
}



bool AnswerTreeNodePayloadSayOp::Run(std::deque<AnswerTreeNodeOp::Ptr>& ops)
{
	if (!pNode->pLeftParent)
	{
		for (auto pDownChild : pNode->pDownChildren)
		{
			CreateOp<AnswerTreeNodePayloadSayOp>(ops, pDownChild);
		}

		for (auto pRightChild : pNode->pRightChildren)
		{
			CreateOp<AnswerTreeNodePayloadSayOp>(ops, pRightChild);
		}
		return false;
	}

	std::vector<AnswerPayload::Ptr> newPayload;
	bool isNew = pNode->isNew;
	pNode->isNew = false;

	bool res = pNode->pPayload->Say(pNode->pLeftParent->pPayload, newPayload);

	if (res)
	{
		if (pNode->verticalRank == pNode->pAnswerTree->maxVerticalRank && !pNode->pPayload->IsEmpty())
		{
			pNode->pAnswerTree->pPayload.push_back(pNode->pPayload);
			return true; //we come to the end of processing
		}

		if (newPayload.size())
		{
			for (auto pNewPayload : newPayload)
			{
				CreateOp<AnswerTreeNodeDuplicateOp<AnswerTreeNodePayloadSayOp> >(ops, pNode->pUpParent, pNode->pLeftParent, pNode, pNode, pNewPayload);
			}
		}

		for (auto pRightChild : pNode->pRightChildren)
		{
			CreateOp<AnswerTreeNodePayloadSayOp>(ops, pRightChild);
		}

		if (isNew)
		{
			// try create a down child
			if (pNode->verticalRank < pNode->pAnswerTree->maxVerticalRank)
			{
				AnswerTreeNode::Ptr pNewLeftParent = pNode->GetLeftmost()->pDownChildren[0];
				CreateOp<AnswerTreeNodeCreateOp<AnswerTreeNodePayloadSayOp> >(ops, pNode, pNewLeftParent, pNode->pPayload->CreateChild());
			}

			// try create a right child
			if (!pNode->pPayload->IsEmpty() && pNode->pUpParent && pNode->horizontalRank < pNode->pAnswerTree->maxHorizontalRank)
			{
				CreateOp<AnswerTreeNodeCreateOp<AnswerTreeNodePayloadSayOp> >(ops, pNode->pUpParent, pNode, pNode->pUpParent->pPayload->CreateChild());
			}
		}

	}
	else
	{
		CreateOp<AnswerTreeNodeRunDeleteOp>(ops, pNode);
	}

	return false;
}


bool AnswerTreeNodePayloadThinkOp::Run(std::deque<AnswerTreeNodeOp::Ptr>& ops)
{
	if (!pNode->pLeftParent)
	{
		for (auto pDownChild : pNode->pDownChildren)
		{
			CreateOp<AnswerTreeNodePayloadThinkOp>(ops, pDownChild);
		}

		for (auto pRightChild : pNode->pRightChildren)
		{
			CreateOp<AnswerTreeNodePayloadThinkOp>(ops, pRightChild);
		}
		return false;
	}

	std::vector<AnswerPayload::Ptr> newPayload;

	pNode->pPayload->Think(pNode->pUpParent == nullptr, newPayload);

	if (newPayload.size())
	{
		for (auto pNewPayload : newPayload)
		{
			CreateOp<AnswerTreeNodeDuplicateOp<AnswerTreeNodePayloadThinkOp> >(ops, pNode->pUpParent, pNode->pLeftParent, pNode, pNode, pNewPayload);
		}
	}

	for (auto pRightChild : pNode->pRightChildren)
	{
		CreateOp<AnswerTreeNodePayloadThinkOp>(ops, pRightChild);
	}


	return false;
}

template<class NextOp>
bool AnswerTreeNodeDuplicateOp<NextOp>::Run(std::deque<AnswerTreeNodeOp::Ptr>& ops)
{
	assert(pLeftParent && pPayload);

	AnswerTreeNode::Ptr newNode = pLeftParent->pAnswerTree->CreateNode(pUpParent, pLeftParent, pPayload);

	// we suppose that there is no loops

	for (auto pDownChild : pNode->pDownChildren)
	{
		CreateOp<AnswerTreeNodeDuplicateOp>(ops, newNode, pDownChild->pLeftParent, pDownChild, pInitNode, pDownChild->pPayload->Clone());
	}

	for (auto pRightChild : pNode->pRightChildren)
	{
		CreateOp<AnswerTreeNodeDuplicateOp>(ops, pRightChild->pUpParent, newNode, pRightChild, pInitNode, pRightChild->pPayload->Clone());
	}

	CreateOp<NextOp>(ops, newNode);

	return false;
}


template<class NextOp>
bool AnswerTreeNodeCreateOp<NextOp>::Run(std::deque<AnswerTreeNodeOp::Ptr>& ops)
{
	assert(pLeftParent && pPayload);

	AnswerTreeNode::Ptr newNode = pLeftParent->pAnswerTree->CreateNode(pUpParent, pLeftParent, pPayload);

	CreateOp<NextOp>(ops, newNode);

	return false;
}


bool AnswerTreeNodeRunDeleteOp::Run(std::deque<AnswerTreeNodeOp::Ptr>& ops)
{
	for (auto pRightChild : pNode->pRightChildren)
	{
		CreateOp<AnswerTreeNodeRunDeleteOp>(ops, pRightChild);
	}

	for (auto pDownChild : pNode->pDownChildren)
	{
		CreateOp<AnswerTreeNodeRunDeleteOp>(ops, pDownChild);
	}

	auto pLeftParent = pNode->pLeftParent;
	auto pUpParent = pNode->pUpParent;

	pNode->pAnswerTree->DeleteSingleNode(pNode);

	if (pLeftParent != nullptr && pLeftParent->IsEmpty())
	{
		CreateOp<AnswerTreeNodeRunDeleteOp>(ops, pLeftParent);
	}

	if (pUpParent != nullptr && pUpParent->IsEmpty())
	{
		CreateOp<AnswerTreeNodeRunDeleteOp>(ops, pUpParent);
	}

	return false;
}



