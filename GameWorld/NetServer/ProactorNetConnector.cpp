/*
 * =====================================================================================
 *
 *       Filename:  ProactorNetConnector.cpp
 *
 *    Description:  :w
 *
 *        Version:  1.0
 *        Created:  2009年12月17日 22时22分15秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include "ProactorNetConnector.h"
#include "ProactorNetServer.h"
#include "ProactorNetHandler.h"

CProactorNetHandler* CProactorNetConnector::make_handler()
{
    CProactorNetHandler* pHandler = 0;

    ACE_NEW_RETURN(pHandler,CProactorNetHandler(),0);

	pHandler->set_server(dynamic_cast<CNetServer*>(m_pServer));

	return pHandler;

}
