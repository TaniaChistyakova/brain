/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#pragma once


#include "Dot.h"
#include "BMath.h"
#include "Letter.h"
#include "DotFixedArray.h"

#include <iterator>
#include <random>
#include <vector>
#include <cassert>



// DotArray is a finite set of 0 and 1's, *{0,1}

// this class is made interface compatible with standard container
// that's why it uses different name convention

class DotArray : public BitTraits<uint64_t>
{
public:

	class DotArraySetter
	{
		DotArray& array;
		const uint64_t pos;
	public:
		DotArraySetter(DotArray& inArray, const uint64_t inPos) noexcept
			: array(inArray)
			, pos(inPos)
		{
		}

		DotArraySetter& operator = (DotArraySetter const& d) noexcept
		{
			return *this = (Dot)d;
		}

		DotArraySetter& operator ^= (DotArraySetter const& d) noexcept
		{
			return *this ^= (Dot)d;
		}

		DotArraySetter& operator &= (DotArraySetter const& d) noexcept
		{
			return *this &= (Dot)d;
		}

		DotArraySetter& operator = (Dot d) noexcept
		{
			array.Set(pos, d);
			return *this;
		}

		DotArraySetter& operator ^= (Dot d) noexcept
		{
			Dot myD = array.Get(pos);
			myD ^= d;
			array.Set(pos, myD);
			return *this;
		}

		DotArraySetter& operator &= (Dot d) noexcept
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

		explicit operator bool () const noexcept
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


	typedef DataType block_type;
	typedef size_t size_type;

	Dot operator[](size_type pos) const noexcept
	{
		return Get(pos);
	}

	DotArraySetter operator[](size_type pos) noexcept
	{
		return DotArraySetter(*this, pos);
	}


	DotArray() noexcept
	{

	}

	DotArray(DotArray&& old) noexcept
		: buffer(std::move(old.buffer))
		, bitSize(old.bitSize)
	{
		old.bitSize = 0;
	}

	DotArray(const DotArray&) = default;
	DotArray& operator = (const DotArray&) = default;

	DotArray(const DotArray& init, size_t sz)
	{
		MakeDirtyBuffer(sz);
		init.CopyTo(*this, sz);
	}

	DotArray(size_type newsize) noexcept
	{
		resize(newsize);
	}

	DotArray(int newsize) noexcept
	{
		resize(newsize);
	}

	template <typename T>
	DotArray(std::initializer_list<T> const& init) noexcept
		: DotArray(std::begin(init), std::end(init))
	{
	}

	template <typename Container>
	DotArray(const Container& cont) noexcept
		: DotArray(std::begin(cont), std::end(cont))
	{
	}

	DotArray(Dot d) noexcept
	{
		resize(1);
		Set(0, d);
	}

	DotArray(Dot d, size_type newSize)
	{
		if (d)
		{
			MakeDirtyBuffer(newSize);
			set();
		}
		else
		{
			resize(newSize);
		}
	}

	template <typename BlockInputIterator>
	DotArray(BlockInputIterator first, BlockInputIterator last) noexcept
	{
		MakeDirtyBuffer(std::distance(first, last));
		size_type i = 0;
		for (auto it = first ; it != last; ++it)
		{
			Set(i++, *it);
		}
		ClearTail();
	}

	bool empty() const noexcept
	{
		return size() == 0;
	}

	size_type size() const noexcept
	{
		return bitSize;
	}

	bool Empty() const noexcept
	{
		return empty();
	}

	size_type Size() const noexcept
	{
		return size();
	}

	void resize(size_type newsize) noexcept
	{
		const size_t oldBufSize = buffer.size();
		MakeDirtyBuffer(newsize);
		if (oldBufSize < buffer.size())
		{
			std::fill(buffer.begin() + oldBufSize, buffer.end(), 0);
		}
		else 
		{
			ClearTail();
		}
	}

