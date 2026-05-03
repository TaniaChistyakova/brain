/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "VectorFunction.h"
#include "HashCalculator.h"


void VectorFunction::AddToHash(class HashCalculator& calc) const
{
	for (size_t i = 0; i < dimensions; i++)
	{
		calc.AddArray(coordinates[i].coefficients);
	}
}

ContextObject::Type VectorFunction::GetType() const
{
	return ContextObject::Type::Function;
}

