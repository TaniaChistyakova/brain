/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#pragma once

#include "Word.h"

#include <memory>



class ContextObject: public std::enable_shared_from_this<ContextObject>
{
public:
	typedef std::shared_ptr<ContextObject> Ptr;
	typedef std::shared_ptr<const ContextObject> CPtr;


	enum class Type {Unknown, Projector, Function, Sentence};

	virtual Type GetType() const = 0;

	virtual void AddToHash(class HashCalculator& calc) const = 0;

	Word CalcHash(class HashCalculator& calc);

	Word GetHash() const
	{
		return currentHash;
	}

	template<class ObjVec>
	static std::vector<ContextObject::CPtr> Downcast(const ObjVec& vec);

private:

	Word currentHash = Dot(0);
};


template<class ObjVec>
inline std::vector<ContextObject::CPtr> ContextObject::Downcast(const ObjVec& vec)
{
	std::vector<ContextObject::CPtr> out(std::cbegin(vec), std::cend(vec));
	return out;
}