	bool any() const noexcept
	{
		assert(!empty());

		for (size_type i = 0; i < buffer.size(); ++i)
		{
			block_type byte = buffer[i];
			if (!!byte)
			{
				return true;
			}
		}
		return false;
	}

	bool none() const noexcept
	{
		return !any();
	}

	void reset() noexcept
	{
		assert(!empty());

		std::fill(buffer.begin(), buffer.end(), 0);
	}

	void flip() noexcept
	{
		assert(!empty());

		for (size_type i = 0; i < buffer.size(); ++i)
		{
			buffer[i] ^= xFF;
		}
		ClearTail();
	}

	void set() noexcept
	{
		assert(!empty());

		std::fill(buffer.begin(), buffer.end(), xFF);
		ClearTail();
	}

	void clear() noexcept
	{
		buffer.clear();
		bitSize = 0;
	}

	bool operator == (DotArray const& d) const noexcept
	{
		assert(!empty());
		assert(size() == d.size());

		return buffer == d.buffer;
	}

	bool operator != (DotArray const& d) const noexcept
	{
		return !(*this == d);
	}

	DotArray& operator ^= (DotArray const& da) noexcept
	{
		if (da.size() < size())
		{
			resize(da.size());
		}

		for (size_type i = 0; i < buffer.size(); ++i)
		{
			buffer[i] ^= da.buffer[i];
		}
		ClearTail();
		return *this;
	}

	DotArray& operator &= (DotArray const& da) noexcept
	{
		if (da.size() < size())
		{
			resize(da.size());
		}

		for (size_type i = 0; i < buffer.size(); ++i)
		{
			buffer[i] &= da.buffer[i];
		}
		return *this;
	}

	template<typename T = uint64_t,
		std::enable_if_t<std::is_integral<T>::value && (std::numeric_limits<T>::digits <= std::numeric_limits<block_type>::digits), bool> = true>
	auto ToBits() const noexcept
	{
		assert(size() <= std::numeric_limits<T>::digits);

		return (T)buffer[0];
	}

	template<typename T = uint64_t,
		std::enable_if_t<std::is_integral<T>::value && (std::numeric_limits<T>::digits > std::numeric_limits<block_type>::digits), bool> = true>
	auto ToBits() const noexcept
	{
		assert(size() <= std::numeric_limits<T>::digits);

		T bitfield = 0;
		size_type byteSize = buffer.size();
		for (size_t i = 0; i < byteSize; i++)
		{
			bitfield <<= x8;
			bitfield |= buffer[byteSize - i - 1];
		}
		return bitfield;
	}


	template <uint8_t _bitSize, typename _c >
	auto ToBits() const noexcept
	{
		assert(size() <= _bitSize);
		DotFixedArray<_bitSize, _c> bitfield;
		bitfield.buffer = ToBits<_c>();
		return bitfield;
	}

	template<class _Word,
		std::enable_if_t<std::is_class<_Word>::value, bool> = true>
	auto ToBits() const noexcept
	{
		return ToBits<_Word::bitSize, typename _Word::block_type>();
	}

	template<typename T = uint64_t,
		std::enable_if_t<std::is_integral<T>::value && (std::numeric_limits<T>::digits <= std::numeric_limits<block_type>::digits), bool> = true>
	auto FromBits(T data, size_t sz) noexcept
	{
		assert(size() <= std::numeric_limits<T>::digits);

		resize(sz);

		buffer[0] = (block_type)data;
	}

	template<typename T = uint64_t,
		std::enable_if_t<std::is_integral<T>::value && (std::numeric_limits<T>::digits > std::numeric_limits<block_type>::digits), bool> = true>
		void FromBits(T data, size_t sz) noexcept
	{
		assert(size() <= std::numeric_limits<T>::digits);

		resize(sz);

		data &= (Pow2(sz) - 1);

		for (size_t i = 0; i < buffer.size(); i++)
		{
			buffer[i] = (block_type)data;
			data >>= x8;
		}
	}

