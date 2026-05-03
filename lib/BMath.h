/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#pragma once

#include <limits>
#include <cstdint>
#include <cstddef>


inline constexpr uint64_t Pow2(uint64_t v)
{
    return 1LL << v;
}

// we assume v is a power of two
// from https://stackoverflow.com/questions/11376288/fast-computing-of-log2-for-64-bit-integers
constexpr int tab64[64] = {
    63,  0, 58,  1, 59, 47, 53,  2,
    60, 39, 48, 27, 54, 33, 42,  3,
    61, 51, 37, 40, 49, 18, 28, 20,
    55, 30, 34, 11, 43, 14, 22,  4,
    62, 57, 46, 52, 38, 26, 32, 41,
    50, 36, 17, 19, 29, 10, 13, 21,
    56, 45, 25, 31, 35, 16,  9, 12,
    44, 24, 15,  8, 23,  7,  6,  5 };

inline constexpr int log2_64(uint64_t value)
{
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    value |= value >> 32;
    return tab64[((uint64_t)((value - (value >> 1)) * 0x07EDD5E59A4E28C2)) >> 58];
}

inline constexpr int64_t Log2(uint64_t v)
{
    return log2_64(v);
}


// from https://en.wikipedia.org/wiki/Hamming_weight

//types and constants used in the functions below
//uint64_t is an unsigned 64-bit integer variable type (defined in C99 version of C language)
const uint64_t m1 = 0x5555555555555555; //binary: 0101...
const uint64_t m2 = 0x3333333333333333; //binary: 00110011..
const uint64_t m4 = 0x0f0f0f0f0f0f0f0f; //binary:  4 zeros,  4 ones ...
const uint64_t m8 = 0x00ff00ff00ff00ff; //binary:  8 zeros,  8 ones ...
const uint64_t m16 = 0x0000ffff0000ffff; //binary: 16 zeros, 16 ones ...
const uint64_t m32 = 0x00000000ffffffff; //binary: 32 zeros, 32 ones
const uint64_t h01 = 0x0101010101010101; //the sum of 256 to the power of 0,1,2,3...


//This uses fewer arithmetic operations than any other known  
//implementation on machines with fast multiplication.
//This algorithm uses 12 arithmetic operations, one of which is a multiply.
inline constexpr int popcount64c(uint64_t x)
{
    x -= (x >> 1) & m1;             //put count of each 2 bits into those 2 bits
    x = (x & m2) + ((x >> 2) & m2); //put count of each 4 bits into those 4 bits 
    x = (x + (x >> 4)) & m4;        //put count of each 8 bits into those 8 bits 
    return (x * h01) >> 56;         //returns left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ... 
}

inline constexpr uint8_t OnesInNumber(uint64_t v)
{
    return (uint8_t)popcount64c(v);
}


template <typename T>
struct BitTraits
{
    typedef T DataType;
    static constexpr size_t x8 = std::numeric_limits<T>::digits;
    static constexpr size_t x3 = Log2(x8);
    static constexpr T xFF = std::numeric_limits<T>::max();
    static constexpr T x1 = 1;
    static constexpr size_t b111 = (0x1LL << (x3)) - 0x1LL;
};

