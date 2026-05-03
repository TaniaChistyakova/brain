/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */
#include "stdafx.h"
#include "ChildVisualizer.h"
#include "RootVisualizer.h"

HRESULT CChildVisualizer::Initialize(
    _In_ DkmVisualizedExpression* pVisualizedExpression,
    _In_ unsigned long long vectorSize,
    _In_ unsigned long long parentIndex,
    _In_ unsigned long long sizeOf,
    _In_ bool rootIsPointer
)
{
    HRESULT hr = S_OK;

    m_pVisualizedExpression = pVisualizedExpression;
    m_vectorSize = vectorSize;
    m_parentIndex = parentIndex;
    m_fRootIsPointer = rootIsPointer;
    x3 = 3 + Log2(sizeOf);

    CComPtr<DkmRootVisualizedExpression> pRootVisualizedExpression = DkmRootVisualizedExpression::TryCast(m_pVisualizedExpression);

    if (pRootVisualizedExpression == nullptr)
    {
        return E_FAIL;
    }

    const unsigned long long blockId = m_parentIndex >> x3;

    CString evalText;
    evalText.Format(m_fRootIsPointer ? L"&((%s)->buffer[%ull])" : L"&((%s).buffer[%ull])", pRootVisualizedExpression->FullName()->Value(), blockId);

    CComPtr<DkmString> pEvalText;
    hr = DkmString::Create(DkmSourceString(evalText), &pEvalText);
    if (FAILED(hr))
    {
        return hr;
    }

    CComPtr<DkmLanguageExpression> pLanguageExpression;
    hr = DkmLanguageExpression::Create(
        pVisualizedExpression->InspectionContext()->Language(),
        DkmEvaluationFlags::TreatFunctionAsAddress,
        pEvalText,
        DkmDataItem::Null(),
        &pLanguageExpression
    );
    if (FAILED(hr))
    {
        return hr;
    }
    CComPtr<DkmEvaluationResult> pEvalResult;
    hr = pVisualizedExpression->EvaluateExpressionCallback(
        pVisualizedExpression->InspectionContext(),
        pLanguageExpression,
        pVisualizedExpression->StackFrame(),
        &pEvalResult
    );
    if (FAILED(hr))
    {
        return hr;
    }

    CComPtr<DkmSuccessEvaluationResult> pSuccessEvalResult = DkmSuccessEvaluationResult::TryCast(pEvalResult);
    if (pSuccessEvalResult == nullptr)
    {
        return E_FAIL;
    }

    m_address = pSuccessEvalResult->Address();

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CChildVisualizer::Initialize2(
    _In_ DkmChildVisualizedExpression* pChildVisualizedExpression
)
{
    HRESULT hr = S_OK;
    m_pThisVisualizedExpression = pChildVisualizedExpression;
    CComPtr<DkmRootVisualizedExpression> pRootVisualizedExpression = DkmRootVisualizedExpression::TryCast(m_pVisualizedExpression);

    if (pRootVisualizedExpression == nullptr)
    {
        return E_FAIL;
    }

    const unsigned long long blockId = m_parentIndex >> x3;

    CString evalText;
    evalText.Format(m_fRootIsPointer ? L"((%s)->buffer[%ull])" : L"((%s).buffer[%ull])", pRootVisualizedExpression->FullName()->Value(), blockId);

    CComPtr<DkmString> pEvalText;
    hr = DkmString::Create(DkmSourceString(evalText), &pEvalText);
    if (FAILED(hr))
    {
        return hr;
    }

    CComPtr<DkmLanguageExpression> pLanguageExpression;
    hr = DkmLanguageExpression::Create(
        pRootVisualizedExpression->InspectionContext()->Language(),
        DkmEvaluationFlags::TreatAsExpression,
        pEvalText,
        DkmDataItem::Null(),
        &pLanguageExpression
    );
    if (FAILED(hr))
    {
        return hr;
    }
    CComPtr<DkmEvaluationResult> pEvalResult;
    hr = pRootVisualizedExpression->EvaluateExpressionCallback(
            pRootVisualizedExpression->InspectionContext(),
            pLanguageExpression,
            pRootVisualizedExpression->StackFrame(),
            &pEvalResult
    );
    if (FAILED(hr))
    {
        return hr;
    }

    CComPtr<DkmSuccessEvaluationResult> pSuccessEvalResult = DkmSuccessEvaluationResult::TryCast(pEvalResult);
    if (pSuccessEvalResult == nullptr)
    {
        return E_FAIL;
    }

    m_pDataEvaluationResult = pSuccessEvalResult;

    CComPtr<DkmString> pValue = pSuccessEvalResult->Value();
    if (pValue == nullptr)
    {
        return E_FAIL;
    }

    CComPtr<DkmPointerValueHome> pPointerValueHome;

    hr = DkmPointerValueHome::Create(Address()->Value(), &pPointerValueHome);
    if (FAILED(hr))
    {
        return hr;
    }

    hr = (DkmChildVisualizedExpression::Create(
        m_pThisVisualizedExpression->InspectionContext(),
        m_pThisVisualizedExpression->VisualizerId(),
        m_pThisVisualizedExpression->SourceId(),
        m_pThisVisualizedExpression->StackFrame(),
        pPointerValueHome,
        pSuccessEvalResult,
        m_pThisVisualizedExpression,
        0,
        DkmDataItem::Null(),
        &m_pDataVisualizedExpression
    ));
    if (FAILED(hr))
    {
        return hr;
    }

    return hr;
}

HRESULT CChildVisualizer::CreateEvaluationResult(
    _In_ DkmString* pName,
    _In_ DkmString* pFullName,
    _In_opt_ DkmString* pType,
    _In_ DkmRootVisualizedExpressionFlags_t flags,
    _In_opt_ DkmVisualizedExpression* pParent,
    _In_ DkmInspectionContext* pInspectionContext,
    _In_ unsigned long long index,
    _Deref_out_ DkmEvaluationResult** ppResultObject
)
{
    HRESULT hr = S_OK;

    unsigned long long block;
    const unsigned long long blockId = index >> x3;
    const unsigned long long bitId = index & ((0x1ull << (x3)) - 0x1ull);

    CString dataArray;
    dataArray.Format(L"buffer[%llu]", blockId);

    CComPtr<DkmRootVisualizedExpression> pRootVisualizedExpression = DkmRootVisualizedExpression::TryCast(m_pVisualizedExpression);

    if (pRootVisualizedExpression == nullptr)
    {
        return E_FAIL;
    }

    hr = CRootVisualizer::GetValue(
        m_pVisualizedExpression,
        pRootVisualizedExpression->FullName(),
        m_fRootIsPointer ? L"(%s)->%s" : L"(%s).%s",
        static_cast<LPCWSTR>(dataArray),
        &block
    );

    if (FAILED(hr))
    {
        return hr;
    }


    unsigned bit = (block >> bitId) & 0x1;


    CString strValue;
    strValue.Format(L"%u", bit);

    CComPtr<DkmString> pValue;
    hr = DkmString::Create(DkmSourceString(strValue), &pValue);
    if (FAILED(hr))
    {
        return hr;
    }

    CComPtr<DkmSuccessEvaluationResult> pSuccessEvaluationResult;
    hr = DkmSuccessEvaluationResult::Create(
        m_pVisualizedExpression->InspectionContext(),
        m_pVisualizedExpression->StackFrame(),
        pName,
        pFullName,
        DkmEvaluationResultFlags::None, //ReadOnly,
        pValue,
        pValue,
        pType,
        DkmEvaluationResultCategory::Data,
        DkmEvaluationResultAccessType::None,
        DkmEvaluationResultStorageType::None,
        DkmEvaluationResultTypeModifierFlags::None,
        nullptr,
        nullptr,
        (DkmReadOnlyCollection<DkmModuleInstance*>*)nullptr,
        DkmDataItem::Null(),
        &pSuccessEvaluationResult
    );
    if (FAILED(hr))
    {
        return hr;
    }

    *ppResultObject = (DkmEvaluationResult*)pSuccessEvaluationResult.Detach();

    return hr;
}

HRESULT STDMETHODCALLTYPE CChildVisualizer::SetValueAsString(
    _In_ Evaluation::DkmChildVisualizedExpression* pVisualizedExpression,
    _In_ DkmString* pValue,
    _In_ UINT32 Timeout,
    _Deref_out_opt_ DkmString** ppErrorText
)
{
    HRESULT hr = S_OK;

    *ppErrorText = nullptr;

    LPCWSTR sizeStr = pValue->Value();
    LPWSTR endPtr;
    unsigned long long Value = wcstoull(sizeStr, &endPtr, 0);
    if (sizeStr == endPtr)
    {
        CComPtr<DkmString> pError;
        hr = DkmString::Create(DkmSourceString(L"Can't get numerical value"), &pError);
        if (FAILED(hr))
        {
            return hr;
        }

        *ppErrorText = (DkmString*)pError.Detach();

        return E_FAIL;
    }

    if (Value > 1)
    {
        CComPtr<DkmString> pError;
        hr = DkmString::Create(DkmSourceString(L"Only 0 and 1 accepted"), &pError);
        if (FAILED(hr))
        {
            return hr;
        }

        *ppErrorText = (DkmString*)pError.Detach();

        return E_FAIL;
    }

    unsigned long long block;
    const unsigned long long blockId = m_parentIndex >> x3;
    const unsigned long long x1 = 0x1ull;
    const unsigned long long xFF = ULLONG_MAX;
    const unsigned long long b111 = ((x1 << (x3)) - x1);
    const unsigned long long bitIndex = m_parentIndex & b111;
    const unsigned long long mask = (x1 << bitIndex) ^ xFF;
    const unsigned long long newBitVal = (Value & x1) << bitIndex;

    CString dataArray;
    dataArray.Format(L"buffer[%llu]", blockId);

    CComPtr<DkmRootVisualizedExpression> pRootVisualizedExpression = DkmRootVisualizedExpression::TryCast(m_pVisualizedExpression);

    if (pRootVisualizedExpression == nullptr)
    {
        return E_FAIL;
    }

    hr = CRootVisualizer::GetValue(
        m_pVisualizedExpression,
        pRootVisualizedExpression->FullName(),
        m_fRootIsPointer ? L"(%s)->%s" : L"(%s).%s",
        static_cast<LPCWSTR>(dataArray),
        &block
    );

    if (FAILED(hr))
    {
        return hr;
    }


    block &= mask;
    block |= newBitVal;

    CString StrValue;
    StrValue.Format(L"%d", (int)block);

    CComPtr<DkmString> pEvalText;
    hr = DkmString::Create(DkmSourceString(StrValue), &pEvalText);
    if (FAILED(hr))
    {
        return hr;
    }
    
    hr = m_pDataVisualizedExpression->SetValueAsStringCallback(m_pDataEvaluationResult, pEvalText, Timeout, ppErrorText);
    if (FAILED(hr))
    {
        return hr;
    }

    return hr;
}

HRESULT CChildVisualizer::GetChildren(
    _In_ UINT32 InitialRequestSize,
    _In_ DkmInspectionContext* pInspectionContext,
    _Out_ DkmArray<DkmChildVisualizedExpression*>* pInitialChildren,
    _Deref_out_ DkmEvaluationResultEnumContext** ppEnumContext
)
{
    return E_NOTIMPL;
}

HRESULT CChildVisualizer::GetItems(
    _In_ DkmVisualizedExpression* pVisualizedExpression,
    _In_ DkmEvaluationResultEnumContext* pEnumContext,
    _In_ UINT32 StartIndex,
    _In_ UINT32 Count,
    _Out_ DkmArray<DkmChildVisualizedExpression*>* pItems
)
{
    return E_NOTIMPL;
}

