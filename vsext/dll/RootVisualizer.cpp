/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "stdafx.h"
#include "RootVisualizer.h"

HRESULT CRootVisualizer::Initialize(
    _In_ DkmVisualizedExpression* pVisualizedExpression,
    _In_ unsigned long long size,
    _In_ unsigned long long bitSize,
    _In_ unsigned long long sizeOf,
    _In_ bool isPointer)
{
    m_pVisualizedExpression = pVisualizedExpression;
    m_size = size;
    m_bitSize = bitSize;
    m_fIsPointer = isPointer;
    m_sizeOf = sizeOf;
    return S_OK;
}

//static 
HRESULT CRootVisualizer::CreateEvaluationResult(_In_ DkmVisualizedExpression* pVisualizedExpression, _Deref_out_ DkmEvaluationResult** ppResultObject)
{
    HRESULT hr = S_OK;
    *ppResultObject = nullptr;

    CComPtr<DkmRootVisualizedExpression> pRootVisualizedExpression = DkmRootVisualizedExpression::TryCast(pVisualizedExpression);
    if (pRootVisualizedExpression == nullptr)
    {
        // This sample doesn't provide child evaluation results, so only root expressions are expected
        return E_NOTIMPL;
    }
    CComPtr<DkmString> pName = pRootVisualizedExpression->Name();
    CComPtr<DkmString> pFullName = pRootVisualizedExpression->FullName();
    CComPtr<DkmString> pType = pRootVisualizedExpression->Type();
    DkmRootVisualizedExpressionFlags_t flags = pRootVisualizedExpression->Flags();

    CString evalText;
    bool isPointer = (pType != nullptr && wcschr(pType->Value(), '*') != nullptr);
    unsigned long long sizeA, sizeB;

    hr = GetValue(
        pVisualizedExpression,
        pFullName,
        isPointer ? L"(%s)->%s.size()" : L"(%s).%s.size()",
        L"buffer",
        &sizeA
    );

    if (FAILED(hr))
    {
        return hr;
    }

    hr = GetValue(
        pVisualizedExpression,
        pFullName,
        isPointer ? L"(%s)->%s" : L"(%s).%s",
        L"bitSize",
        &sizeB
    );

    if (FAILED(hr))
    {
        return hr;
    }

    unsigned long long sizeOfBlock;
    hr = GetValue(
        pVisualizedExpression,
        pFullName,
        isPointer ? L"sizeof((%s)->%s[0])" : L"sizeof((%s).%s[0])",
        L"buffer",
        &sizeOfBlock
    );

    if (FAILED(hr))
    {
        return hr;
    }

    CComObject<CRootVisualizer>* pRootVisualizer;
    if (SUCCEEDED(hr = CComObject<CRootVisualizer>::CreateInstance(&pRootVisualizer)) && pRootVisualizer != nullptr)
    {
        if (SUCCEEDED(hr = pRootVisualizer->Initialize(pVisualizedExpression, sizeA, sizeB, sizeOfBlock, isPointer)) && pVisualizedExpression != nullptr)
        {
            pVisualizedExpression->SetDataItem(DkmDataCreationDisposition::CreateNew, pRootVisualizer);

            hr = pRootVisualizer->CreateEvaluationResult(
                pName,
                pFullName,
                pType,
                flags,
                nullptr,
                pVisualizedExpression->InspectionContext(),
                ppResultObject
            );
        }
    }

    return hr;
}

