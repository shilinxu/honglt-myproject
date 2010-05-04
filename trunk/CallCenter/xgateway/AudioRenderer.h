//
// AudioRenderer.h
//

#if !defined(AFX_AUDIORENDERER_H__7EBB10DF_CB4A_4C63_BFDA_C56CB1028FB6__INCLUDED_)
#define AFX_AUDIORENDERER_H__7EBB10DF_CB4A_4C63_BFDA_C56CB1028FB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FilterGraph.h"

class CAudioRenderer : public CFilterGraph
{
// Construction
public:
	CAudioRenderer();
	virtual ~CAudioRenderer();
	
// Attributes
public:

// Operations
public:
	virtual HRESULT	Connnect();
	virtual HRESULT	StartPreview();
public:
	virtual HRESULT	DeliverBuffer(BYTE* pByte, DWORD nLength);
};

#endif // !defined(AFX_AUDIORENDERER_H__7EBB10DF_CB4A_4C63_BFDA_C56CB1028FB6__INCLUDED_)
