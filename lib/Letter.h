/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#pragma once

#include "BMath.h"

/*
 * This is a special header.
 *
 * It's focused to presentation of the human word. Initially I was thinking to use a variable sized array to make the word. But I found it
 * just impossible to cast variable sized input letter data to a variable sized word. The word should have a fixed size.
 *
 * Then I understood the word is the hash of the mask. I could make any size of hash, including ridiculously big one. But all
 * the words should be in the same memory, so size of hash should be less or equal to 64-bit number.
 *
 * The next step happened when I got the size of hash and the size of all arguments of a functor should be same, because they
 * are replaceable. So the possible sizes were 8, 16, 32 and 64 bits. We can ignore the border cases of 8 and 64 bit for
 * technical reasons.
 *
 * And final step, selection between 16 and 32. A dictionary size of an average person is 10-12k human words in any language.
 * That means the 16 bit wins. And actual hash code size is 14 bits.
 *
 */


 // we clearly have 3 separate types of a letter: a sentence symbol, a functor and a word
 // so we can presume we can spend one bit to i
constexpr size_t LetterTypeSize = 2;

enum class LetterType
{
	Ignored = 0b00,
	Sentence = 0b01,
	Functor = 0b10,
	Word = 0b11,
};

// A human dictionary approximate have 10-12k words, which is a bit less than 2**14=16k 
// we assume some hash codes can be not used because of hash collisions
constexpr size_t WordSize = 14;

// this means the maximal functor arity is 3, 
// it should fit into a word
constexpr size_t FunctorMaxArity = 3;
constexpr size_t FunctorSize = Pow2(FunctorMaxArity); //=8 
constexpr size_t FunctorArityLength = Log2(FunctorMaxArity)+1;

constexpr size_t SentenceObjLengthSize = 5;

// Seems we can clamp arity to this value, it is critically important
// to not overhead the memory
constexpr size_t SentenceMaxArity = Pow2(SentenceObjLengthSize)-1; // ~=31
// we apply this limit in ScalarFunctionGenerator

//maximal function coefficient count is 2^arity, it is 250MB if max arity is 31

// Letter is a access primitive, it can be a word, a sentence signal, a functor
typedef uint16_t Letter;

static_assert(LetterTypeSize + WordSize >= BitTraits<Letter>::x8);
static_assert(FunctorSize <= WordSize);


class LetterTools
{
public:
	static LetterType Type(Letter ltr);
	static uint64_t GetWord(Letter ltr);
	static uint64_t GetFunctorCode(Letter ltr, uint64_t arity);
	static uint64_t GetFunctorArity(Letter ltr);
	static uint64_t GetSentenceObjLength(Letter ltr);

	static Letter PutWord(uint64_t bits);
	static Letter PutFunctor(uint64_t arity, size_t bits);
	static Letter PutSentenceObjLength(uint64_t bits);
};

inline LetterType LetterTools::Type(Letter ltr)
{
	size_t typeBits = ltr & (Pow2(LetterTypeSize) - 1);
	return (LetterType)typeBits;
}

inline uint64_t LetterTools::GetWord(Letter ltr)
{
	return (ltr >> LetterTypeSize);
}

inline uint64_t LetterTools::GetFunctorCode(Letter ltr, uint64_t arity)
{
	return (ltr >> (LetterTypeSize+FunctorArityLength)) & (Pow2(Pow2(arity)) - 1);
}

inline uint64_t LetterTools::GetFunctorArity(Letter ltr)
{
	return (ltr >> LetterTypeSize) & (Pow2(FunctorArityLength) - 1);
}

inline uint64_t LetterTools::GetSentenceObjLength(Letter ltr)
{
	return (ltr >> LetterTypeSize) & SentenceMaxArity;
}

inline Letter LetterTools::PutWord(uint64_t bits)
{
	return Letter(Letter(LetterType::Word)
		| (bits << LetterTypeSize));
}

inline Letter LetterTools::PutFunctor(size_t arity, uint64_t bits)
{
	return Letter(Letter(LetterType::Functor)
		| ((arity & (Pow2(FunctorArityLength)-1)) << LetterTypeSize)
		| (bits << (LetterTypeSize + FunctorArityLength)));

}

inline Letter LetterTools::PutSentenceObjLength(uint64_t bits)
{
	return Letter(Letter(LetterType::Sentence)
		| ((bits & (Pow2(SentenceObjLengthSize) - 1)) << LetterTypeSize));
}
