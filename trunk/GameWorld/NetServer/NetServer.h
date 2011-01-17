/*
 * =====================================================================================
 *
 *       ¿ ¿ ¿:  NetServer.h
 * 
 *       ¿    ¿:  
 * 
 *       ¿    ¿:  1.0
 *       ¿¿¿¿:  2009¿02¿23¿ 19¿57¿21¿
 *       ¿¿¿¿:  none
 *       ¿ ¿ ¿:  g++
 * 
 *       ¿    ¿:  ¿¿ (TONY), mgphuang@163.com
 *       ¿    ¿:  ¿¿¿¿
 *
 * =====================================================================================
 */
#ifndef _NET_SERVER_H_
#define _NET_SERVER_H_

#include	<TypeDef.h>

#ifdef TEST
#include <gtest/gtest_prod.h>
#endif

#include <ace/Null_Mutex.h>
#include <ace/Task_Ex_T.h>
#include <ace/Hash_Map_Manager.h>
#include <ace/INET_Addr.h>
#include <ace/Event_Handler.h>
#include <ace/Message_Block.h>

//#include <boost/python.hpp>

#include <string>


class CSession;
class CSessionManager;

struct INetClient
{
    virtual void OnNewConnect(uint32 sockID){};
    virtual void OnLostConnect(){};
	virtual void OnReceive(const char* pBuffer,uint32 len){};
    virtual void send_packet(const char* pBuffer,uint32 len)=0;
};


typedef uint32 NET_SOCK_ID;

typedef ACE_Hash_Map_Manager<NET_SOCK_ID,
                    INetClient*,ACE_Null_Mutex> NET_SOCK_ID_MAP;
                    //ACE_Event_Handler*,ACE_Null_Mutex> NET_SOCK_ID_MAP;


//const int32 MB_NET_SERVER_SHUTDOWN = ACE_Message_Block::MB_USER +1;

class CNetServer:public ACE_Task<ACE_MT_SYNCH>
{
public:
    CNetServer();
    virtual ~CNetServer();

	virtual int32 start(uint32 nThreadCount );

    virtual int32 listen(uint16 nPort, std::string HostName);
    virtual int32 connect(uint16 nPort, std::string HostName);

    virtual int32 stop();
    int32 get_queue_max_size(){return this->msg_queue()->high_water_mark();};
    void set_queue_max_size(uint32 maxSize){this->msg_queue()->high_water_mark(maxSize);};
    //void set_auth_func(boost::python::object funAuth){m_funAuth = funAuth;}

    void register_handler(uint32 SockID,INetClient* pHandler);
	void unregister_handler(uint32 SockID,INetClient* pHandler);

    INetClient* get_handler(uint32 SockID);
    uint32 count_handler();

    CSessionManager* get_session_manager(){return m_pSessionManager;}
    /*-----------------------------------------------------------------------------
     *  send packet  
     *-----------------------------------------------------------------------------*/
	virtual void send(int32 SockID,const char* pBuffer,uint32 len);
	void send_user(uint32 nUserID,const char* pBuffer,uint32 len);
    virtual  void kick_client(NET_SOCK_ID SockID,uint32 delay){};


    void dump();
    int svc(void);
	static ACE_THR_FUNC_RETURN run(void*);

    /* *  
    * @brief ¿¿¿¿¿¿¿¿¿¿¿
        */
//    boost::python::object m_funAuth;
 //   uint32 auth(const char * pTicket);

protected:
	
	virtual int32 _listen(const ACE_INET_Addr &localHostAddr)=0;
    virtual int32 _connect(const ACE_INET_Addr &)=0;
    virtual int32 _stop()=0;
	virtual ACE_THR_FUNC_RETURN _run(void)=0;
    /*-----------------------------------------------------------------------------
     *  æ•°æ®åŒ…å‡½æ•?
     *-----------------------------------------------------------------------------*/
    void recv_error_packet(char* pPacketHeader,ACE_HANDLE clientHandle) ;

    /*-----------------------------------------------------------------------------
     *  è¿æ¥å¥æŸ„ç®¡ç†
     *-----------------------------------------------------------------------------*/
    //proxy <---> client id

    /**
     * @brief store proxy id to net_handler item
     */
    NET_SOCK_ID_MAP* m_pConnectHandlerMap;

    CSessionManager* m_pSessionManager;
    /**
     * @brief store client id to its proxy id;
     */

    uint16 m_PortNo;

    std::string m_HostName;

    ACE_INET_Addr m_HostAddr;

};

#endif
