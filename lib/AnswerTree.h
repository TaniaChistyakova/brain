/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#pragma once

#include <memory>
#include <deque>
#include <vector>


class AnswerTree;
class AnswerPayload;
class AnswerTreeNode;
class AnswerTreeNodeOp;

typedef AnswerTree* AnswerTreePtr;
typedef const AnswerTree* AnswerTreeCPtr;
typedef std::shared_ptr<AnswerTreeNode> AnswerTreeNodePtr;
typedef std::shared_ptr<const AnswerTreeNode> AnswerTreeNodeCPtr;
typedef std::shared_ptr<AnswerTreeNodeOp> AnswerTreeNodeOpPtr;


class AnswerPayload : public std::enable_shared_from_this<AnswerPayload>
{
public:
	typedef std::shared_ptr<AnswerPayload> Ptr;
	typedef std::shared_ptr<const AnswerPayload> CPtr;

	virtual bool Say(AnswerPayload::Ptr pLeft, std::vector<AnswerPayload::Ptr>& newNodes) = 0;
	virtual void Think(bool bRoot, std::vector<AnswerPayload::Ptr>& newNodes) = 0;
	virtual AnswerPayload::Ptr Clone() const = 0;
	virtual AnswerPayload::Ptr CreateChild() const = 0;
	virtual bool IsEmpty() const = 0;
};

class AnswerTree
{
public:
	typedef AnswerTreePtr Ptr;
	typedef AnswerTreeCPtr CPtr;

	AnswerTree();
	~AnswerTree();

	AnswerTreeNodePtr CreateNode(AnswerTreeNodePtr inUpParent, AnswerTreeNodePtr inLeftParent, AnswerPayload::Ptr InPayload);
	AnswerTreeNodePtr CreateLeftmostNode(AnswerPayload::Ptr InPayload);
	void DeleteSingleNode(AnswerTreeNodePtr pNode);

	AnswerTreeNodePtr GetRoot();

	size_t maxVerticalRank;
	size_t maxHorizontalRank;

	std::vector<AnswerTreeNodePtr> pLeftmost;

	std::vector<AnswerPayload::Ptr> pPayload;
};

class AnswerTreeNode : public std::enable_shared_from_this<AnswerTreeNode>
{
public:
	typedef AnswerTreeNodePtr Ptr;
	typedef AnswerTreeNodeCPtr CPtr;

	AnswerTreeNode(AnswerPayload::Ptr InPayload);

	Ptr GetLeftmost();
	Ptr GetTop();
	Ptr GetRoot();
	bool IsEmpty() const;

	Ptr pUpParent;
	Ptr pLeftParent;
	std::vector<Ptr> pRightChildren;
	std::vector<Ptr> pDownChildren;

	AnswerTree::Ptr pAnswerTree;

	size_t verticalRank;
	size_t horizontalRank;

	bool isNew;

	AnswerPayload::Ptr pPayload;
};

class AnswerTreeNodeOp
{
public:
	typedef std::shared_ptr<AnswerTreeNodeOp> Ptr;
	virtual bool Run(std::deque<AnswerTreeNodeOp::Ptr>& ops) = 0;
};

class AnswerTreeNodePayloadSayOp : public AnswerTreeNodeOp
{
public:
	AnswerTreeNodePayloadSayOp(AnswerTreeNode::Ptr inNode)
		: pNode(inNode)
	{}

	bool Run(std::deque<AnswerTreeNodeOp::Ptr>& ops) override;

	AnswerTreeNode::Ptr pNode;
};


class AnswerTreeNodePayloadThinkOp : public AnswerTreeNodeOp
{
public:
	AnswerTreeNodePayloadThinkOp(AnswerTreeNode::Ptr inNode)
		: pNode(inNode)
	{}

	bool Run(std::deque<AnswerTreeNodeOp::Ptr>& ops) override;

	AnswerTreeNode::Ptr pNode;
};

template<class NextOp>
class AnswerTreeNodeCreateOp : public AnswerTreeNodeOp
{
public:
	AnswerTreeNodeCreateOp(AnswerTreeNode::Ptr inUpParent, AnswerTreeNode::Ptr inLeftParent, AnswerPayload::Ptr InPayload)
		: pUpParent(inUpParent), pLeftParent(inLeftParent), pPayload(InPayload)
	{}

	bool Run(std::deque<AnswerTreeNodeOp::Ptr>& ops) override;

	AnswerTreeNode::Ptr pUpParent;
	AnswerTreeNode::Ptr pLeftParent;
	AnswerPayload::Ptr pPayload;
};

template<class NextOp>
class AnswerTreeNodeDuplicateOp : public AnswerTreeNodeOp
{
public:
	AnswerTreeNodeDuplicateOp(AnswerTreeNode::Ptr inUpParent, AnswerTreeNode::Ptr inLeftParent, AnswerTreeNode::Ptr inNode, AnswerTreeNode::Ptr inInitNode, AnswerPayload::Ptr InPayload)
		: pUpParent(inUpParent), pLeftParent(inLeftParent), pNode(inNode), pInitNode(inInitNode), pPayload(InPayload)
	{}

	bool Run(std::deque<AnswerTreeNodeOp::Ptr>& ops) override;

	AnswerTreeNode::Ptr pUpParent;
	AnswerTreeNode::Ptr pLeftParent;
	AnswerTreeNode::Ptr pNode;
	AnswerTreeNode::Ptr pInitNode;
	AnswerPayload::Ptr pPayload;
};


class AnswerTreeNodeRunDeleteOp : public AnswerTreeNodeOp
{
public:
	AnswerTreeNodeRunDeleteOp(AnswerTreeNode::Ptr inNode)
		: pNode(inNode)
	{}

	bool Run(std::deque<AnswerTreeNodeOp::Ptr>& ops) override;

	AnswerTreeNode::Ptr pNode;
};

template<typename Op, typename ...Args>
void CreateOp(std::deque<AnswerTreeNodeOp::Ptr>& ops, Args... args)
{
	ops.push_front(std::make_shared<Op>(std::forward<Args>(args)...));
}


template<typename Op, typename ...Args>
bool RunQueue(Args... args)
{
	std::deque<AnswerTreeNodeOp::Ptr> ops;
	CreateOp<Op>(ops, std::forward<Args>(args)...);
	bool res = false;
	while (ops.size())
	{
		auto op = ops.front();
		ops.pop_front();
		if (op->Run(ops))
		{
			res = true;
		}
	}

	return res;
}

