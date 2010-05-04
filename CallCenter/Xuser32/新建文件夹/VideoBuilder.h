//
// VideoBuilder.h
//

#if !defined(AFX_VIDEOBUILDER_H__A89523CF_9736_4D01_8981_1A0EE6DAD20E__INCLUDED_)
#define AFX_VIDEOBUILDER_H__A89523CF_9736_4D01_8981_1A0EE6DAD20E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VideoCapture.h"

class CVideoBuilder : public CVideoCapture  
{
// Construction
public:
	CVideoBuilder();
	virtual ~CVideoBuilder();

// Attributes
public:

// Operations
public:
	BOOL		BuildGraph();
	BOOL		AudioCapture(LPCTSTR lpszCapture, IBaseFilter* pRenderer);
	BOOL		StreamCapture(LPCTSTR lpszAudio, LPCTSTR lpszVideo);
	BOOL		StreamRender(LPCTSTR lpszAudio, LPCTSTR lpszVideo);
protected:
	BOOL		InfluxAudio(LPCTSTR lpszCapture, IBaseFilter* pMuxer);
	BOOL		InfluxVideo(LPCTSTR lpszCapture, IBaseFilter* pMuxer);
	BOOL		DemuxAudio(LPCTSTR lpszRender, IBaseFilter* pDemuxer);
	BOOL		DemuxVideo(LPCTSTR lpszRender, IBaseFilter* pDemuxer);
};

#endif // !defined(AFX_VIDEOBUILDER_H__A89523CF_9736_4D01_8981_1A0EE6DAD20E__INCLUDED_)
