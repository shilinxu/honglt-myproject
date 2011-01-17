

#include "SessionManager.h"
#include "MyMarco.h"

CSessionManager::CSessionManager(void)
{
}

CSessionManager::~CSessionManager(void)
{
    clear_all_session();
}

CSession* CSessionManager::find_session(uint32 nUserID,
                                        bool bIncludeDel,
                                        bool bIncludeLost)
{
	CSession* pfind=NULL;

	if (m_SessionMap.find(nUserID,pfind) == 0)
		return pfind;
	else
    {
        if ((bIncludeDel)&&(m_DeleteSessionMap.find(nUserID,pfind) == 0))
            return pfind;
        else if ((bIncludeLost)&&(m_LostSessionMap.find(nUserID,pfind) == 0))
            return pfind;
        else return NULL;

    }
}

CSession* CSessionManager::create_session( uint32 uid )
{
	CSession* pNewSession=0;

	//�ȼ���ǲ����Ѿ��������������ˡ�
	pNewSession = find_session(uid);
	if (pNewSession != 0)
	{
		//////////////////////////////////////////////////////////////////////////
		//�˴���Ӧ�ô����û��Ͽ����Ӻ����������ϵ����������ͻ�����ϡ�
		//////////////////////////////////////////////////////////////////////////

        ACE_DEBUG((MY_LOG_TRACE "CSessionManager:return existed session user id:[%u]\n",uid));
		return pNewSession;
	}
	else
	{
		pNewSession = new CSession();
		pNewSession->UserID(uid);

		m_SessionMap.bind(uid,pNewSession);
        ACE_DEBUG((MY_LOG_TRACE "CSessionManager:ceate new session user id:[%u]\n",uid));

		return pNewSession;
	}
}

void CSessionManager::update( uint32 nTime )
{
   // //�������е������
   // for (USER_SESSION_MAP::iterator iter = m_WorldSessionMap.begin();
   // 	iter!=m_WorldSessionMap.end();iter++)
   // {
   // 	iter->item()->update(nTime);
   // }

   // //�����е���������ݰ����͵����Ӿ���С�
   // for (USER_SESSION_MAP::iterator iter = m_WorldSessionMap.begin();
   //     iter!=m_WorldSessionMap.end();iter++)
   // {
   //     
   // }
}

void CSessionManager::free_session_delay( uint32 uid )
{
	CSession* pSession = find_session(uid);
	if (pSession)
	{
		m_SessionMap.unbind(uid);
		m_DeleteSessionMap.bind(uid,pSession);
	}
}

void CSessionManager::free_session()
{
	for (USER_SESSION_MAP_ITER p = m_DeleteSessionMap.begin();
		p!=m_DeleteSessionMap.end();p++)
	{
		#ifdef WIN32
				delete dynamic_cast<CSession*>((*p).int_id_);
		#else
				delete dynamic_cast<CSession*>((*p).item());
		#endif
	}
	m_DeleteSessionMap.unbind_all();
}

//int32 CSessionManager::push_packet( const CWorldPacket *pPacket,uint32 nNetHandleID )
//{

//	CWorldPacket* pNewPacket = new CWorldPacket(*pPacket);
//
//	//��ʼ����������İ���
//	switch (pNewPacket->opt_code())
//	{
//        //ϵͳ�˳���
//        case SY_GAME_WORLD_EXIST:
//            GAME_WORLD::instance()->shutdown();
//            break;
//	}
//
//	//�������������ݰ�
//	CSession* pSession=0;
//	pSession = SESSION_MANAGER::instance()->find(pNewPacket->user_id());
//	if (pSession)
//		pSession->push_rev_queue(pNewPacket);
//	else
//	{
//        ACE_DEBUG((MY_LOG_ERROR 
//			"Client_Net_Handle:rev packet from user:��%u��,but user id is not exist in WorldSessionMap\n",
//			pNewPacket->user_id()));
//		#ifndef _GGate
//		//�����Ӧ�ô���ʽ�ģ��Ǵ����Ӳ��ᱻ�رա�Ŀǰÿ�����ֱ�����ӣ�������Ҫ�رմ����ӡ�
//		return -1;
//		#endif
//	}
//	return 0;
//}

void CSessionManager::clear_session_map(USER_SESSION_MAP &sessionMap)
{
    for (USER_SESSION_MAP::iterator p = sessionMap.begin();
            p!=sessionMap.end();p++)
	{
		#ifdef WIN32
			delete dynamic_cast<CSession*>((*p).int_id_);
		#else
			delete dynamic_cast<CSession*>((*p).item());
		#endif
	}
    sessionMap.unbind_all();
}
void CSessionManager::clear_all_session()
{
    clear_session_map(m_DeleteSessionMap);
    clear_session_map(m_SessionMap);
    clear_session_map(m_LostSessionMap);
}
