/*
 * =====================================================================================
 *
 *       Filename:  PreactorNetHandler.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2009�?8�?3�?10�?6�?3�?
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Tony
 *        Company:  
 *
 * =====================================================================================
 */

#include "ProactorNetHandler.h"
#include "ProactorNetServer.h"

#include "Protocol.h"
#include "MyMarco.h"

#include "NetServer.h"
#include "SessionManager.h"
//#include <boost/python.hpp>

#include <ace/OS.h>

CProactorNetHandler::~CProactorNetHandler()
{
    //unbind_session();
}

void CProactorNetHandler::open(ACE_HANDLE new_handle, ACE_Message_Block &message_block)
{
	//将自己注册到服务�?
	m_pServer->register_handler((uint32)handle(),
            (INetClient*)(this));
    m_pSession = NULL;
	m_nReadCount = 0;
	m_nWriteCount = 0;
	m_pSenderCache = NULL;
    m_reader.open(*this,handle(),0,proactor());
	m_writer.open(*this,handle(),0,proactor());
	read_packet_head();
    return ;
}

void CProactorNetHandler::handle_timeout(const ACE_Time_Value &tv,const void *p)
{
    
}

void CProactorNetHandler::handle_read_stream(const ACE_Asynch_Read_Stream::Result &result)
{
	ACE_DEBUG((MY_LOG_INFO "CProactorNetHandler::handle_read_stream callback\n"));
	m_nReadCount--;	

    if (!result.success() || result.bytes_transferred()==0)
        //读出错或连接断开
    {
        //disconnect();
		//if (test_delete_self())
		//	return;
        m_pReaderCache->release();
        disconnect();
        delete this;
        return;
    }

    else if (result.bytes_transferred() < result.bytes_to_read())
        //碎片包，接收余下部分
    {
		read_packet_body(result.bytes_to_read()-result.bytes_transferred());
    }
    else if (m_pReaderCache->length() == PACKET_HEADER_SIZE)
        //接收完成的数据包�?
    {
        LPTPACKET_HEADER pHeader = (LPTPACKET_HEADER)(m_pReaderCache->base());
		if ((pHeader->m_wPacketHead != 0x5342)||(pHeader->m_wSize > BUFFERSIZE))
		{
			ACE_DEBUG((MY_LOG_ERROR "CProactorNetHandler:receive unknown format packet,kick socket %d \n",handle()));
			disconnect();
	        ACE_DEBUG((MY_LOG_INFO "CProactorNetHandler::handle_read_stream callback return \n"));
			return;
		}

        if (pHeader->m_wSize > 0)
        {
			read_packet_body(pHeader->m_wSize);
	        ACE_DEBUG((MY_LOG_INFO "CProactorNetHandler::handle_read_stream callback return \n"));
            return;
        }

        do_packet();
        read_packet_head();
    }
	else 
	{
        do_packet();
		read_packet_head();
	}
	ACE_DEBUG((MY_LOG_INFO "CProactorNetHandler::handle_read_stream callback return \n"));
	return;
}

void CProactorNetHandler::handle_write_stream(const ACE_Asynch_Write_Stream::Result &result)
{

	ACE_DEBUG((MY_LOG_INFO "CProactorNetHandler::handle_write_stream callback\n"));
    result.message_block().release();
    return;
    
//    ACE_Message_Block *pMessage = m_pSenderCache;
//    m_mutex.acquire();
//    m_pSenderCache = m_pSenderCache->next();
//    m_mutex.release();
//    delete pMessage;
//	m_nWriteCount--;
//	send_packet();
}

void CProactorNetHandler::free_handler()
{
	
}


void CProactorNetHandler::read_packet_head()
{
	ACE_NEW(m_pReaderCache,ACE_Message_Block(ACE_DEFAULT_CDR_BUFSIZE));
	m_reader.read(*m_pReaderCache,PACKET_HEADER_SIZE);
	m_nReadCount++;
}

void CProactorNetHandler::disconnect()
{
	if (handle() == ACE_INVALID_HANDLE)
		return;
	m_reader.cancel();
	m_writer.cancel();
	ACE_OS::closesocket((handle()));

	m_pServer->unregister_handler(
            (uint32)handle(),
            dynamic_cast<INetClient*>(this));
	handle(ACE_INVALID_HANDLE);
}

