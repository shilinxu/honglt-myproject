
#ifndef _World_Session_Map_h_
#define _World_Session_Map_h_

#include <list>
#include <ace/Thread_Mutex.h>
#include <ace/Null_Mutex.h>
#include <ace/Hash_Map_Manager.h>
#include "Session.h"

typedef uint32 NET_USER_ID;
typedef ACE_Hash_Map_Manager<NET_USER_ID,
                    CSession*,ACE_Null_Mutex> USER_SESSION_MAP;
typedef ACE_Hash_Map_Iterator<NET_USER_ID,
        CSession*,ACE_Null_Mutex> USER_SESSION_MAP_ITER;
class CSessionManager
{
public:

	//************************************
	// 方法:  CSessionManager
	// 全名:  CSessionManager::CSessionManager
	// 权限:  public 
	// 返回:  
	// 限定: 
	// 参数:  void
	//************************************
	CSessionManager(void);

	//************************************
	// 方法:  ~CSessionManager
	// 全名:  CSessionManager::~CSessionManager
	// 权限:  public 
	// 返回:  
	// 限定: 
	// 参数:  void
	//************************************
	~CSessionManager(void);
	
	//************************************
	// 方法:  create_session
	// 全名:  CSessionManager::create_session
	// 权限:  public 
	// 返回:  CWorldSession*
	// 限定: 
	// 参数:  uint32 uid
	//************************************
	CSession* create_session(uint32 uid);


	//************************************
	// 方法:  free_session_delay
	// 全名:  CSessionManager::free_session_delay
	// 权限:  public 
	// 返回:  void
	// 限定: 
	// 参数:  uint32 uid
	//************************************
	void free_session_delay(uint32 uid);

    /** 
     * @brief 释放需要已经被删除的对象
     */
	void free_session();

    /** 
     * @brief 清理所有分配的事务对象
     */
    void clear_session_map(USER_SESSION_MAP &sessionMap);

    void clear_all_session();

    CSession* find_session(uint32 nUserID,bool bIncludeDel=false,bool bIncludeLost=false);

    void update(uint32 nTime);

protected:

    USER_SESSION_MAP m_DeleteSessionMap;
    USER_SESSION_MAP m_SessionMap;
    USER_SESSION_MAP m_LostSessionMap;

private:
    //void clear_session_map(SessionMap &worldSessionMap);
};

//typedef ACE_Singleton<CSessionManager,ACE_Thread_Mutex> SESSION_MANAGER;
#endif // _World_Session_Map_h_
