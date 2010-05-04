#ifndef GDAT_H_
#define GDAT_H_

#include "windows.h"

#define klMIN1     1 
#define klMIN5     5
#define klMIN15    15
#define klMIN30    30
#define klHOUR     60
#define klDAY      240
#define klWEEK     1200
#define klMONTH    4800 
/////////////////////

#define KTLEN 8
const int ktarr[KTLEN] = {1,5,15,30,60,240,1200,4800};

#pragma pack(push,1)

#ifndef GLOBAL_T_
#define GLOBAL_T_

typedef struct tag_NTime
{
	unsigned short year;
	unsigned char month;
	unsigned char day;
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
}NTime;

///////////////////////////////////////////////////////////////////////////
//��ʷK������
typedef struct gdat_HISDAT
{
	NTime Time;			//ʱ��
	float Open;			//���̼�
	float High;			//��߼�
	float Low;			//��ͼ�
	float Close;		//���̼�
	DWORD Volume;       //�ɽ���
	float Amount;		//���	
}GHISDAT,*LPGHISDAT;

#endif

///////////////////////////////////////////////////////////////////////////
//���ɻ�����Ϣ
typedef struct gdat_STOCKINFO 
{
	char		Code[8];			// ֤ȯ����
	char        Name[9];			// ֤ȯ����

	float       ActiveGb;			// ��ͨ�ɱ�
	float       zgb;				// �ܹɱ�
	float		mgsy;				// ÿ������
	float		mgjzc;				// ÿ�ɾ��ʲ�
}GSTOCKINFO,*LPGSTOCKINFO;

///////////////////////////////////////////////////////////////////////////
//�ֱ���ϸ����
typedef struct gdat_TICKDAT
{ 
	NTime	Time;
	float   Now;            //�ּ�(Ԫ)
	DWORD   NowVol;         //����(��)
	float 	Amount;		    //�ɽ����(Ԫ)
	short   InOutFlag;      //������־
}GTICKDAT,*LPGTICKDAT;

////////////////////////////////////////////////////////////////////////////
//�̿�����
typedef struct gdat_KPDAT
{ 
	char  Code[8];	//��Ʊ����
	float Yclose;	//�������̼�
	float Open;		//���̼�
	float High;		//��߼�
	float Low;		//��ͼ�
	float Close;	//���̼�
	DWORD Volume;	//�ɽ���
	DWORD Inside;   // ����
	DWORD Outside;  // ����
	float Amount;	//�ɽ����
	float Buyp[5];	// ��������
	DWORD Buyv[5];	// ��Ӧ�������۵��������
	float Sellp[5];	// ���������
	DWORD Sellv[5];	// ��Ӧ��������۵��������
}GKPDAT,*LPGKPDAT;

//////////////////////////////////////////////////////////////////////////////
//��ʱͼ����
typedef struct gdat_Fs{
	NTime Time;				 //ʱ��
    float pricefs;			 //��ʱ��
    float priceav;			 //���߼�
    UINT  vol;				 //�ɽ���
    UINT  wBuyv;			 //ί����	
    UINT  wSellv;			 //ί����	
}GFSDAT, *LPGFSDAT;

/////////////////////////////////////////////////////////////////////////////
//��ʱ������
typedef struct gdat_Big{
    char  code[8];			 //��Ʊ����
	char  name[20];			 //��Ʊ����	
	float bfb;				 //�������ͨ�̵Ļ�����
	NTime Time;				 //ʱ��
	float price;			 //�۸�
    DWORD vol;				 //�ɽ���
    DWORD bs;				 //������־
}GBIGDAT, *LPGBIGDAT;

union gdat_Golbal
{
	LPGSTOCKINFO pStockInfo;
	LPGHISDAT pHisDat;
	LPGTICKDAT pTick;
	LPGKPDAT pkp;
	LPGBIGDAT pbig;
	LPGFSDAT pfs;
};

#pragma pack(pop)


//ϵͳ����
int WINAPI GdatOpen();		//��������
int WINAPI GdatClose();		//�ر�����

//ȫ������
int WINAPI GdatFlush(LPGKPDAT pkp);															//ˢ�µ�������
int WINAPI GdatMultiKp(LPGKPDAT pkp, char codearr[][100], int codenum);							//����̿�
int WINAPI GdatFlushKline(char* Code, int kltype, int reqno, BYTE nTQ, LPGHISDAT pHisDat);	//���ٻ�ȡ��k������
int WINAPI GdatBig(LPGBIGDAT pBigBs);													    //��


//���ɺ���
int WINAPI GdatStockInfo(char* Code, LPGSTOCKINFO pStockInfo);							//��ȡ��Ʊ������Ϣ
int WINAPI GdatKline(char* Code, int kltype, int reqno, BYTE nTQ, LPGHISDAT pHisDat);	//��ȡk������
int WINAPI GdatBs(char* Code, int reqno, LPGTICKDAT pTick);								//��ȡ��ϸ�ֱ�����
int WINAPI GdatHisBs(char* Code, NTime tim, LPGTICKDAT pTick);							//��ȡ��ʷ�ֱ�����
int WINAPI GdatKp(char* Code, LPGKPDAT pkp);											//��ȡ�̿�����
int WINAPI GdatFs(char* Code, LPGFSDAT pfs);											//��ȡ��ʱ����

#endif