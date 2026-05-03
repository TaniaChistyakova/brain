/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#pragma once

#include <iterator>
#include <memory>

#include "ScalarFunction.h"
#include "ContextObject.h"

// Vector function is a fixed set of scalar function. Vector function acts 
// from *{0,1} to *{0,1}

class VectorFunction : public ContextObject
{
    friend class Functor;
    friend class VectorFunctionGenerator;
public:

    typedef std::shared_ptr<VectorFunction> Ptr;
    typedef std::shared_ptr<const VectorFunction> CPtr;

    VectorFunction(uint64_t inDimensions, uint64_t inArity)
        : dimensions(inDimensions)
        , arity(inArity)
    {
    }

    VectorFunction(int inDimensions, int inArity)
        : VectorFunction((size_t)inDimensions, (size_t)inArity)
    {
    }

    template <typename T>
    VectorFunction(std::initializer_list<T> const& init)
        : VectorFunction(std::begin(init), std::end(init))
    {
    }

    template <typename Container>
    void Load_ForTest(Container&& cont)
    {
        Load_ForTest(std::begin(cont), std::end(cont));
    }

    template <typename Iterator>
    void Load_ForTest(Iterator first, Iterator last)
    {
        assert(dimensions == std::distance(first, last));
        assert(arity == first->Arity());

        coordinates.clear();
        coordinates.reserve(dimensions);
        for (auto it = first; it != last; ++it)
        {
            assert(arity == it->Arity());
            coordinates.emplace_back(it->Copy_ForTest());
        }
    }

    VectorFunction(VectorFunction&&) = default;
    VectorFunction& operator = (VectorFunction&&) = default;

    VectorFunction(VectorFunction const&) = default;
    VectorFunction& operator = (VectorFunction const&) = default;

    uint64_t Arity() const
    {
        return arity;
    }

    uint64_t Dimensions() const
    {
        return dimensions;
    }

    VectorFunction& operator &= (VectorFunction const& f)
    {
        arity = std::min(arity, f.arity);
        dimensions = std::min(dimensions, f.dimensions);
        coordinates.erase(coordinates.begin() + dimensions, coordinates.end());

        for (size_t i = 0; i < dimensions; i++)
        {
            coordinates[i] &= f.coordinates[i];
        }
        return *this;
    }

    VectorFunction& operator ^= (VectorFunction const& f)
    {
        arity = std::min(arity, f.arity);
        dimensions = std::min(dimensions, f.dimensions);
        coordinates.erase(coordinates.begin() + dimensions, coordinates.end());

        for (size_t i = 0; i < dimensions; i++)
        {
            coordinates[i] ^= f.coordinates[i];
        }
        return *this;
    }

    friend VectorFunction operator ^ (VectorFunction const& f1, VectorFunction const& f2)
    {
        VectorFunction f(f1);
        f ^= f2;
        return f;
    }

    friend VectorFunction operator & (VectorFunction const& f1, VectorFunction const& f2)
    {
        VectorFunction f(f1);
        f &= f2;
        return f;
    }

    template<typename BitBucket>
    DotArray operator() (BitBucket const& args) const
    {
        // any scalar function takes 2**arity bits memory,
        // so for practical reasons we can't use more than 64 arguments 

        uint64_t argsAsULongLong = args.ToBits();
        argsAsULongLong &= (Pow2(arity) - BitTraits<uint64_t>::x1);
        DotArray res(dimensions);

        if (coordinates.empty())
        {
            return res;
        }

        for (size_t i = 0; i < dimensions; i++)
        {
            res[i] = coordinates[i](argsAsULongLong);
        }
        return res;
    }

    void ReduceArity_ForTest(size_t newArity)
    {
        assert(newArity <= arity);

        arity = newArity;

        for (size_t i = 0; i < dimensions; i++)
        {
            coordinates[i].ReduceArity_ForTest(newArity);
        }
    }

    void ReduceDimensions_ForTest(size_t newDimensions)
    {
        assert(newDimensions <= dimensions);

        if (newDimensions == dimensions)
        {
            return;
        }
        dimensions = newDimensions;
        coordinates.erase(coordinates.begin() + dimensions, coordinates.end());
    }

    void AddToHash(class HashCalculator& calc) const override;
    virtual Type GetType() const override;

private:

    //think about it later! we will need it when we put VectorFunction into ScalarFunction as an argument
    VectorFunction(const DotArray&) = delete; 

    //default ctor should not be used outside, it's for performance
    VectorFunction()
    {
        arity = 0;
        dimensions = 0;
    }

	std::vector<ScalarFunction> coordinates;

	uint64_t arity;
	uint64_t dimensions;
};


