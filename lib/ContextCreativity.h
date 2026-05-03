/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#pragma once

#include "Context.h"

enum class OpType { AND = 1, XOR = 0 };
enum class RootType { DYNAMIC = 1, STATIC = 0 };
enum class VersionType { EXTRA = 1, INTRA = 0 };

struct TIM
{
	// which operation is being applied to functions
	OpType functionOp; 

	// operation being applied to projectors
	OpType projectorOp;

	// if static, the root context apply operations on functions, children - to projectors
	// if dynamic otherwise
	RootType root;

	// if extraversion, the root context is being multiplied, children not
	// if intraversion, otherwise
	VersionType version;
};

class ContextCreativity
{
public:
	ContextCreativity(HashCalculator* inHashCalculator, const TIM& inTim);

	size_t Imagine(Context::Ptr ctx, bool isRoot, std::vector<Context::Ptr>& newCtx);

private:
	template<class Iter>
	auto CreateNewObjects(Context::CPtr ctx, OpType op);

	template<class Iter>
	static auto GetExitingObjects(Context::CPtr ctx);

	std::vector<Sentence::CPtr> CreateNewSentences(std::vector<VectorFunction::CPtr> const& functions, std::vector<Projector::CPtr> const& projectors);

	static void AddToContext(Context::Ptr ctx, std::vector<ContextObject::CPtr> const& input);
	static std::vector<Context::Ptr> MultiplyContext(Context::Ptr ctx, std::vector<ContextObject::CPtr> const& input);

	HashCalculator* hashCalculator;
	const TIM tim;
};

