/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "ContextObject.h"
#include "HashCalculator.h"


Word ContextObject::CalcHash(class HashCalculator& calc) 
{
	calc.Begin();
	AddToHash(calc);
	currentHash = calc.End();
	return GetHash();
}
