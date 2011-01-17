
#include "ProactorNetServer.h"
#include "ProactorNetAcceptor.h"
#include "ProactorNetConnector.h"
#include <ace/Proactor.h>
#include "MyMarco.h"

CProactorNetServer::CProactorNetServer()
:m_pAcceptor(0),m_pConnector(0)
{

}

CProactorNetServer::~CProactorNetServer()
{
    SAFE_DELETE(m_pAcceptor);
    SAFE_DELETE(m_pConnector);
}

int32 CProactorNetServer::_listen( const ACE_INET_Addr &localHostAddr )
{
	if (m_pAcceptor == 0)
	{
		ACE_NEW_RETURN(m_pAcceptor,CProactorNetAcceptor(),-1);
		m_pAcceptor->set_server(this);
	}
	m_pAcceptor->open(localHostAddr);
	return 0;
}

int32 CProactorNetServer::_stop()
{
    if (m_pAcceptor) m_pAcceptor->cancel();
    if (m_pConnector) m_pConnector->cancel();
    return 0;
}

int32 CProactorNetServer::_connect(const ACE_INET_Addr &remoteHostAddr)
{
   if (m_pConnector == 0) 
   {
       ACE_NEW_RETURN(m_pConnector,CProactorNetConnector(),-1);
       m_pConnector->set_server(this);
	   m_pConnector->open();
   }
   
   return m_pConnector->connect(remoteHostAddr);

}
ACE_THR_FUNC_RETURN CProactorNetServer::_run(void)
{
	ACE_Proactor::instance()->proactor_run_event_loop();
	return 0;
}

