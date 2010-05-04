//
// AudioCapture.h
//

#if !defined(AFX_AUDIOCAPTURE_H__7EBB10DF_CB4A_4C63_BFDA_C56CB1028FB6__INCLUDED_)
#define AFX_AUDIOCAPTURE_H__7EBB10DF_CB4A_4C63_BFDA_C56CB1028FB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FilterGraph.h"

class CAudioCapture : public CFilterGraph
{
// Construction
public:
	CAudioCapture();
	virtual ~CAudioCapture();
	
// Attributes
protected:

// Operations
public:
	virtual HRESULT	ConnnectTo(LPCWSTR lpszCapture);
	virtual HRESULT	StartPreview();
};

//extern CAudioCapture AudioCapture;

#endif // !defined(AFX_AUDIOCAPTURE_H__7EBB10DF_CB4A_4C63_BFDA_C56CB1028FB6__INCLUDED_)
