/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#pragma once

#include "Letter.h"
#include "DotArray.h"

class DotArray;

template <typename ContT>
class LetterView
{
public:

	LetterView(ContT& inCont, size_t inStart)
		: container(inCont)
		, start(inStart)
		, end(inCont.size())
	{
		assert(container.size() >= sizeInLetters());
	}

	LetterView(ContT& inCont)
		: container(inCont)
		, start(0)
		, end(inCont.size())
	{
		assert(container.size() >= sizeInLetters());
	}

	LetterView(ContT& inCont, size_t inStart, size_t inEnd)
		: container(inCont)
		, start(inStart)
		, end(inEnd)
	{
		assert(container.size() >= sizeInLetters());
	}


	Letter operator[] (size_t i) const
	{
		assert(i <= end - start);
		return container[start + i];
	}

	Letter& operator[] (size_t i)
	{
		assert(i <= end - start);
		return container[start + i];
	}

	size_t size() const
	{
		return sizeInLetters() * BitTraits<Letter>::x8;
	}

	size_t sizeInLetters() const
	{
		return end - start;
	}

private:
	ContT& container;
	size_t start;
	size_t end;
};



template <>
class LetterView<DotArray>
{
public:

	class DotArraySetter
	{
		DotArray& array;
		const size_t pos;
	public:
		DotArraySetter(DotArray& inArray, const uint64_t inPos) noexcept
			: array(inArray)
			, pos(inPos)
		{
		}

		DotArraySetter& operator = (DotArraySetter const& d) noexcept
		{
			return *this = (Letter)d;
		}

		DotArraySetter& operator = (Letter d) noexcept
		{
			array.SetLetter(pos, d);
			return *this;
		}

		operator Letter () const noexcept
		{
			return array.GetLetter(pos);
		}
	};

	LetterView(DotArray& inDots)
		: dots(inDots)
		, start(0)
		, end(inDots.GetSizeInLetters())
	{
		endInBits = dots.size();
	}

	LetterView(DotArray& inDots, size_t inStart)
		: dots(inDots)
		, start(inStart)
		, end(inDots.GetSizeInLetters())
	{
		endInBits = dots.size();
	}

	LetterView(DotArray& inDots, size_t inStart, size_t inEnd)
		: dots(inDots)
		, start(inStart)
		, end(inEnd)
	{
		endInBits = end * BitTraits<Letter>::x8;
	}

	Letter operator[] (size_t i) const
	{
		return dots.GetLetter(start + i);
	}

	DotArraySetter operator[](size_t pos) noexcept
	{
		return DotArraySetter(dots, pos);
	}

	size_t size() const
	{
		return endInBits - start * BitTraits<Letter>::x8;
	}

	size_t sizeInLetters() const
	{
		return end - start;
	}

private:
	DotArray& dots;
	size_t start;
	size_t end;
	size_t endInBits;
};

