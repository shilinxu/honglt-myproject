//
// * =====================================================================================
// *
// *       文 件 名:  PreactorNetServer.h
// * 
// *       描    述:  
// * 
// *       版    本:  1.0
// *       创建日期:  2009年02月23日 21时15分00秒
// *       重构版本:  none
// *       编 译 器:  g++
// * 
// *       作    者:  黄勇 (TONY), mgphuang@163.com
// *       公    司:  通用科技
// *
// * =====================================================================================
// 
#ifndef _PREACTORNETSERVER_H
#define _PREACTORNETSERVER_H


#include "TypeDef.h"
#include "NetServer.h"

class CProactorNetHandler;
class CProactorNetAcceptor;

class CProactorNetServer : public CNetServer 
{
public:

  CProactorNetServer();

  virtual ~CProactorNetServer();

  int32 _start(const ACE_INET_Addr &localHostAddr);
  ACE_THR_FUNC_RETURN _run(void);
  int _stop();


protected:


public:

  /*-----------------------------------------------------------------------------
   *  
   *-----------------------------------------------------------------------------*/

  /*-----------------------------------------------------------------------------
  *  数据收发
  *-----------------------------------------------------------------------------*/

  //virtual void send(int32 SockID,ACE_Message_Block* pPacket){};
  //virtual void send(int32 SockID,const char* pBuffer,uint32 len){};
  /*-----------------------------------------------------------------------------
   *  对客户端连接的操作,所有方法为异步操作
   *----------------------------------------------------------------------------*/
private:
	CProactorNetAcceptor* m_pAcceptor;
};
#endif
