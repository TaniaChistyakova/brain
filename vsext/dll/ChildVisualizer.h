/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */
#pragma once

class ATL_NO_VTABLE __declspec(uuid("EFFE4350-D4BA-45EF-B70D-B74F4FC31972")) CChildVisualizer :
    public IUnknown,
    public CComObjectRootEx<CComMultiThreadModel>
{
private:
    CComPtr<DkmVisualizedExpression> m_pVisualizedExpression;
    CComPtr<DkmChildVisualizedExpression> m_pThisVisualizedExpression;
    CComPtr<DkmChildVisualizedExpression> m_pDataVisualizedExpression;
    unsigned long long m_vectorSize;
    unsigned long long m_parentIndex;
    unsigned long long x3;
    bool m_fRootIsPointer;

    CComPtr<DkmDataAddress> m_address;
    CComPtr<DkmSuccessEvaluationResult> m_pDataEvaluationResult; 
public:
    CChildVisualizer()
    {
        m_vectorSize = 0;
        m_parentIndex = 0;
        m_fRootIsPointer = false;
        x3 = 0;
    }
    ~CChildVisualizer()
    {
    }

    DECLARE_NO_REGISTRY();
    DECLARE_NOT_AGGREGATABLE(CChildVisualizer);

    HRESULT STDMETHODCALLTYPE Initialize(
        _In_ DkmVisualizedExpression* pVisualizedExpression,
        _In_ unsigned long long vectorSize,
        _In_ unsigned long long parentIndex,
        _In_ unsigned long long sizeOf,
        _In_ bool rootIsPointer
    );
    HRESULT STDMETHODCALLTYPE Initialize2(
        _In_ DkmChildVisualizedExpression* pChildVisualizedExpression
    );

    HRESULT STDMETHODCALLTYPE CreateEvaluationResult(
        _In_ DkmString* pName,
        _In_ DkmString* pFullName,
        _In_opt_ DkmString* pType,
        _In_ Evaluation::DkmRootVisualizedExpressionFlags_t flags,
        _In_opt_ Evaluation::DkmVisualizedExpression* pParent,
        _In_ Evaluation::DkmInspectionContext* pInspectionContext,
        _In_ unsigned long long index,
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
    HRESULT STDMETHODCALLTYPE SetValueAsString(
        _In_ Evaluation::DkmChildVisualizedExpression* pVisualizedExpression,
        _In_ DkmString* pValue,
        _In_ UINT32 Timeout,
        _Deref_out_opt_ DkmString** ppErrorText
    );


    DECLSPEC_NOTHROW CComPtr<DkmDataAddress> STDMETHODCALLTYPE Address()
    {
        return m_address;
    }

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
