/*
 * =====================================================================================
 *
 *       Filename:  ProactorNetAcceptor.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2009年08月13日 17时34分19秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */



#include "ProactorNetAcceptor.h"
#include "ProactorNetServer.h"

CProactorNetHandler* CProactorNetAcceptor::make_handler()
{
	//此处可以使用内存池对相同句柄进行引用计数,以减少内存的使用数量.对多个连接,返回相同的处理句柄
	CProactorNetHandler* pHandler = 0;
	//优化为内存池！
	ACE_NEW_RETURN(pHandler,CProactorNetHandler(),0);
	pHandler->set_server(dynamic_cast<CNetServer*>(m_pServer));
	return pHandler;
}

