/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#pragma once

#include <vector>
#include <cassert>

#include "Dot.h"
#include "BMath.h"

template<uint8_t _bitSize, typename _c = uint16_t>
class DotFixedArray
{
public:

	typedef size_t size_type;
	typedef _c block_type;
	using Traits = BitTraits<block_type>;
	static constexpr uint8_t bitSize = _bitSize;

	static_assert(bitSize >= sizeof(block_type));

	class DotFixedArraySetter
	{
		DotFixedArray& array;
		const uint64_t pos;
	public:
		DotFixedArraySetter(DotFixedArray& inArray, const uint64_t inPos) noexcept
			: array(inArray)
			, pos(inPos)
		{
		}

		DotFixedArraySetter& operator = (Dot d) noexcept
		{
			array.Set(pos, d);
			return *this;
		}

		DotFixedArraySetter& operator ^= (Dot d) noexcept
		{
			Dot myD = array.Get(pos);
			myD ^= d;
			array.Set(pos, myD);
			return *this;
		}

		DotFixedArraySetter& operator &= (Dot d) noexcept
		{
			Dot myD = array.Get(pos);
			myD &= d;
			array.Set(pos, myD);
			return *this;
		}

		operator Dot () const noexcept
		{
			return array.Get(pos);
		}

		explicit operator bool() const noexcept
		{
			return !!Dot(array.Get(pos));
		}

		bool operator !() const noexcept
		{
			return !Dot(array.Get(pos));
		}

		bool operator == (Dot d) const noexcept
		{
			return (Dot)(*this) == d;
		}

		bool operator != (Dot d) const noexcept
		{
			return !((*this) == d);
		}

	};

	Dot operator[](size_type pos) const noexcept
	{
		return Get(pos);
	}

	DotFixedArraySetter operator[](size_type pos) noexcept
	{
		return DotFixedArraySetter(*this, pos);
	}


	DotFixedArray() noexcept
	{

	}

	DotFixedArray(block_type block)
	{
		buffer = block & (Pow2(bitSize) - 1);
	}

	template <typename T>
	DotFixedArray(std::initializer_list<T> const& init) noexcept
		: DotFixedArray(std::begin(init), std::end(init))
	{
	}

	template <typename Container>
	DotFixedArray(const Container& cont) noexcept
		: DotFixedArray(std::begin(cont), std::end(cont))
	{
	}

	DotFixedArray(Dot d) noexcept
	{
		Set(0, d);
	}

	template <typename BlockInputIterator>
	DotFixedArray(BlockInputIterator first, BlockInputIterator last) noexcept
	{
		size_type i = 0;
		for (auto it = first; it != last; ++it)
		{
			Set(i++, *it);
		}
	}

	DotFixedArray(DotFixedArray&&) = default;
	DotFixedArray& operator = (DotFixedArray&&) = default;

	DotFixedArray(DotFixedArray const&) = default;
	DotFixedArray& operator = (DotFixedArray const&) = default;

	bool empty() const noexcept
	{
		return false;
	}

	size_type size() const noexcept
	{
		return bitSize;
	}

	block_type ToBits() const
	{
		return buffer;
	}

	static DotFixedArray<bitSize, _c> FromBits(block_type b) 
	{
		return DotFixedArray<bitSize, _c>(b);
	}

	std::vector<Dot> ToVector_ForTest() const noexcept
	{
		std::vector<Dot> res(size());

		for (uint64_t i = 0; i < size(); i++)
		{
			res[i] = (*this)[i];
		}

		return res;
	}

	friend bool operator == (DotFixedArray<bitSize, _c> w1, DotFixedArray<bitSize, _c> w2)
	{
		return w1.buffer == w2.buffer;
	};

	friend bool operator != (DotFixedArray<bitSize, _c> w1, DotFixedArray<bitSize, _c> w2)
	{
		return !(w1 == w2);
	};

	friend bool operator < (DotFixedArray<bitSize, _c> w1, DotFixedArray<bitSize, _c> w2)
	{
		return w1.buffer < w2.buffer;
	};

private:
	friend class DotArray;

	void Set(const size_type pos, const Dot d) noexcept
	{
		assert(pos < bitSize);

		const size_type bitIndex = pos & Traits::b111;

		const block_type mask = (Traits::x1 << bitIndex) ^ Traits::xFF;
		const block_type newBitVal = (d.value & Traits::x1) << bitIndex;

		block_type bRes = buffer;

		bRes &= mask;
		bRes |= newBitVal;

		buffer = bRes;
	}

	Dot Get(const size_type pos) const noexcept
	{
		assert(pos < bitSize);

		Dot result;

		const size_type bitIndex = pos & Traits::b111;

		result.value = (buffer >> bitIndex) & Traits::x1;

		return result;
	}

	block_type buffer = 0;
};



