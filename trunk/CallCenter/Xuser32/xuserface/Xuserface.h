//
// Xuserface.h
//

#ifndef XUSERFACE_CONST
#define XUSERFACE_CONST

//////////////////////////////////////////////////////////////////////////
// definitions of payload types

#define QCIF_WIDTH  176
#define QCIF_HEIGHT 144

// 负载表现方式

#define RenderPayload(a)	( (1 << 12) | (a & 0x0FFF) )	// 采集
#define PreviewPayload(a)	( (2 << 12) | (a & 0x0FFF) )	// 预览
#define RecordPayload(a)	( (4 << 12) | (a & 0x0FFF) )	// 录像

// 负载类型

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
 *   函数 : XConnect(pAddress, nPort)
 *   描述 : 启动网络连接
 *   输入 : pAddress -- 网关IP地址
            nPort -- 网关端口
 *   输出 : 0 -- 成功
 ****************************************************************/
int WINAPI XConnect(LPCTSTR pAddress, int nPort);

/****************************************************************
 *   函数 : Xdisconnect()
 *   描述 : 注销网络连接
 *   输入 : 无
 *   输出 : 无
 ****************************************************************/
void WINAPI XDisconnect();

//////////////////////////////////////////////////////////////////////////
// Gateway operations

/****************************************************************
 *   函数 : XLoginHost(xUserId, xPassword, bTestor)
 *   描述 : 用户登录
 *   输入 : xUserId -- 用户ID，或电话号码，或员工编号
            xPassword -- 用户密码
			bTestor -- 为True时, xUserId指向员工编号
 *   输出 :    0 -- 成功
            8000 -- 未知
			8001 -- 查无此人
			8002 -- 欠费停机
 ****************************************************************/
int WINAPI XLoginHost(LPCTSTR xUserId, LPCTSTR xPassword, BOOL bTestor);

/****************************************************************
 *   函数 : XCallWorker(string, nTimeOut)
 *   描述 : 事件查询
 *   输入 : string -- 事件内容, XML格式
            nTimeOut -- 等待超时时间, 缺省6000微秒
 *   输出 :    0 -- 成功
 ****************************************************************/
LONG WINAPI XCallWorker(LPTSTR string, DWORD nTimeOut);

//////////////////////////////////////////////////////////////////////////
// Directshow operations

/****************************************************************
 *   函数 : XRenderStream(nPayloadMask, lpszName)
 *   描述 : 加载本地音视频
 *   输入 : nPayloadMask -- 负载表现方式, 由PayloadMask与RenderPayload,PreviewPayload组合而成
				eg: RenderPayload(PayloadMask_Video)|PreviewPayload(PayloadMask_Video)
            lpszName -- 视图名称
 *   输出 : >=0 -- 成功
 ****************************************************************/
HRESULT WINAPI XRenderStream(WORD nPayloadMask, LPCTSTR lpszName);

/****************************************************************
 *   函数 : XRenderPreview(nPayloadMask, lpszName)
 *   描述 : 预览远端音视频
 *   输入 : nPayloadMask (同上)
            lpszName -- 视图名称
 *   输出 : >=0 -- 成功
 ****************************************************************/
HRESULT WINAPI XRenderPreview(WORD nPayloadMask, LPCTSTR lpszName);

/****************************************************************
 *   函数 : XStartPreview(rect, hWnd, lpszName)
 *   描述 : 显示预览音视频资源
 *   输入 : rect -- 窗体位置(left, top, right, bottom)
			hWnd -- 窗体句柄
            lpszName -- 指定需要显示的视图名称
			         -- NULL表示本地资源
 *   输出 : >=0 -- 成功
 ****************************************************************/
HRESULT WINAPI XStartPreview(const RECT& rect, HWND hWnd, LPCTSTR lpszName);

/****************************************************************
 *   函数 : XResetRender()
 *   描述 : 清除所有视图
 *   输入 : 无
 *   输出 : 无
 ****************************************************************/
void WINAPI XResetRender();

//////////////////////////////////////////////////////////////////////////
// Chat operations

/****************************************************************
 *   函数 : XDialupWith(xRemoteId, xCallerId)
 *   描述 : 提交电话外呼
 *   输入 : xRemoteId -- 对方号码
			xCallerId -- 显示号码
 *   输出 : >=0 -- 成功
 ****************************************************************/
HRESULT WINAPI XDialupWith(LPCTSTR xRemoteId, LPCTSTR xCallerId);

/****************************************************************
 *   函数 : XLeaveTalk(xRemoteId, xCallerId)
 *   描述 : 结束对话
 *   输入 : 无
 *   输出 : >=0 -- 成功
 ****************************************************************/
HRESULT WINAPI XLeaveTalk();

/****************************************************************
 *   函数 : XStartTalk()
 *   描述 : 通话开始
 *   输入 : 无
 *   输出 : 无
 ****************************************************************/
void WINAPI XStartTalk();

/****************************************************************
 *   函数 : XRecordFile()
 *   描述 : 提交录音请求
 *   输入 : 无
 *   输出 : >=0 -- 成功
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
