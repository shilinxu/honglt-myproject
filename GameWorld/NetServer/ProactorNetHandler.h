/*
 * =====================================================================================
 *
 *       Filename:  PreactorNetHandler.h
 *
 *    Description:  :e 
 *
 *        Version:  1.0
 *        Created:  2009年08月13日 10时36分34秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Tony
 *        Company:  
 *
 * =====================================================================================
 */
#include <ace/Svc_Handler.h>
#include <ace/Message_Block.h>
#include <ace/Asynch_IO.h>
#include <ace/Mutex.h>
#include "TypeDef.h"

#include "NetServer.h"

class CProactorNetHandler:public ACE_Service_Handler,public INetClient
{
public:
    ~CProactorNetHandler();
    void open(ACE_HANDLE new_handle,ACE_Message_Block & message_block);
    void send_packet(const char* pBuffer,uint32 len);
	void set_server(CNetServer* pServer){m_pServer = pServer;}
    void set_session(CSession* pSession){m_pSession = pSession;}
    CSession* get_session(){return m_pSession;}
	void disconnect();
	bool test_delete_self();

    void bind_to_session(uint32 nUserID);
    void unbind_session();

protected:
    virtual void handle_timeout(const ACE_Time_Value &tv,const void *p);
    virtual void handle_read_stream(const ACE_Asynch_Read_Stream::Result &result);
	virtual void handle_write_stream(const ACE_Asynch_Write_Stream::Result &result);

	void read_packet_body( uint32 nSize );
	void read_packet_head();
	void free_handler();
	void send_packet();
    void do_packet();

private:
	uint32 m_nReadCount;
	uint32 m_nWriteCount;
    ACE_Message_Block *m_pReaderCache;
	ACE_Message_Block *m_pSenderCache;

    ACE_Asynch_Write_Stream m_writer;
    ACE_Asynch_Read_Stream  m_reader;
private:
    CNetServer *m_pServer;
    CSession *m_pSession;


    //ACE_Mutex m_mutex;
};
