/*
 * =====================================================================================
 *
 *       Filename:  ProactorNetConnecter.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2009年12月17日 22时21分06秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef __PROACTORNETHANDLER__
#define __PROACTORNETHANDLER__

#include <ace/Asynch_Connector.h>

class CProactorNetServer;
class CProactorNetHandler;

class CProactorNetConnector:public ACE_Asynch_Connector<CProactorNetHandler>
{
public:
	void set_server(CProactorNetServer* pServer){m_pServer = pServer;};
    CProactorNetHandler* make_handler();
private:
    CProactorNetServer *m_pServer;
};

#endif 
