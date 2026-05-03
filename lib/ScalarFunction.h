/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#pragma once

#include "BMath.h"
#include "Dot.h"
#include "DotArray.h"

#include <memory>
#include <vector>
#include <algorithm>

// Scalar function is Algebraic normal form or Zhegalkin polynomial
// https://en.wikipedia.org/wiki/Zhegalkin_polynomial
// a function over \mathbb{F}_2
// Additionally we can define XOR AND operators acting over set of polynomial some arity
// Constants 1 and 0 implemented with ScalarFunction(Dot d, uint64_t newArgCount) ctor
// ScalarFunctions with arity N form a group over XOR and monoid over AND, so
// they are a commutative ring

class ScalarFunction
{
public:
    ScalarFunction(uint64_t newArgCount)
    {
        Resize(newArgCount);
    }

    ScalarFunction(Dot d, uint64_t newArgCount)
    {
        Resize(newArgCount);
        if (d)
        {
            LazyInit();
            coefficients[0] = d;
        }
    }

    uint64_t Arity() const
    {
        return arity;
    }

    template<typename T>
    auto operator() (const T& args) const
    {
        return Apply(args);
    }

    // this function should be used for testing purposes only
    void Load_ForTest(std::vector<Dot> const& data)
    {
        LazyInit();

        assert(Pow2(arity) == data.size());
        assert(Pow2(arity) == coefficients.size());

        uint64_t offset = 0;
        uint64_t n = arity;

        for (uint64_t k = 0; k <= n; k++)
        {
            std::string bitmask(k, 1); // K leading 1's
            bitmask.resize(n, 0); // N-K trailing 0's

            // print integers and permute bitmask
            do
            {
                bool res = false;
                uint64_t id;
                res = StringToBits_ForTest(bitmask, id);
                assert(res);
                coefficients[id] = data[offset];
                ++offset;
            } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
        }
    }

    ScalarFunction bitwiseXor_ForTest(ScalarFunction const& f) const
    {
        assert(arity == f.arity);
        ScalarFunction res(arity);
        
        if (Empty() && f.Empty())
        {
            return res;
        }

        if (Empty())
        {
            res.coefficients = f.coefficients;
            return res;
        }

        if (f.Empty())
        {
            res.coefficients = coefficients;
            return res;
        }

        assert(coefficients.size() == f.coefficients.size());
        assert(Pow2(arity) == coefficients.size());
        assert(Pow2(f.arity) == f.coefficients.size());

        if (coefficients == f.coefficients)
        {
            return res;
        }

        res.LazyInit();

        for (uint64_t i = 0; i < coefficients.size(); i++)
        {
            res.coefficients[i] = coefficients[i] ^ f.coefficients[i];
        }

        return res;
    }

    ScalarFunction& operator ^= (ScalarFunction const& f)
    {
        arity = std::min(arity, f.arity);

        if (Empty() && f.Empty())
        {
            return *this;
        }

        if (Empty())
        {
            coefficients = f.coefficients;
            return *this;
        }

        if (f.Empty())
        {
            return *this;
        }

        coefficients ^= f.coefficients;

        return *this;
    }

    ScalarFunction bitwiseAnd_ForTest(ScalarFunction const& f) const
    {
        assert(arity == f.arity);
        ScalarFunction res(arity);

        if (Empty())
        {
            return res;
        }

        if (f.Empty())
        {
            return res;
        }

        res.LazyInit();

        assert(coefficients.size() == f.coefficients.size());
        assert(Pow2(arity) == coefficients.size());
        assert(Pow2(f.arity) == f.coefficients.size());

        for (uint64_t i = 0; i < coefficients.size(); i++)
        {
            for (uint64_t j = 0; j < coefficients.size(); j++)
            {
                uint64_t newIndex = i | j;
                res.coefficients[newIndex] ^= coefficients[i] & f.coefficients[j];
            }
        }

        return res;
    }

    ScalarFunction& operator &= (ScalarFunction const& f)
    {
        arity = std::min(arity, f.arity);

        if (Empty())
        {
            return *this;
        }

        if (f.Empty())
        {
            coefficients.clear();
            return *this;
        }

        auto oldCoef = std::move(coefficients);

        LazyInit();
        oldCoef.FunctionalAnd(f.coefficients, coefficients);
        return *this;
    }


    friend ScalarFunction operator ^ (ScalarFunction const& f1, ScalarFunction const& f2)
    {
        ScalarFunction f(f1);

        f ^= f2;

        return f;
    }

    friend ScalarFunction operator & (ScalarFunction const& f1, ScalarFunction const& f2)
    {
        ScalarFunction f(f1);
        f &= f2;

        return f;
    }

    ScalarFunction& operator &= (Dot d)
    {
        if (!d)
        {
            coefficients.clear();
        }
        return *this;
    }

    ScalarFunction& operator ^= (Dot d)
    {
        if (d ^ Dot())
        {
            LazyInit();
        }

        if (!Empty())
        {
            coefficients[0] ^= d;
        }
        return *this;
    }

    ScalarFunction Copy_ForTest() const
    {
        ScalarFunction f;
        f.arity = arity;
        f.coefficients = coefficients;
        return f;
    }

