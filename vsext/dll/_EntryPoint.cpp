/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "stdafx.h"
#include "_EntryPoint.h"

HRESULT STDMETHODCALLTYPE CBinaryVisualizerService::EvaluateVisualizedExpression(
    _In_ Evaluation::DkmVisualizedExpression* pVisualizedExpression,
    _Deref_out_opt_ Evaluation::DkmEvaluationResult** ppResultObject
    )
{
    HRESULT hr;

    hr = CRootVisualizer::CreateEvaluationResult(pVisualizedExpression, ppResultObject);
    if (FAILED(hr))
    {
        return hr;
    }

    hr = (*ppResultObject)->SetDataItem(DkmDataCreationDisposition::CreateNew, pVisualizedExpression);

    return hr;
}

HRESULT STDMETHODCALLTYPE CBinaryVisualizerService::UseDefaultEvaluationBehavior(
    _In_ Evaluation::DkmVisualizedExpression* pVisualizedExpression,
    _Out_ bool* pUseDefaultEvaluationBehavior,
    _Deref_out_opt_ Evaluation::DkmEvaluationResult** ppDefaultEvaluationResult
    )
{
    *pUseDefaultEvaluationBehavior = false;
    *ppDefaultEvaluationResult = NULL;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CBinaryVisualizerService::GetChildren(
    _In_ Evaluation::DkmVisualizedExpression* pVisualizedExpression,
    _In_ UINT32 InitialRequestSize,
    _In_ Evaluation::DkmInspectionContext* pInspectionContext,
    _Out_ DkmArray<Evaluation::DkmChildVisualizedExpression*>* pInitialChildren,
    _Deref_out_ Evaluation::DkmEvaluationResultEnumContext** ppEnumContext
    )
{
    HRESULT hr = S_OK;

    CComPtr<CRootVisualizer> pRootVisualizer;
    hr = pVisualizedExpression->GetDataItem(&pRootVisualizer);

    if (SUCCEEDED(hr))
    {
        hr = pRootVisualizer->GetChildren(InitialRequestSize, pInspectionContext, pInitialChildren, ppEnumContext);
    }

    return hr;
}

HRESULT STDMETHODCALLTYPE CBinaryVisualizerService::GetItems(
    _In_ Evaluation::DkmVisualizedExpression* pVisualizedExpression,
    _In_ Evaluation::DkmEvaluationResultEnumContext* pEnumContext,
    _In_ UINT32 StartIndex,
    _In_ UINT32 Count,
    _Out_ DkmArray<Evaluation::DkmChildVisualizedExpression*>* pItems
    )
{
    HRESULT hr = S_OK;

    CComPtr<CRootVisualizer> pRootVisualizer;
    hr = pVisualizedExpression->GetDataItem(&pRootVisualizer);

    if (SUCCEEDED(hr))
    {
        hr = pRootVisualizer->GetItems(pVisualizedExpression, pEnumContext, StartIndex, Count, pItems);
    }

    return hr;
}

HRESULT STDMETHODCALLTYPE CBinaryVisualizerService::SetValueAsString(
    _In_ Evaluation::DkmVisualizedExpression* pVisualizedExpression,
    _In_ DkmString* pValue,
    _In_ UINT32 Timeout,
    _Deref_out_opt_ DkmString** ppErrorText
    )
{
    HRESULT hr = S_OK;
    CComPtr<DkmChildVisualizedExpression> pChildVisualizedExpression = DkmChildVisualizedExpression::TryCast(pVisualizedExpression);
    if (!pChildVisualizedExpression)
    {
        return E_FAIL;
    }
    CComPtr<CChildVisualizer> pChildVisualizer;
    pChildVisualizedExpression->GetDataItem(&pChildVisualizer);

    if (SUCCEEDED(hr))
    {
        hr = pChildVisualizer->SetValueAsString(pChildVisualizedExpression, pValue, Timeout, ppErrorText);
    }

    return hr;
}

HRESULT STDMETHODCALLTYPE CBinaryVisualizerService::GetUnderlyingString(
    _In_ Evaluation::DkmVisualizedExpression* pVisualizedExpression,
    _Deref_out_opt_ DkmString** ppStringValue
    )
{
    return E_NOTIMPL;
}