	template <class _Arr>
	void Append(_Arr const& d)
	{
		size_type origSize = size();
		resize(origSize + d.size());
		for (size_t i = 0; i < d.size(); i++)
		{
			(*this)[i + origSize] = d[i];
		}
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


	/* //per bit implementation
	
	    DotArray newCoefficients(coefficients.size());

        for (uint64_t i = 0; i < coefficients.size(); i++)
        {
            for (uint64_t j = 0; j < coefficients.size(); j++)
            {
                uint64_t newIndex = i | j;
                newCoefficients[newIndex] ^= coefficients[i] & f.coefficients[j];
            }
        }

        coefficients = newCoefficients;
	*/
	void FunctionalAnd(DotArray const& a, DotArray& res) const noexcept
	{
		assert(!empty());
		assert(!a.empty());

		const size_t bufSz = std::min(buffer.size(), a.buffer.size());
		const size_type newBitTail = std::min(bitSize, a.bitSize) & b111;

		std::vector<block_type>& newBuffer = res.buffer;

		for (size_t i = 0; i < bufSz; i++)
		{
			block_type b1 = buffer[i];

			if (!b1)
			{
				continue;
			}

			if (i == bufSz - 1)
			{
				if (newBitTail != 0)
				{
					b1 &= (x1 << (newBitTail)) - x1;
				}
			}

			for(size_t j = 0; j < bufSz; ++j)
			{
				block_type b2 = a.buffer[j];

				if (!b2)
				{
					continue;
				}

				if (j == bufSz - 1)
				{
					if (newBitTail != 0)
					{
						b2 &= (x1 << (newBitTail)) - x1;
					}
				}

				const size_t newByteIndex = i | j;
				block_type bRes = newBuffer[newByteIndex];

				for (size_t i2 = 0; i2 < x8; i2++)
				{
					for (size_t j2 = 0; j2 < x8; j2++)
					{
						const size_t newBitIndex = i2 | j2;
						const block_type newBit = ((b1 >> i2) & (b2 >> j2)) & x1;
						const block_type oldBit = (bRes >> newBitIndex) & x1;
						const block_type mask = (x1 << newBitIndex) ^ xFF;
						bRes &= mask;
						bRes |= (newBit ^ oldBit) << newBitIndex;
					}
				}

				newBuffer[newByteIndex] = bRes;
			}
		}
	}


	friend DotArray operator ^ (DotArray const& f1, DotArray const& f2) noexcept
	{
		if (f1.size() <= f2.size())
		{
			DotArray f = f1;
			f ^= f2;
			return f;
		}
		else
		{
			DotArray f = f2;
			f ^= f1;
			return f;
		}
	}

	friend DotArray operator & (DotArray const& f1, DotArray const& f2) noexcept
	{
		if (f1.size() <= f2.size())
		{
			DotArray f = f1;
			f &= f2;
			return f;
		}
		else
		{
			DotArray f = f2;
			f &= f1;
			return f;
		}
	}

	static DotArray LongOr (DotArray const& f1, DotArray const& f2) noexcept
	{
		if (f1.size() <= f2.size())
		{
			DotArray f = f2;
			for (size_t i = 0; i < f1.buffer.size(); i++)
			{
				f.buffer[i] |= f1.buffer[i];
			}
			return f;
		}
		else
		{
			DotArray f = f1;
			for (size_t i = 0; i < f2.buffer.size(); i++)
			{
				f.buffer[i] |= f2.buffer[i];
			}
			return f;
		}
	}

	bool Inc()
	{
		if (bitSize == 0)
		{
			return false;
		}

		for (size_t i = 0; i < buffer.size() - 1; i++)
		{
			if (buffer[i] == xFF)
			{
				buffer[i] = 0;
			}
			else
			{
				++buffer[i];
				return true;
			}
		}

		block_type val = buffer.back();
		++val;
		const block_type bitMask = (block_type)Pow2(bitSize & b111) - x1;

		if (val > bitMask)
		{
			return false;
		}

		buffer.back() = val;
		return true;
	}

	template< class Generator >
	static DotArray GenerateRandom(size_type newsize, Generator& g)
	{
		DotArray ar;
		std::uniform_int_distribution<uint64_t>  distr(0, xFF);
		ar.MakeDirtyBuffer(newsize);
		for (size_type i = 0; i < ar.buffer.size(); ++i)
		{
			ar.buffer[i] = (block_type)distr(g);
		}
		ar.ClearTail();
		return ar;
	}

	size_t onesNum() const noexcept
	{
		size_type n = 0;

		for (size_type i = 0; i < buffer.size(); ++i)
		{
			n += OnesInNumber(buffer[i]);
		}

		return n;
	}

	size_t zeroesNum() const noexcept
	{
		return size() - onesNum();
	}

protected:

	void ClearTail() noexcept
	{
		const size_type newBitTail = bitSize & b111;
		if (newBitTail != 0)
		{
			buffer.back() &= (x1 << (newBitTail)) - x1;
		}
	}

	void MakeDirtyBuffer(size_type newsize) noexcept
	{
		size_type bufSize = (newsize >> x3);
		const size_type newBitTail = newsize & b111;
		const size_type oldSize = bitSize;
		if (newBitTail != 0)
		{
			bufSize += 1;
		}

		buffer.resize(bufSize);

		bitSize = newsize;
	}

public:

	size_t CopyTo(DotArray& dst, size_t lenght) const
	{
		size_t minBitSize = std::min(bitSize, dst.bitSize);
		minBitSize = std::min(minBitSize, lenght);
		if (minBitSize == 0)
		{
			return 0;
		}

		size_t szBuf = minBitSize >> x3;

		for (size_t i = 0; i < szBuf; i++)
		{
			dst.buffer[i] = buffer[i];
		}

		block_type copyMask = (block_type)Pow2(minBitSize & b111) - x1;
		block_type srcBits = buffer[szBuf] & copyMask;
		block_type dstBits = dst.buffer[szBuf] & (xFF ^ copyMask);
		dst.buffer[szBuf] = srcBits | dstBits;
		return minBitSize;
	}

	void Set(const size_type pos, const Dot d) noexcept
	{
		assert(pos < bitSize);

		const size_type byteIndex = pos >> x3;
		const size_type bitIndex = pos & b111;

		const block_type mask = (x1 << bitIndex) ^ xFF;
		const block_type newBitVal = (d.value & x1) << bitIndex;

		block_type bRes = buffer[byteIndex];

		bRes &= mask;
		bRes |= newBitVal;

		buffer[byteIndex] = bRes;
	}

	Dot Get(const size_type pos) const noexcept
	{
		assert(pos < bitSize);

		Dot result;

		const size_type byteIndex = pos >> x3;
		const size_type bitIndex = pos & b111;

		result.value = (buffer[byteIndex] >> bitIndex) & x1;

		return result;
	}

	static_assert(sizeof(block_type) >= sizeof(Letter));

	void SetLetter(const size_type pos, const Letter d) noexcept
	{
		assert(pos < GetSizeInLetters());

		if constexpr (sizeof(block_type) > sizeof(Letter))
		{
			static constexpr size_t letter_x3 = (x3 - BitTraits<Letter>::x3);
			static constexpr size_t letter_b111 = (0x1LL << (letter_x3)) - 0x1LL;

			const size_type byteIndex = pos >> (x3 - BitTraits<Letter>::x3);
			const size_type bitIndex = pos & letter_b111;

			Letter* pOurLetter = (Letter*)(&buffer[byteIndex]) + bitIndex;
			*pOurLetter = d;

		}
		else
		{
			buffer[pos] = d;
		}
		ClearTail();
	}

	Letter GetLetter(const size_type pos) const noexcept
	{
		assert(pos < GetSizeInLetters());

		if constexpr (sizeof(block_type) > sizeof(Letter))
		{
			static constexpr size_t letter_x3 = (x3 - BitTraits<Letter>::x3);
			static constexpr size_t letter_b111 = (0x1LL << (letter_x3)) - 0x1LL;

			const size_type byteIndex = pos >> (x3 - BitTraits<Letter>::x3);
			const size_type bitIndex = pos & letter_b111;

			const Letter* pOurLetter = (Letter*)(&buffer[byteIndex]) + bitIndex;
			return *pOurLetter;

		}
		else
		{
			return buffer[pos];
		}
	}

	size_type GetSizeInLetters() const
	{
		return (bitSize >> BitTraits<Letter>::x3) + (bitSize & BitTraits<Letter>::b111 ? 1 : 0);
	}

	private:

	std::vector<block_type> buffer;
	size_type bitSize = 0;
};


class DotArrayView
{
public:
	friend class ConstDotArrayView;

