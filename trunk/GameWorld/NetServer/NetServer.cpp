/*
 * =====================================================================================
 *
 *       文 件 名:  NetServer.cpp
 * 
 *       描    述:  
 * 
 *       版    本:  1.0
 *       创建日期:  2009年02月23日 19时57分21秒
 *       重构版本:  none
 *       编 译 器:  g++
 * 
 *       作    者:  黄勇 (TONY), mgphuang@163.com
 *       公    司:  通用科技
 *
 * =====================================================================================
 */

#include "NetServer.h"
#include "MyMarco.h"
#include "Protocol.h"
#include "SessionManager.h"

CNetServer::CNetServer()
{

    
}
CNetServer::~CNetServer()
{
    m_pConnectHandlerMap->unbind_all();
    delete m_pConnectHandlerMap;
    m_pConnectHandlerMap = NULL;
    delete m_pSessionManager;
    m_pSessionManager = NULL;
}

int32 CNetServer::start( uint32 nThreadCount )
{

    //if (m_funAuth.ptr() == NULL)
    //{
    //    ACE_DEBUG((MY_LOG_INFO "CNetServer::start : please set auth function first! \n"));
    //    return -1;

    //}

    //创建连接管理器 
    ACE_NEW_RETURN(m_pConnectHandlerMap,NET_SOCK_ID_MAP(),-1);

    //创建事务管理器
    ACE_NEW_RETURN(m_pSessionManager,CSessionManager(),-1);

    //激活主动对象
	this->activate();

    //启动IO处理线程
	ACE_Thread_Manager::instance()->spawn_n(nThreadCount,&CNetServer::run,(void*)this);

    return 0;
}

int32 CNetServer::listen(uint16 nPort, std::string HostName )
{
    m_PortNo = nPort;
    m_HostName = HostName;

    if (HostName.empty())
    {
        m_HostAddr = ACE_INET_Addr(m_PortNo);
    }
    else
    {
        m_HostAddr = ACE_INET_Addr(m_PortNo,m_HostName.c_str());
    }

	if (_listen(m_HostAddr) != 0)
		return -1;
	return 0;
}

int32 CNetServer::connect(uint16 nPort, std::string HostName )
{
    m_PortNo = nPort;
    m_HostName = HostName;

    if (HostName.empty())
    {
        m_HostAddr = ACE_INET_Addr(m_PortNo);
    }
    else
    {
        m_HostAddr = ACE_INET_Addr(m_PortNo,m_HostName.c_str());
    }

	if (_connect(m_HostAddr) != 0)
		return -1;
	return 0;
}

int32 CNetServer::stop()
{
	return _stop();
}

void CNetServer::register_handler(uint32 SockID,INetClient* pHandler)
{
    INetClient *pOldHandler = get_handler(SockID);
    if (pOldHandler!=0)
        ACE_DEBUG((MY_LOG_ERROR "CNetServer:exist handle by sock id: %d \n",SockID));
    m_pConnectHandlerMap->bind(SockID,pHandler);
}

void CNetServer::unregister_handler(uint32 SockID,INetClient* pHandler)
{
	INetClient* pOldHandler = get_handler(SockID);
	if (pOldHandler == pHandler)
	{
		m_pConnectHandlerMap->unbind(SockID,pHandler);
	}
}
uint32 CNetServer::count_handler()
{
    return m_pConnectHandlerMap->current_size();
}

INetClient* CNetServer::get_handler(NET_SOCK_ID SockID)
{
    INetClient* pHander = 0;
    if (0==m_pConnectHandlerMap->find(SockID,pHander))
	{
		ACE_DEBUG((MY_LOG_INFO "CNetServer:find handle by sock id: %d \n",SockID));
        return pHander;
    }
    else
    {
        ACE_DEBUG((MY_LOG_INFO "CNetServer:miss handle by id: %d \n",SockID));
        return 0;
    }

}

void CNetServer::recv_error_packet(char* pPacketHeader,ACE_HANDLE clientHandle)
{
    ACE_DEBUG((LM_ERROR,"recv a unknown packet! \n"));    
    ACE_HEX_DUMP((LM_INFO,pPacketHeader,PACKET_HEADER_SIZE));
}

void CNetServer::dump()
{

}

int CNetServer::svc()
{
//
//		if (mblk->msg_type() == MB_NET_SERVER_SHUTDOWN)
//		{
//			stop();
//			return 0;
//		}
//         ACE_HEX_DUMP((LM_INFO,mblk->base(),mblk->length()));
//         
//         delete mblk;
    return 0;
}

void CNetServer::send(int32 SockID,const char* pBuffer,uint32 len)
{
    INetClient* pHandle = get_handler(SockID);
    if (pHandle == 0)return; 
    static_cast<INetClient*>(pHandle)->send_packet(pBuffer,len);
}

void CNetServer::send_user(uint32 nUserID,const char* pBuffer,uint32 len)
{
    CSession* pSession = get_session_manager()->find_session(nUserID);
    if (pSession)
        send(pSession->NetHandleID(),pBuffer,len);
    else
        ACE_DEBUG((LM_INFO,"CNetServer::miss session UserID:%d! \n",nUserID));    


}

ACE_THR_FUNC_RETURN CNetServer::run( void* argv)
{
	CNetServer* pServer = (CNetServer*)argv;
	ACE_THR_FUNC_RETURN r;
	r = pServer->_run();
	return r;
}

//uint32 CNetServer::auth(const char* pTicket)
//{
//    return *(uint32*)pTicket;
//    return boost::python::call<uint32>(m_funAuth.ptr(),pTicket);
//}
