/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */
#pragma once

#include "ChildVisualizer.h"

class ATL_NO_VTABLE __declspec(uuid("F0E2C012-E960-40C8-BE04-BB83CF4DAE76")) CRootVisualizer :
    public IUnknown,
    public CComObjectRootEx<CComMultiThreadModel>
{
private:
    CComPtr<DkmVisualizedExpression> m_pVisualizedExpression;
    unsigned long long m_size;
    unsigned long long m_bitSize;
    unsigned long long m_sizeOf;
    bool m_fIsPointer;

public:
    CRootVisualizer()
    {
        m_size = 0;
        m_bitSize = 0;
        m_sizeOf = 0;
        m_fIsPointer = false;
    }
    ~CRootVisualizer()
    {
    }

    DECLARE_NO_REGISTRY();
    DECLARE_NOT_AGGREGATABLE(CRootVisualizer);

    HRESULT STDMETHODCALLTYPE Initialize(
        _In_ DkmVisualizedExpression* pVisualizedExpression,
        _In_ unsigned long long size,
        _In_ unsigned long long bitSize,
        _In_ unsigned long long sizeOf,
        _In_ bool isPointer
    );

    static HRESULT CreateEvaluationResult(
        _In_ Evaluation::DkmVisualizedExpression* pVisualizedExpression,
        _Deref_out_ Evaluation::DkmEvaluationResult** ppResultObject
    );

    HRESULT STDMETHODCALLTYPE CreateEvaluationResult(
        _In_ DkmString* pName,
        _In_ DkmString* pFullName,
        _In_ DkmString* pType,
        _In_ Evaluation::DkmRootVisualizedExpressionFlags_t flags,
        _In_opt_ Evaluation::DkmVisualizedExpression* pParent,
        _In_ Evaluation::DkmInspectionContext* pInspectionContext,
        _Deref_out_ Evaluation::DkmEvaluationResult** ppResultObject
    );
    HRESULT STDMETHODCALLTYPE GetChildren(
        _In_ UINT32 InitialRequestSize,
        _In_ Evaluation::DkmInspectionContext* pInspectionContext,
        _Out_ DkmArray<Evaluation::DkmChildVisualizedExpression*>* pInitialChildren,
        _Deref_out_ Evaluation::DkmEvaluationResultEnumContext** ppEnumContext
    );
    HRESULT STDMETHODCALLTYPE GetItems(
        _In_ Evaluation::DkmVisualizedExpression* pVisualizedExpression,
        _In_ Evaluation::DkmEvaluationResultEnumContext* pEnumContext,
        _In_ UINT32 StartIndex,
        _In_ UINT32 Count,
        _Out_ DkmArray<Evaluation::DkmChildVisualizedExpression*>* pItems
    );

    static HRESULT STDMETHODCALLTYPE GetValue(
        _In_ Evaluation::DkmVisualizedExpression* pVisualizedExpression,
        _In_ DkmString* pFullName,
        _In_ LPCWSTR pExpression,
        _In_ LPCWSTR pMemberName,
        _Out_ unsigned long long* pSize
    );

protected:

    HRESULT STDMETHODCALLTYPE _InternalQueryInterface(REFIID riid, void** ppvObject)
    {
        if (ppvObject == NULL)
            return E_POINTER;

        if (riid == __uuidof(IUnknown))
        {
            *ppvObject = static_cast<IUnknown*>(this);
            AddRef();
            return S_OK;
        }

        *ppvObject = NULL;
        return E_NOINTERFACE;
    }
};