//
// DlgConnect.h
//

#if !defined(AFX_DLGCONNECT_H__B99F0E55_6587_4EF8_88AD_66787BEE3240__INCLUDED_)
#define AFX_DLGCONNECT_H__B99F0E55_6587_4EF8_88AD_66787BEE3240__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlCoolBar.h"
#include "CtrlCoolHeaderBar.h"

class CConnectDlg	: public CDialog
{
// Construction
public:
	CConnectDlg();
	virtual ~CConnectDlg();
	DECLARE_DYNAMIC(CConnectDlg)
	
// Dialog Data
protected:
	//{{AFX_DATA(CConnectDlg)
	enum { IDD = IDD_CONNECT };
	//}}AFX_DATA
	
// Attributes
protected:
	CCoolHeaderBarCtrl	m_wndToolBar;
	NOTIFYICONDATA	m_pTray;
protected:
	BOOL			m_bInTimer;
	
// Operations
protected:
	void		UpdateMessages();
	BOOL		CreateReal(UINT nID);
	void		OpenFromTray(int nShowCmd = SW_SHOW);

// Overrides
public:
	//{{AFX_VIRTUAL(CConnectDlg)
	public:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PostNcDestroy();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CConnectDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnTrayOpen();
	afx_msg void OnNetworkConnect();
	//}}AFX_MSG
	afx_msg LONG OnTray(UINT wParam, LONG lParam);
	afx_msg LONG OnStartPreview(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

#if 0

#ifndef __ITaskbarList_INTERFACE_DEFINED__
#define __ITaskbarList_INTERFACE_DEFINED__

EXTERN_C const IID IID_ITaskbarList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56FDF342-FD6D-11d0-958A-006097C9A090")
    ITaskbarList : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE HrInit( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddTab( 
            /* [in] */ HWND hwnd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DeleteTab( 
            /* [in] */ HWND hwnd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ActivateTab( 
            /* [in] */ HWND hwnd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetActiveAlt( 
            /* [in] */ HWND hwnd) = 0;
        
    };
    
#else     /* C style interface */

    typedef struct ITaskbarListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITaskbarList * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITaskbarList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITaskbarList * This);
        
        HRESULT ( STDMETHODCALLTYPE *HrInit )( 
            ITaskbarList * This);
        
        HRESULT ( STDMETHODCALLTYPE *AddTab )( 
            ITaskbarList * This,
            /* [in] */ HWND hwnd);
        
        HRESULT ( STDMETHODCALLTYPE *DeleteTab )( 
            ITaskbarList * This,
            /* [in] */ HWND hwnd);
        
        HRESULT ( STDMETHODCALLTYPE *ActivateTab )( 
            ITaskbarList * This,
            /* [in] */ HWND hwnd);
        
        HRESULT ( STDMETHODCALLTYPE *SetActiveAlt )( 
            ITaskbarList * This,
            /* [in] */ HWND hwnd);
        
        END_INTERFACE
    } ITaskbarListVtbl;

    interface ITaskbarList
    {
        CONST_VTBL struct ITaskbarListVtbl *lpVtbl;
    };

#ifdef COBJMACROS

#define ITaskbarList_QueryInterface(This,riid,ppvObject)    \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITaskbarList_AddRef(This)    \
    (This)->lpVtbl -> AddRef(This)

#define ITaskbarList_Release(This)    \
    (This)->lpVtbl -> Release(This)


#define ITaskbarList_HrInit(This)    \
    (This)->lpVtbl -> HrInit(This)

#define ITaskbarList_AddTab(This,hwnd)    \
    (This)->lpVtbl -> AddTab(This,hwnd)

#define ITaskbarList_DeleteTab(This,hwnd)    \
    (This)->lpVtbl -> DeleteTab(This,hwnd)

#define ITaskbarList_ActivateTab(This,hwnd)    \
    (This)->lpVtbl -> ActivateTab(This,hwnd)

#define ITaskbarList_SetActiveAlt(This,hwnd)    \
    (This)->lpVtbl -> SetActiveAlt(This,hwnd)

#endif /* COBJMACROS */

#endif     /* C style interface */

HRESULT STDMETHODCALLTYPE ITaskbarList_HrInit_Proxy( 
    ITaskbarList * This);

void __RPC_STUB ITaskbarList_HrInit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);

HRESULT STDMETHODCALLTYPE ITaskbarList_AddTab_Proxy( 
    ITaskbarList * This,
    /* [in] */ HWND hwnd);

void __RPC_STUB ITaskbarList_AddTab_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);

HRESULT STDMETHODCALLTYPE ITaskbarList_DeleteTab_Proxy( 
    ITaskbarList * This,
    /* [in] */ HWND hwnd);

void __RPC_STUB ITaskbarList_DeleteTab_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);