	typedef typename DotArray::size_type size_type;

	DotArrayView(DotArray& inArray, size_t inStartPos) noexcept
		: array(inArray), startPos(inStartPos)
	{
		assert(startPos < array.size());
		bitSize = array.size() - startPos;
	}

	DotArrayView(DotArray& inArray, size_t inStartPos, size_t inBitSize) noexcept
		: array(inArray), startPos(inStartPos), bitSize(inBitSize)
	{
		assert(startPos + inBitSize <= array.size());
	}

	Dot operator[](size_type pos) const noexcept
	{
		return array.Get(pos + startPos);
	}

	DotArray::DotArraySetter operator[](size_type pos) noexcept
	{
		return DotArray::DotArraySetter(array, pos + startPos);
	}

	size_type size() const noexcept
	{
		return bitSize;
	}

	uint64_t ToBits() const noexcept
	{
		assert(size() < std::numeric_limits<uint64_t>::digits);
		uint64_t bitfield = 0;
		for (uint64_t i = 0; i < size(); i++)
		{
			if ((*this)[i])
			{
				bitfield |= 1LL << i;
			}
		}
		return bitfield;
	}

private:

	DotArray& array;
	size_t startPos;
	size_t bitSize;
};


class ConstDotArrayView
{
public:
	typedef typename DotArray::size_type size_type;

