 //                               �����ȨЭ��
//1��	����������Ϣ�������޹�˾�����¼�Ʊ���˾��ӵ��"����������и�����Ʒ���ļ�������ĵ�"�����¼�Ʊ���Ʒ������ȫ��Ȩ���κε�λ�͸�����ʹ��ǰ���ñ���˾����Ȩ��
//2��	����˾��Ȩ��������ȫ�������ĵ�λ�͸������ʹ�ñ���Ʒ��
//A��	����Ʒ��ͨ���Ϸ�������������ı���˾Ӳ����Ʒ���ʹ��ʱ��
//B��	��˽�����������������Ʒ�ͱ���Ʒ���κ�һ���֡�
//3��	�����ϵ�2�������������λ�͸�����Ҫʹ�ñ���Ʒʱ�������ñ���˾��������Ȩ��
//4��	�����Ȩ�ĵ�λ�͸��˲���ת�����õ���Ȩ��
//5��	ʹ�ñ���Ʒ����������ȫ��ⱾЭ����������ȫ�����ܡ�

#pragma pack(1)

typedef struct tagBUS_OP{
	BOOL		bEnHwOpBus;
	BOOL		bEnHwOpSetLinkFromVlm;
	int			nST;
	int			nTs;
	int			nToBusCh;
	int			nPlayST;
	int			nPlayTs;
	int			nPlayToBusCh;
	int			nSpeakerVlm;
	int			nTotListener;
	int	*		pnListenerCh;
	int			nFromSpeaker;
	int			nDefaultSpeakerVlm;
}BUS_OP, *PBUS_OP;

#pragma pack()

#ifdef _STBUS_CPP
	CRITICAL_SECTION	csSTBus;	// critical section of ST-bus
#else
	extern CRITICAL_SECTION	csSTBus;	// critical section of ST-bus
#endif

#ifdef __cplusplus
	extern "C" {
#endif

int WINAPI SsmGetChBusInfo(int ch, PBUS_OP* p);

#ifdef __cplusplus
	}
#endif