HRESULT STDMETHODCALLTYPE ITaskbarList_ActivateTab_Proxy( 
    ITaskbarList * This,
    /* [in] */ HWND hwnd);

void __RPC_STUB ITaskbarList_ActivateTab_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);

HRESULT STDMETHODCALLTYPE ITaskbarList_SetActiveAlt_Proxy( 
    ITaskbarList * This,
    /* [in] */ HWND hwnd);

void __RPC_STUB ITaskbarList_SetActiveAlt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);

#endif     /* __ITaskbarList_INTERFACE_DEFINED__ */

#ifndef __ITaskbarList2_INTERFACE_DEFINED__
#define __ITaskbarList2_INTERFACE_DEFINED__

/* interface ITaskbarList2 */
/* [object][uuid] */ 

EXTERN_C const IID IID_ITaskbarList2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("602D4995-B13A-429b-A66E-1935E44F4317")
    ITaskbarList2 : public ITaskbarList
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE MarkFullscreenWindow( 
            /* [in] */ HWND hwnd,
            /* [in] */ BOOL fFullscreen) = 0;
        
    };
    
#else     /* C style interface */

    typedef struct ITaskbarList2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITaskbarList2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITaskbarList2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITaskbarList2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *HrInit )( 
            ITaskbarList2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *AddTab )( 
            ITaskbarList2 * This,
            /* [in] */ HWND hwnd);
        
        HRESULT ( STDMETHODCALLTYPE *DeleteTab )( 
            ITaskbarList2 * This,
            /* [in] */ HWND hwnd);
        
        HRESULT ( STDMETHODCALLTYPE *ActivateTab )( 
            ITaskbarList2 * This,
            /* [in] */ HWND hwnd);
        
        HRESULT ( STDMETHODCALLTYPE *SetActiveAlt )( 
            ITaskbarList2 * This,
            /* [in] */ HWND hwnd);
        
        HRESULT ( STDMETHODCALLTYPE *MarkFullscreenWindow )( 
            ITaskbarList2 * This,
            /* [in] */ HWND hwnd,
            /* [in] */ BOOL fFullscreen);
        
        END_INTERFACE
    } ITaskbarList2Vtbl;

    interface ITaskbarList2
    {
        CONST_VTBL struct ITaskbarList2Vtbl *lpVtbl;
    };

#ifdef COBJMACROS

#define ITaskbarList2_QueryInterface(This,riid,ppvObject)    \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITaskbarList2_AddRef(This)    \
    (This)->lpVtbl -> AddRef(This)

#define ITaskbarList2_Release(This)    \
    (This)->lpVtbl -> Release(This)

#define ITaskbarList2_HrInit(This)    \
    (This)->lpVtbl -> HrInit(This)

#define ITaskbarList2_AddTab(This,hwnd)    \
    (This)->lpVtbl -> AddTab(This,hwnd)

#define ITaskbarList2_DeleteTab(This,hwnd)    \
    (This)->lpVtbl -> DeleteTab(This,hwnd)

#define ITaskbarList2_ActivateTab(This,hwnd)    \
    (This)->lpVtbl -> ActivateTab(This,hwnd)

#define ITaskbarList2_SetActiveAlt(This,hwnd)    \
    (This)->lpVtbl -> SetActiveAlt(This,hwnd)

#define ITaskbarList2_MarkFullscreenWindow(This,hwnd,fFullscreen)    \
    (This)->lpVtbl -> MarkFullscreenWindow(This,hwnd,fFullscreen)

#endif /* COBJMACROS */

#endif     /* C style interface */

HRESULT STDMETHODCALLTYPE ITaskbarList2_MarkFullscreenWindow_Proxy( 
    ITaskbarList2 * This,
    /* [in] */ HWND hwnd,
    /* [in] */ BOOL fFullscreen);

void __RPC_STUB ITaskbarList2_MarkFullscreenWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);

#endif     /* __ITaskbarList2_INTERFACE_DEFINED__ */

#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCONNECT_H__B99F0E55_6587_4EF8_88AD_66787BEE3240__INCLUDED_)
