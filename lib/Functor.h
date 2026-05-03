/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#pragma once


#include "ScalarFunction.h"
#include "VectorFunction.h"
#include "Projector.h"

// the code is taken from https://stackoverflow.com/questions/24314727/remove-pointer-analog-that-works-for-anything-that-supports-operator

template <typename T>
class remove_pointer_ {
    template <typename U = T>
    static auto test(int) -> std::remove_reference<decltype(*std::declval<U>())>;
    static auto test(...) -> std::remove_cv<T>;

public:
    using type = typename decltype(test(0))::type;
};

template <typename T>
using remove_pointer = typename remove_pointer_<T>::type;

// Functor is a second level function, first level functions are its arguments. 
// Additionally it can accept projectors acting like a function over subsets. Using functor
// we can implement second level logic and second level set theory-ish. 
class Functor : public ScalarFunction
{
public:

    Functor(ScalarFunction&& f)
        : ScalarFunction(std::move(f))
    {
    }

    Functor(uint64_t inArity, uint64_t inCode)
    {
        arity = inArity;
        coefficients.FromBits(inCode, Pow2(arity));
    }

    template<class T,
        std::enable_if_t<!std::is_convertible_v<std::remove_pointer<std::remove_all_extents<T> >, VectorFunction> 
                      && !std::is_convertible_v<std::remove_pointer<std::remove_all_extents<T> >, Projector>, bool> = true>
    auto operator()(const T& args) const
    {
        return ScalarFunction::Apply(args);
    }

    template<typename It>
    auto ApplyVecFunc(It first, It last) const
    {
        assert(arity == std::distance(first, last));

        uint64_t resDim = BitTraits<uint64_t>::xFF, resAr = BitTraits<uint64_t>::xFF;
        for (auto it = first; it != last; ++it)
        {
            resDim = std::min(Helper::Deref(*it).dimensions, resDim);
            resAr = std::min(Helper::Deref(*it).arity, resAr);
        }

        const auto& begin = Helper::Deref(*first);
        VectorFunction::Ptr res = std::make_shared<VectorFunction>(resDim, resAr);

        if (Empty())
        {
            return res;
        }

        res->coordinates.reserve(res->dimensions);

        for (size_t i = 0; i < res->dimensions; i++)
        {
            res->coordinates.emplace_back(ScalarFunction(res->arity));
        }

        //this loop is a hotspot for parallelisation 
        for (size_t i = 0; i < res->dimensions; i++)
        {
            std::vector<const ScalarFunction*> args(arity);

            size_t j = 0;
            for (auto it = first; it != last; ++it)
            {
                args[j++] = &(Helper::Deref(*it).coordinates[i]);
            }

            res->coordinates[i] = Apply(args);
        }

        return res;
    }

    template<template<class, class...> class Cont, class... Args>
    auto operator()(const Cont<VectorFunction::Ptr, Args...>& objs) const
    {
        return ApplyVecFunc(std::begin(objs), std::end(objs));
    }

    template<template<class, class...> class Cont, class... Args>
    auto operator()(const Cont<VectorFunction::CPtr, Args...>& objs) const
    {
        return ApplyVecFunc(std::begin(objs), std::end(objs));
    }

    template<template<class, class...> class Cont, class... Args>
    auto operator()(const Cont<Projector::Ptr, Args...>& objs) const
    {
        auto proj = Apply(std::begin(objs), std::end(objs));

        proj.Pack();

        return std::make_shared<Projector>(std::move(proj));
    }

    template<template<class, class...> class Cont, class... Args>
    auto operator()(const Cont<Projector::CPtr, Args...>& objs) const
    {
        auto proj = Apply(std::begin(objs), std::end(objs));

        proj.Pack();

        return std::make_shared<Projector>(std::move(proj));
    }
};
