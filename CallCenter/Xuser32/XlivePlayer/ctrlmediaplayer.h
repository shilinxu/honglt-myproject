//
// CtrlMediaPlayer.h
//

#if !defined(AFX_CTRLMEDIAPLAYER_H__C7FD016F_7EEA_4D8F_80F3_73BB808AA1C3__INCLUDED_)
#define AFX_CTRLMEDIAPLAYER_H__C7FD016F_7EEA_4D8F_80F3_73BB808AA1C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RenderGraph.h"

class CMediaPlayer : public CGraphRender
{
// Construction
public:
	CMediaPlayer();
	virtual ~CMediaPlayer();
	
// Attributes
public:

// Operations
protected:
	virtual HRESULT	RenderUrl(LPCWSTR lpszUrl, IBaseFilter** ppCapture = NULL);
	virtual HRESULT BranchStream(LPCWSTR lpszName, IBaseFilter** ppFilter);
protected:
	virtual HRESULT RenderStream(IBaseFilter* pFilter);
	virtual HRESULT ResizeConfig(IBaseFilter* pCapture);

	friend class CPlayerWnd;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLMEDIAPLAYER_H__C7FD016F_7EEA_4D8F_80F3_73BB808AA1C3__INCLUDED_)