    size_t Size() const
    {
        return arity;
    }

    void ReduceArity_ForTest(size_t newArity)
    {
        assert(newArity <= arity);

        using Traits = BitTraits<uint64_t>;

        size_t oldArity = arity;
        arity = newArity;

        if (Empty() || newArity == oldArity)
        {
            return;
        }

        coefficients.resize(Pow2(arity));
    }

    bool isConstant() const
    {
        if (arity == 0 || Empty())
        {
            return true;
        }

        for (size_t i = 1; i < coefficients.size(); i++)
        {
            if (coefficients[i])
            {
                return false;
            }
        }

        return true;
    }

protected:

    friend class ScalarFunctionGenerator;
    friend class VectorFunction;

    static inline bool StringToBits_ForTest(std::string const& str, uint64_t & bitfield)
    {
        bitfield = 0;

        if (str.length() > std::numeric_limits<uint64_t>::digits)
        {
            return false;
        }

        for (size_t i = 0; i < str.length(); i++)
        {
            auto val = str[i];
            if (val && val != '0')
            {
                bitfield |= 1LL << i;
            }
        }

        return true;
    }


    ScalarFunction()
    {
        arity = 0;
    }

    void Resize(uint64_t newArgCount)
    {
        assert(newArgCount != 0);

        arity = newArgCount;
    }

    void LazyInit()
    {
        if (Empty())
        {
            coefficients.resize(Pow2(arity));
        }
    }

    bool Empty() const
    {
        return coefficients.empty();
    }

    struct Helper
    {
        template<typename T>
        static const T& Deref(const T& obj) { return obj; }

        template<typename T>
        static const T& Deref(const T* obj) { return *obj; }

        template<typename T>
        static T& Deref(T& obj) { return obj; }

        template<typename T>
        static T& Deref(T* obj) { return *obj; }

        template<typename T>
        static T& Deref(std::shared_ptr<T> obj) { return *obj; }

        template<typename T>
        static T Ctor(Dot coef, const T& arg, size_t sz) { return T(coef, sz); }

        static Dot Ctor(Dot coef, const Dot& arg, size_t sz) { return coef; }

        template<typename T>
        static size_t Size(const T& arg) { return arg.Size(); }

        static size_t Size(const Dot& arg) { return 1; }

        template<typename F>
        static bool IsNone(const F& f)
        {
            return f.Empty();
        }

        static bool IsNone(const Dot& t)
        {
            return !t;
        }
    };

    template<typename T>
    auto Apply(T begin, T end) const
    {
        assert(std::distance(begin, end) == arity);

        size_t sz = 0;
        for (auto it=begin; it != end; ++it)
        {
            sz = std::max(sz, Helper::Size(Helper::Deref(*it)));
        }

        if (Empty())
        {
            return Helper::Ctor(Dot(), Helper::Deref(*begin), sz);
        }

        assert(Pow2(arity) == coefficients.size());

        auto result = Helper::Ctor(coefficients[0], Helper::Deref(*begin), sz);

        for (uint64_t i = 1; i < coefficients.size(); i++)
        {
            auto coef = coefficients[i];
            if (!coef)
            {
                continue;
            }

            for (uint64_t j = 0; j < arity; j++)
            {
                uint64_t offset = j;
                if (!!(i & (1LL << offset)))
                {
                    T it = begin;
                    std::advance(it, j++);
                    auto tmp = Helper::Deref(*it);

                    for (; (!Helper::IsNone(tmp)) && j < arity; j++)
                    {
                        uint64_t offset = j;
                        if (!!(i & (1LL << offset)))
                        {
                            T it = begin;
                            std::advance(it, j);
                            tmp &= Helper::Deref(*it);
                        }
                    }
                    result ^= tmp;

                    break;
                }
            }

        }
        return result;
    }

    template<typename T>
    auto Apply(const T& args) const
    {
        return Apply(std::begin(args), std::end(args));
    }

    // useless version, just for experimenting
    Dot Apply(const DotArray& args) const
    {
        if (Empty())
        {
            return Dot();
        }

        assert(Pow2(arity) == coefficients.size());

        Dot result = coefficients[0];

        for (uint64_t i = 1; i < coefficients.size(); i++)
        {
            Dot coef = coefficients[i];
            if (!coef)
            {
                continue;
            }

            Dot tmp = coef;
            for (uint64_t j = 0; (!!(tmp)) && j < arity; j++)
            {
                uint64_t offset = j;
                if (!!(i & (1LL << offset)))
                {
                    tmp &= args[j];
                }
            }
            result ^= tmp;
        }

        return result;
    }

    Dot Apply(const uint64_t args) const
    {
        if (Empty())
        {
            return Dot();
        }

        Dot result = coefficients[0];
        const uint64_t maxArgs = std::min(args + 1, coefficients.size());

        for (uint64_t i = 1; i < maxArgs; i++)
        {
            Dot tmp(((args & i) == i));
            result ^= coefficients[i] & tmp;
        }

        return result;
    }

    DotArray coefficients; //Zhegalkin polynomial
    uint64_t arity;
};
