#include "Session.h"
#include <ace/OS.h>

CSession::CSession(void)
:m_nNetHandleID(0)
{

	m_tvLastRevData = ACE_OS::gettimeofday();
}

CSession::~CSession(void)
{

}

void CSession::notify_receive()
{}
void CSession::notify_login()
{}
void CSession::notify_connect()
{}
void CSession::notify_auth()
{}
void CSession::notify_disconnect()
{}

