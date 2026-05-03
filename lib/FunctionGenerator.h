/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#pragma once

#include "DotArray.h"
#include "ScalarFunction.h"
#include "VectorFunction.h"

class ScalarFunctionGenerator
{
public:
	ScalarFunctionGenerator(uint64_t inArity)
		: arity(inArity)
	{

	}

	template< class Generator >
	void Begin(Generator& generator)
	{
		truthTable = DotArray::GenerateRandom(Pow2(arity), generator);
	}

	void Begin()
	{
		truthTable = DotArray(Pow2(arity));
	}

	void InsertTruthTableValue(Dot res, DotArray const& args)
	{
		uint64_t idx = args.ToBits() & (Pow2(arity) - 1);
		truthTable[idx] = res;
	}

	void InsertTruthTableValue(Dot res, uint64_t args)
	{
		truthTable[args & (Pow2(arity) - 1)] = res;
	}

	ScalarFunction End()
	{
		ScalarFunction f(arity);
		f.LazyInit();
		for (size_t i = 0; i < truthTable.size(); ++i)
		{
			Dot f_i = f(i);
			Dot d_i = truthTable[i];
			f.coefficients[i] ^= f_i ^ d_i;
		}
		truthTable.clear();
		return f;
	}

	template< class Generator >
	ScalarFunction GenRandom(Generator& generator)
	{
		ScalarFunction f(arity);
		f.coefficients = DotArray::GenerateRandom(Pow2(arity), generator);
		return f;
	}

private:
	DotArray truthTable;
	uint64_t arity;
};

class VectorFunctionGenerator
{
public:
	VectorFunctionGenerator(uint64_t inDimensions, uint64_t inArity)
		: dimensions(inDimensions)
		, arity(std::min(inArity, SentenceMaxArity))
	{
		gens.resize(dimensions, arity);
	}

	void Begin()
	{
		for (auto& gen : gens)
		{
			gen.Begin();
		}
	}

	template< class Generator >
	void Begin(Generator& generator)
	{
		for (auto& gen : gens)
		{
			gen.Begin(generator);
		}
	}

	void InsertTruthTableValue(DotArray const& res, DotArray const& args)
	{
		uint64_t idx = args.ToBits() & (Pow2(arity)-1);

		for (size_t i = 0; i < dimensions; ++i)
		{
			gens[i].InsertTruthTableValue(i < res.size() ? res[i] : 0, idx);
		}
	}

	VectorFunction::Ptr End()
	{
		VectorFunction::Ptr fRes = std::make_shared<VectorFunction>(dimensions, arity);

		fRes->coordinates.reserve(fRes->dimensions);

		for (size_t i = 0; i < dimensions; i++)
		{
			fRes->coordinates.emplace_back(gens[i].End());
		}

		return fRes;
	}

	template< class Generator >
	VectorFunction::Ptr GenRandom(Generator& generator)
	{
		VectorFunction::Ptr fRes = std::make_shared<VectorFunction>(dimensions, arity);

		fRes->coordinates.reserve(fRes->dimensions);

		for (size_t i = 0; i < dimensions; i++)
		{
			fRes->coordinates.emplace_back(gens[i].GenRandom(generator));
		}

		return fRes;
	}

private:

	std::vector<ScalarFunctionGenerator> gens;
	uint64_t dimensions;
	uint64_t arity;
};
