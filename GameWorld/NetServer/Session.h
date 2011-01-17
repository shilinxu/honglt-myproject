#ifndef _Session_h_
#define _Session_h_

#include <TypeDef.h> 
#include <string>
#include <ace/Time_Value.h>


enum TSessionMode{
    TSM_CONNECT,
    TSM_AUTH,
    TSM_DISCONNECT
};

class CSession
{
public:
	CSession(void);
	~CSession(void);

	void notify_receive();
    void notify_login();
    void notify_connect();
    void notify_auth();
    void notify_disconnect();

	uint32 NetHandleID() const { return m_nNetHandleID; }
	void NetHandleID(uint32 val) { m_nNetHandleID = val; }

	uint32 UserID() const { return m_nUserID; }
	void UserID(uint32 val) { m_nUserID = val; }
	
    void update(uint32 nTime){};

protected:
	bool m_bEmpty;					//�Ƿ���ʹ���С�
	uint32 m_nUserID;				//���Ψһ���
	uint8 m_nCertification[33];		//��¼Ʊ��
	uint32 m_nCurrentMode;			//��ǰ״̬�����ӣ���¼�����˳�
	uint32 m_nNetHandleID;
	ACE_Time_Value m_tvLastRevData;
    ACE_Time_Value m_tvLogin;

private:

};


#endif //_World_Session_h_
