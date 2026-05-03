/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "AnswerTree.h"

#include "TestTools.h"

#include "gtest/gtest.h"

class AnswerPayloadTest : public AnswerPayload
{
public:
	void Think(bool bRoot, std::vector<AnswerPayload::Ptr>& newNodes) override;
	bool Say(AnswerPayload::Ptr pLeft, std::vector<AnswerPayload::Ptr>& newNodes) override;
	AnswerPayload::Ptr Clone() const override;
	AnswerPayload::Ptr CreateChild() const override;
	bool IsEmpty() const override;
};


bool AnswerPayloadTest::Say(AnswerPayload::Ptr pLeft, std::vector<AnswerPayload::Ptr>& newNodes)
{
	return true;
}

void AnswerPayloadTest::Think(bool bRoot, std::vector<AnswerPayload::Ptr>& newNodes)
{
}

AnswerPayload::Ptr AnswerPayloadTest::Clone() const
{
	return std::make_shared<AnswerPayloadTest>();
}

AnswerPayload::Ptr AnswerPayloadTest::CreateChild() const
{
	return std::make_shared<AnswerPayloadTest>();
}

bool AnswerPayloadTest::IsEmpty() const
{
	return false;
}


TEST(AnswerTreeTest, SmokeTest)
{
	AnswerTree tree;
	AnswerTree::Ptr pAnswerTree = &tree;
	pAnswerTree->maxHorizontalRank = 5;

	AnswerPayload::Ptr pRootAnwer = std::make_shared<AnswerPayloadTest>();
	AnswerTreeNode::Ptr pRoot = pAnswerTree->CreateNode(nullptr, nullptr, pRootAnwer);
	AnswerTreeNode::Ptr pNode1 = pAnswerTree->CreateNode(pRoot, nullptr, pRootAnwer->CreateChild());
	AnswerTreeNode::Ptr pNode2 = pAnswerTree->CreateNode(pNode1, nullptr, pNode1->pPayload->CreateChild());
	AnswerTreeNode::Ptr pNode3 = pAnswerTree->CreateNode(pNode2, nullptr, pNode2->pPayload->CreateChild());

	AnswerTreeNode::Ptr pGrow = pAnswerTree->CreateNode(nullptr, pRoot, pRootAnwer->Clone());

	bool res = RunQueue<AnswerTreeNodePayloadSayOp>(pRoot);

	EXPECT_TRUE(res);
	EXPECT_TRUE(pAnswerTree->pPayload.size() != 0);
	EXPECT_EQ(0, pGrow->pRightChildren.size());
	EXPECT_EQ(5, pGrow->pDownChildren.size());
	EXPECT_EQ(5, pGrow->pDownChildren[0]->pDownChildren.size());

	RunQueue<AnswerTreeNodePayloadThinkOp>(pRoot);

	EXPECT_TRUE(pAnswerTree->pPayload.size() != 0);
	EXPECT_EQ(0, pGrow->pRightChildren.size());
	EXPECT_EQ(5, pGrow->pDownChildren.size());
	EXPECT_EQ(5, pGrow->pDownChildren[0]->pDownChildren.size());


}