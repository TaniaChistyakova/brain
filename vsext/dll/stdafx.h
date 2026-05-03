/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_FREE_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include "resource.h"
#include "assert.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <atlcoll.h>

#include <vsdebugeng.h>
#include <vsdebugeng.templates.h>

using namespace ATL;
using namespace Microsoft::VisualStudio::Debugger;
using namespace Microsoft::VisualStudio::Debugger::Evaluation;

inline _Ret_range_(0x8000000, 0xffffffff) HRESULT WIN32_ERROR(LONG lError)
{
    HRESULT hr = HRESULT_FROM_WIN32(lError);
    if (SUCCEEDED(hr))
        hr = E_FAIL;
    return hr;
}

inline _Ret_range_(0x8000000, 0xffffffff) HRESULT WIN32_LAST_ERROR()
{
    return WIN32_ERROR(GetLastError());
}

#if defined(_PREFAST_)
    #define VSAnalysisAssume(cond, text) __analysis_assume(cond)
#elif defined(_DEBUG)
    #define VSAnalysisAssume(cond, text)  assert(cond)
#else
    #define VSAnalysisAssume(cond, text)
#endif

// we assume v is a power of two
inline constexpr int64_t Log2(uint64_t v)
{
    int64_t lg = -1;
    while (v)
    {
        v >>= 1;
        ++lg;
    }
    return lg;
}


