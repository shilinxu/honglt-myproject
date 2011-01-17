/*
 * =====================================================================================
 *
 *       Filename:  ProactorNetAcceptor.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2009年08月13日 17时34分08秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef ProactorNetAcceptor_h__
#define ProactorNetAcceptor_h__

#include <ace/Asynch_Acceptor.h>
#include "ProactorNetHandler.h"

class CProactorNetServer;

class CProactorNetAcceptor:public ACE_Asynch_Acceptor<CProactorNetHandler>
{
public:
	void set_server(CProactorNetServer* pServer){m_pServer = pServer;};
	CProactorNetHandler* make_handler();
private:
	CProactorNetServer *m_pServer;

}
;

#endif // ProactorNetAcceptor_h__