HRESULT CRootVisualizer::CreateEvaluationResult(
    _In_ DkmString* pName,
    _In_ DkmString* pFullName,
    _In_ DkmString* pType,
    _In_ DkmRootVisualizedExpressionFlags_t flags,
    _In_opt_ DkmVisualizedExpression* pParent,
    _In_ DkmInspectionContext* pInspectionContext,
    _Deref_out_ DkmEvaluationResult** ppResultObject
)
{
    HRESULT hr = S_OK;

    CComPtr<DkmPointerValueHome> pPointerValueHome = DkmPointerValueHome::TryCast(m_pVisualizedExpression->ValueHome());
    if (pPointerValueHome == nullptr)
    {
        // This sample only handles visualizing in-memory Sample structures
        return E_NOTIMPL;
    }

    // Create method for DkmDataAddress takes a runtime instance.
    CComPtr<DkmDataAddress> pAddress;
    hr = DkmDataAddress::Create(m_pVisualizedExpression->RuntimeInstance(), pPointerValueHome->Address(), NULL, &pAddress);
    if (FAILED(hr))
    {
        return hr;
    }

    CString strValue;

    if (m_size > 0)
    {
        CString bitData;
        int idx = 0;
        for (size_t sz = 0; sz < min(m_bitSize, 32); sz += m_sizeOf * 8)
        {
            CString textIdx;
            textIdx.Format(L"%d", idx);
            unsigned long long firstBlock;
            hr = GetValue(
                m_pVisualizedExpression,
                pFullName,
                m_fIsPointer ? L"(%s)->buffer[%s]" : L"(%s).buffer[%s]",
                static_cast<LPCWSTR>(textIdx),
                &firstBlock
            );

            if (FAILED(hr))
            {
                return hr;
            }

            for (size_t i = 0; i < min(m_bitSize - sz, m_sizeOf * 8); i++)
            {
                if (i + sz >= 32) break;                
                
                bitData.AppendChar(firstBlock & 1 ? '1' : '0');
                firstBlock >>= 1;

            }
            ++idx;
        }

        if (m_bitSize >= 32)
        {
            bitData.Append(L"..");
        }

        strValue.Format(L"[%s], Size = %llu", static_cast<LPCWSTR>(bitData), m_bitSize);
    }
    else
    {
        strValue = L"[]";
    }

    CString strEditableValue;

    // If we are formatting a pointer, we want to also show the address of the pointer
    if (m_fIsPointer)
    {
        // Make the editable value just the pointer string
        UINT64 address = pPointerValueHome->Address();
        DkmProcess* pTargetProcess = pInspectionContext->RuntimeInstance()->Process();
        if ((pTargetProcess->SystemInformation()->Flags() & DefaultPort::DkmSystemInformationFlags::Is64Bit) != 0)
        {
            strEditableValue.Format(L"0x%08x%08x", static_cast<DWORD>(address >> 32), static_cast<DWORD>(address));
        }
        else
        {
            strEditableValue.Format(L"0x%08x", static_cast<DWORD>(address));
        }

        // Prefix the value with the address
        CString strValueWithAddress;
        strValueWithAddress.Format(L"%s {%s}", static_cast<LPCWSTR>(strEditableValue), static_cast<LPCWSTR>(strValue));
        strValue = strValueWithAddress;
    }

    CComPtr<DkmString> pValue;
    hr = DkmString::Create(DkmSourceString(strValue), &pValue);
    if (FAILED(hr))
    {
        return hr;
    }

    CComPtr<DkmString> pEditableValue;
    hr = DkmString::Create(strEditableValue, &pEditableValue);
    if (FAILED(hr))
    {
        return hr;
    }

    DkmEvaluationResultFlags_t resultFlags = DkmEvaluationResultFlags::Expandable;

    if (strEditableValue.IsEmpty())
    {
        // We only allow editting pointers, so mark non-pointers as read-only
        resultFlags |= DkmEvaluationResultFlags::ReadOnly;
    }

    CComPtr<DkmSuccessEvaluationResult> pSuccessEvaluationResult;
    hr = DkmSuccessEvaluationResult::Create(
        m_pVisualizedExpression->InspectionContext(),
        m_pVisualizedExpression->StackFrame(),
        pName,
        pFullName,
        resultFlags,
        pValue,
        pValue,
        pType,
        DkmEvaluationResultCategory::Class,
        DkmEvaluationResultAccessType::None,
        DkmEvaluationResultStorageType::None,
        DkmEvaluationResultTypeModifierFlags::None,
        pAddress,
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

HRESULT CRootVisualizer::GetChildren(
    _In_ UINT32 InitialRequestSize,
    _In_ DkmInspectionContext* pInspectionContext,
    _Out_ DkmArray<DkmChildVisualizedExpression*>* pInitialChildren,
    _Deref_out_ DkmEvaluationResultEnumContext** ppEnumContext
)
{
    HRESULT hr = S_OK;
    pInitialChildren->Members = nullptr;
    pInitialChildren->Length = 0;

    CComPtr<DkmEvaluationResultEnumContext> pEnumContext;
    hr = DkmEvaluationResultEnumContext::Create(
        (DWORD) min(m_bitSize, (unsigned long long)UINT_MAX),
        m_pVisualizedExpression->StackFrame(),
        pInspectionContext,
        this,
        &pEnumContext);
    if (FAILED(hr))
    {
        return hr;
    }

    if (InitialRequestSize > 0)
    {
        GetItems(m_pVisualizedExpression, pEnumContext, 0, InitialRequestSize, pInitialChildren);
    }

    *ppEnumContext = pEnumContext.Detach();

    return hr;
}

HRESULT CRootVisualizer::GetItems(
    _In_ DkmVisualizedExpression* pVisualizedExpression,
    _In_ DkmEvaluationResultEnumContext* pEnumContext,
    _In_ UINT32 StartIndex,
    _In_ UINT32 Count,
    _Out_ DkmArray<DkmChildVisualizedExpression*>* pItems
)
{
    HRESULT hr = S_OK;


    CAtlList<CComPtr<DkmChildVisualizedExpression>> childItems;

    for (UINT32 i = StartIndex; i < Count + StartIndex && i < m_bitSize; i++)
    {

        CString Name;
        Name.Format(L"[%u]", i);
        CComPtr<DkmString> pChildName;
        hr = DkmString::Create(Name, &pChildName);
        if (FAILED(hr))
        {
            return hr;
        }

        CComPtr<DkmString> pChildFullName;
        hr = m_pVisualizedExpression->CreateDefaultChildFullName(i, &pChildFullName);
        if (FAILED(hr))
        {
            return hr;
        }

        CComObject<CChildVisualizer>* pChildVisualizer;
        CComObject<CChildVisualizer>::CreateInstance(&pChildVisualizer);
        if (pChildVisualizer == NULL)
        {
            return E_OUTOFMEMORY;
        }

        hr = pChildVisualizer->Initialize(m_pVisualizedExpression, m_bitSize, i, m_sizeOf, m_fIsPointer);
        if (FAILED(hr))
        {
            return hr;
        }

        CComPtr<DkmString> pTypeText;
        hr = DkmString::Create(DkmSourceString(L"unsinged __int64"), &pTypeText);
        if (FAILED(hr))
        {
            return hr;
        }

        CComPtr<DkmEvaluationResult> pEvaluationResult;
        hr = pChildVisualizer->CreateEvaluationResult(
            pChildName,
            pChildFullName,
            pTypeText,
            DkmRootVisualizedExpressionFlags::None,
            m_pVisualizedExpression,
            m_pVisualizedExpression->InspectionContext(),
            i,
            &pEvaluationResult
        );
        if (FAILED(hr))
        {
            return hr;
        }

        CComPtr<DkmChildVisualizedExpression> pChildVisualizedExpression;
        hr = DkmChildVisualizedExpression::Create(
            m_pVisualizedExpression->InspectionContext(),
            m_pVisualizedExpression->VisualizerId(),
            m_pVisualizedExpression->SourceId(),
            m_pVisualizedExpression->StackFrame(),
            nullptr,
            pEvaluationResult,
            m_pVisualizedExpression,
            i,
            pChildVisualizer,
            &pChildVisualizedExpression
        );
        if (FAILED(hr))
        {
            return hr;
        }

        hr = pChildVisualizer->Initialize2(pChildVisualizedExpression);
        if (FAILED(hr))
        {
            return hr;
        }

        childItems.AddTail(pChildVisualizedExpression);
    }

    CAutoDkmArray<DkmChildVisualizedExpression*> resultValues;
    DkmAllocArray(childItems.GetCount(), &resultValues);
    if (FAILED(hr))
    {
        return hr;
    }

    UINT32 j = 0;
    POSITION pos = childItems.GetHeadPosition();
    while (pos != NULL)
    {
        CComPtr<DkmChildVisualizedExpression> pCurr = childItems.GetNext(pos);
        resultValues.Members[j] = pCurr.Detach();
        j++;
    }

    *pItems = resultValues.Detach();

    return hr;
}



HRESULT CRootVisualizer::GetValue(
    _In_ Evaluation::DkmVisualizedExpression* pVisualizedExpression,
    _In_ DkmString* pFullName,
    _In_ LPCWSTR pExpression,
    _In_ LPCWSTR pMemberName,
    _Out_ unsigned long long* pSize
)
{
    HRESULT hr = S_OK;

    CString evalText;
    evalText.Format(pExpression, pFullName->Value(), pMemberName);

    CComPtr<DkmString> pEvalText;
    hr = DkmString::Create(DkmSourceString(evalText), &pEvalText);
    if (FAILED(hr))
    {
        return hr;
    }

    CComPtr<DkmLanguageExpression> pLanguageExpression;
    hr = DkmLanguageExpression::Create(
        pVisualizedExpression->InspectionContext()->Language(),
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

    CComPtr<DkmString> pValue = pSuccessEvalResult->Value();
    if (pValue == nullptr)
    {
        return E_FAIL;
    }

    LPCWSTR sizeStr = pValue->Value();
    LPWSTR endPtr;
    *pSize = wcstoull(sizeStr, &endPtr, 0);
    if (sizeStr == endPtr)
    {
        return E_FAIL;
    }

    return hr;
}