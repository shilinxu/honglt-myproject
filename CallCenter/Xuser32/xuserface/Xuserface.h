//
// Xuserface.h
//

#ifndef XUSERFACE_CONST
#define XUSERFACE_CONST

//////////////////////////////////////////////////////////////////////////
// definitions of payload types

#define QCIF_WIDTH  176
#define QCIF_HEIGHT 144

// ���ر��ַ�ʽ

#define RenderPayload(a)	( (1 << 12) | (a & 0x0FFF) )	// �ɼ�
#define PreviewPayload(a)	( (2 << 12) | (a & 0x0FFF) )	// Ԥ��
#define RecordPayload(a)	( (4 << 12) | (a & 0x0FFF) )	// ¼��

// ��������

enum PayloadMask
{
	PayloadMask_Audio	= 0x1,
	PayloadMask_Video	= 0x2,
	PayloadMask_File	= 0x4,
	PayloadMask_Control	= 0x8,
	PayloadMask_Unknown	= 0xf,
};

//////////////////////////////////////////////////////////////////////////
// definitions of thread state

#define TM_SHUTDOWN		101
#undef ToHex

//////////////////////////////////////////////////////////////////////////
// definitions of debug types

#define MSG_DEFAULT		0
#define MSG_SYSTEM		1
#define MSG_ERROR		2
#define MSG_DEBUG		3
#define MSG_TEMP		4

#endif // XUSERFACE_CONST

#ifdef __cplusplus
	extern "C"{
#endif

class CXMLElement;

//////////////////////////////////////////////////////////////////////////
// SOCKET connection

/****************************************************************
 *   ���� : XConnect(pAddress, nPort)
 *   ���� : ������������
 *   ���� : pAddress -- ����IP��ַ
            nPort -- ���ض˿�
 *   ��� : 0 -- �ɹ�
 ****************************************************************/
int WINAPI XConnect(LPCTSTR pAddress, int nPort);

/****************************************************************
 *   ���� : Xdisconnect()
 *   ���� : ע����������
 *   ���� : ��
 *   ��� : ��
 ****************************************************************/
void WINAPI XDisconnect();

//////////////////////////////////////////////////////////////////////////
// Gateway operations

/****************************************************************
 *   ���� : XLoginHost(xUserId, xPassword, bTestor)
 *   ���� : �û���¼
 *   ���� : xUserId -- �û�ID����绰���룬��Ա�����
            xPassword -- �û�����
			bTestor -- ΪTrueʱ, xUserIdָ��Ա�����
 *   ��� :    0 -- �ɹ�
            8000 -- δ֪
			8001 -- ���޴���
			8002 -- Ƿ��ͣ��
 ****************************************************************/
int WINAPI XLoginHost(LPCTSTR xUserId, LPCTSTR xPassword, BOOL bTestor);

/****************************************************************
 *   ���� : XCallWorker(string, nTimeOut)
 *   ���� : �¼���ѯ
 *   ���� : string -- �¼�����, XML��ʽ
            nTimeOut -- �ȴ���ʱʱ��, ȱʡ6000΢��
 *   ��� :    0 -- �ɹ�
 ****************************************************************/
LONG WINAPI XCallWorker(LPTSTR string, DWORD nTimeOut);

//////////////////////////////////////////////////////////////////////////
// Directshow operations

/****************************************************************
 *   ���� : XRenderStream(nPayloadMask, lpszName)
 *   ���� : ���ر�������Ƶ
 *   ���� : nPayloadMask -- ���ر��ַ�ʽ, ��PayloadMask��RenderPayload,PreviewPayload��϶���
				eg: RenderPayload(PayloadMask_Video)|PreviewPayload(PayloadMask_Video)
            lpszName -- ��ͼ����
 *   ��� : >=0 -- �ɹ�
 ****************************************************************/
HRESULT WINAPI XRenderStream(WORD nPayloadMask, LPCTSTR lpszName);

/****************************************************************
 *   ���� : XRenderPreview(nPayloadMask, lpszName)
 *   ���� : Ԥ��Զ������Ƶ
 *   ���� : nPayloadMask (ͬ��)
            lpszName -- ��ͼ����
 *   ��� : >=0 -- �ɹ�
 ****************************************************************/
HRESULT WINAPI XRenderPreview(WORD nPayloadMask, LPCTSTR lpszName);

/****************************************************************
 *   ���� : XStartPreview(rect, hWnd, lpszName)
 *   ���� : ��ʾԤ������Ƶ��Դ
 *   ���� : rect -- ����λ��(left, top, right, bottom)
			hWnd -- ������
            lpszName -- ָ����Ҫ��ʾ����ͼ����
			         -- NULL��ʾ������Դ
 *   ��� : >=0 -- �ɹ�
 ****************************************************************/
HRESULT WINAPI XStartPreview(const RECT& rect, HWND hWnd, LPCTSTR lpszName);

/****************************************************************
 *   ���� : XResetRender()
 *   ���� : ���������ͼ
 *   ���� : ��
 *   ��� : ��
 ****************************************************************/
void WINAPI XResetRender();

//////////////////////////////////////////////////////////////////////////
// Chat operations

/****************************************************************
 *   ���� : XDialupWith(xRemoteId, xCallerId)
 *   ���� : �ύ�绰���
 *   ���� : xRemoteId -- �Է�����
			xCallerId -- ��ʾ����
 *   ��� : >=0 -- �ɹ�
 ****************************************************************/
HRESULT WINAPI XDialupWith(LPCTSTR xRemoteId, LPCTSTR xCallerId);

/****************************************************************
 *   ���� : XLeaveTalk(xRemoteId, xCallerId)
 *   ���� : �����Ի�
 *   ���� : ��
 *   ��� : >=0 -- �ɹ�
 ****************************************************************/
HRESULT WINAPI XLeaveTalk();

/****************************************************************
 *   ���� : XStartTalk()
 *   ���� : ͨ����ʼ
 *   ���� : ��
 *   ��� : ��
 ****************************************************************/
void WINAPI XStartTalk();

/****************************************************************
 *   ���� : XRecordFile()
 *   ���� : �ύ¼������
 *   ���� : ��
 *   ��� : >=0 -- �ɹ�
 ****************************************************************/
HRESULT WINAPI XRecordFile();

//////////////////////////////////////////////////////////////////////////
// Debug operations

DWORD WINAPI XGetLastError();
void WINAPI XDebugLog(int nType, LPCTSTR pszFormat, ...);

#ifdef __cplusplus
	}
#endif

//////////////////////////////////////////////////////////////////////////
// Utility Functions

extern LPCTSTR _tcsistr(LPCTSTR pszString, LPCTSTR pszPattern);
extern LPCTSTR _tcsnistr(LPCTSTR pszString, LPCTSTR pszPattern, DWORD plen);

extern CString ToHex(BYTE* pBuffer, DWORD nLength);
extern CString ToASCII(BYTE* pBuffer, DWORD nLength);
