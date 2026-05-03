/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#pragma once

#include <cstdint>


struct Dot
{
    Dot() noexcept
    {
        value = 0;
    }

    Dot(uint64_t v) noexcept
    {
        value = !!v;
    }

    Dot& operator = (uint64_t v) noexcept
    {
        value = v;
        return *this;
    }


    Dot& operator ^= (Dot d) noexcept
    {
        value ^= d.value;
        return *this;
    }

    Dot& operator &= (Dot d) noexcept
    {
        value &= d.value;
        return *this;
    }

    explicit operator bool() const noexcept
    {
        return !!value;
    }

    inline bool operator == (Dot d) const noexcept
    {
        return value == d.value;
    }

    inline bool operator != (Dot d) const noexcept
    {
        return !((*this) == d);
    }

protected:
    friend class DotArray;
    template<uint8_t bitSize, typename _c> friend class DotFixedArray;

    uint8_t value : 1;
};

inline Dot operator ^ (Dot d1, Dot d2) noexcept
{
    d1 ^= d2;
    return d1;
}

inline Dot operator & (Dot d1, Dot d2) noexcept
{
    d1 &= d2;
    return d1;
}