	ConstDotArrayView(const DotArrayView& view)
		: array(view.array)
		, startPos(view.startPos)
		, bitSize(view.bitSize)
	{
	}

	ConstDotArrayView(const DotArray& inArray, size_t inStartPos) noexcept
		: array(inArray), startPos(inStartPos)
	{
		assert(startPos < array.size());
		bitSize = array.size() - startPos;
	}

	ConstDotArrayView(const DotArray& inArray, size_t inStartPos, size_t inBitSize) noexcept
		: array(inArray), startPos(inStartPos), bitSize(inBitSize)
	{
		assert(startPos + inBitSize <= array.size());
	}

	Dot operator[](size_type pos) const noexcept
	{
		return array.Get(pos + startPos);
	}

	size_type size() const noexcept
	{
		return bitSize;
	}

	uint64_t ToBits() const noexcept
	{
		assert(size() < std::numeric_limits<uint64_t>::digits);
		uint64_t bitfield = 0;
		for (uint64_t i = 0; i < size(); i++)
		{
			if ((*this)[i])
			{
				bitfield |= 1LL << i;
			}
		}
		return bitfield;
	}

private:

	const DotArray& array;
	size_t startPos;
	size_t bitSize;
};


template<class DotsView1, class DotsView2> inline
size_t Copy(DotsView1& dst, const DotsView2& src, size_t lenght)
{
	size_t sz = std::min(dst.size(), src.size());
	sz = std::min(sz, lenght);
	for (size_t i = 0; i < sz; i++)
	{
		dst[i] = src[i];
	}

	return sz;
}

template<> inline
size_t Copy<>(DotArray& dst, const DotArray& src, size_t lenght)
{
	return src.CopyTo(dst, lenght);
}

