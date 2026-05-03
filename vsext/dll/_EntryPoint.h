/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#pragma once



#include "BinaryVisualizer.Contract.h"
#include "RootVisualizer.h"

class ATL_NO_VTABLE CBinaryVisualizerService :
    // Inherit from CBinaryVisualizerServiceContract to provide the list of interfaces that
    // this class implements (interface list comes from BinaryVisualizer.vsdconfigxml)
    public CBinaryVisualizerServiceContract,

    // Inherit from CComObjectRootEx to provide ATL support for reference counting and
    // object creation.
	public CComObjectRootEx<CComMultiThreadModel>,

    // Inherit from CComCoClass to provide ATL support for exporting this class from
    // DllGetClassObject
    public CComCoClass<CBinaryVisualizerService, &CBinaryVisualizerServiceContract::ClassId>
{
protected:
    CBinaryVisualizerService()
    {
    }
    ~CBinaryVisualizerService()
    {
    }

public:
    DECLARE_NO_REGISTRY();
    DECLARE_NOT_AGGREGATABLE(CBinaryVisualizerService);

// IDkmCustomVisualizer methods
public:
    HRESULT STDMETHODCALLTYPE EvaluateVisualizedExpression(
        _In_ Evaluation::DkmVisualizedExpression* pVisualizedExpression,
        _Deref_out_opt_ Evaluation::DkmEvaluationResult** ppResultObject
        );
    HRESULT STDMETHODCALLTYPE UseDefaultEvaluationBehavior(
        _In_ Evaluation::DkmVisualizedExpression* pVisualizedExpression,
        _Out_ bool* pUseDefaultEvaluationBehavior,
        _Deref_out_opt_ Evaluation::DkmEvaluationResult** ppDefaultEvaluationResult
        );
    HRESULT STDMETHODCALLTYPE GetChildren(
        _In_ Evaluation::DkmVisualizedExpression* pVisualizedExpression,
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
        _In_ Evaluation::DkmVisualizedExpression* pVisualizedExpression,
        _In_ DkmString* pValue,
        _In_ UINT32 Timeout,
        _Deref_out_opt_ DkmString** ppErrorText
        );
    HRESULT STDMETHODCALLTYPE GetUnderlyingString(
        _In_ Evaluation::DkmVisualizedExpression* pVisualizedExpression,
        _Deref_out_opt_ DkmString** ppStringValue
        );
};

OBJECT_ENTRY_AUTO(CBinaryVisualizerService::ClassId, CBinaryVisualizerService)