void CProactorNetHandler::send_packet(const char* pBuffer,uint32 len)
{

	ACE_DEBUG((MY_LOG_INFO "CProactorNetHandler::send_packet input\n"));
    ACE_Message_Block * mblk=0;
	ACE_NEW(mblk,ACE_Message_Block(ACE_DEFAULT_CDR_BUFSIZE));
    ACE_OS::memcpy(mblk->base(),pBuffer,len);
    mblk->wr_ptr(len);
    m_writer.write(*mblk,mblk->length());
	ACE_DEBUG((MY_LOG_INFO "CProactorNetHandler::send_packet return\n"));
    return;

	//if (m_pSenderCache == NULL)
    //{
    //    m_mutex.acquire();
	//	m_pSenderCache = mblk;
    //    m_mutex.release();
    //}
	//else
	//{
	//	ACE_Message_Block * pMessage=m_pSenderCache;
	//	while (pMessage->next() != NULL)
	//	{
	//		pMessage = pMessage->next();
	//	}
	//	pMessage->next(mblk);
	//}
    //send_packet();
}

void CProactorNetHandler::send_packet()
{
	ACE_DEBUG((MY_LOG_INFO "CProactorNetHandler::send packet callback\n"));
    if (m_pSenderCache == NULL) return;

    if (m_nWriteCount == 0)
    {
	    ACE_DEBUG((MY_LOG_INFO "CProactorNetHandler::asyn write!\n"));
        m_writer.write(*m_pSenderCache,m_pSenderCache->length());
        m_nWriteCount++;
    }
}

void CProactorNetHandler::read_packet_body( uint32 nSize )
{
	m_reader.read(*m_pReaderCache,nSize);
	m_nReadCount++;
}

bool CProactorNetHandler::test_delete_self()
{
	ACE_DEBUG((MY_LOG_INFO "CProactorNetHandler::test_delete_self\n"));
	ACE_DEBUG((MY_LOG_INFO "ReadCount=%d\tWriteCount=%d",m_nReadCount,m_nWriteCount));
	if ((m_nReadCount ==0)&&(m_nWriteCount==0))
	{
	    ACE_DEBUG((MY_LOG_INFO "CProactorNetHandler::del this %d\n",this));
		delete this;
		return true;
	}
	else
		return false;
}

void CProactorNetHandler::do_packet()
{
    send_packet(m_pReaderCache->base(),m_pReaderCache->length());
    //if (m_pSession == NULL)
    //{
    //    /*-----------------------------------------------------------------------------
    //     *  验证连接票据
    //     *-----------------------------------------------------------------------------*/
    //    //临时
    //    uint32 sockid = handle();
    //    uint32 nUserID = m_pServer->auth((const char*)&sockid);
    //    //uint32 nUserID = m_pServer->auth(m_pReaderCache->base());
    //    if (nUserID != 0)
    //    {                                 
    //        bind_to_session(nUserID);
    //    }
    //    else
    //    {
    //        disconnect(); 
    //    }
    //}
    //else
    //{
	//    ACE_DEBUG((MY_LOG_INFO "CProactorNetHandler::put\n"));
        m_pServer->putq(m_pReaderCache);
	//    ACE_DEBUG((MY_LOG_INFO "CProactorNetHandler::put ok\n"));
    //}
	//ACE_DEBUG((MY_LOG_INFO "CProactorNetHandler::do_packet ok\n"));
}

void CProactorNetHandler::bind_to_session(uint32 nUserID)
{
	ACE_DEBUG((MY_LOG_INFO "CProactorNetHandler::bind_to_session\n"));
    CSession* pSession = NULL;
    if (get_session() == NULL)
        pSession = m_pServer->get_session_manager()->create_session(nUserID);
    pSession->NetHandleID((uint32)handle());
    this->set_session(pSession);
	ACE_DEBUG((MY_LOG_INFO "CProactorNetHandler::bind_to_session return\n"));
}

void CProactorNetHandler::unbind_session()
{
	ACE_DEBUG((MY_LOG_INFO "CProactorNetHandler::unbind_to_session\n"));
    CSession* pSession = get_session();
    if (pSession)
        m_pServer->get_session_manager()->free_session_delay(pSession->UserID());
	ACE_DEBUG((MY_LOG_INFO "CProactorNetHandler::unbind_to_session return\n"));
}
