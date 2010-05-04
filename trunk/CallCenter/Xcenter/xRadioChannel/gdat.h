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
//历史K线数据
typedef struct gdat_HISDAT
{
	NTime Time;			//时间
	float Open;			//开盘价
	float High;			//最高价
	float Low;			//最低价
	float Close;		//收盘价
	DWORD Volume;       //成交量
	float Amount;		//金额	
}GHISDAT,*LPGHISDAT;

#endif

///////////////////////////////////////////////////////////////////////////
//个股基础信息
typedef struct gdat_STOCKINFO 
{
	char		Code[8];			// 证券编码
	char        Name[9];			// 证券名称

	float       ActiveGb;			// 流通股本
	float       zgb;				// 总股本
	float		mgsy;				// 每股收益
	float		mgjzc;				// 每股净资产
}GSTOCKINFO,*LPGSTOCKINFO;

///////////////////////////////////////////////////////////////////////////
//分比明细数据
typedef struct gdat_TICKDAT
{ 
	NTime	Time;
	float   Now;            //现价(元)
	DWORD   NowVol;         //现手(手)
	float 	Amount;		    //成交金额(元)
	short   InOutFlag;      //买卖标志
}GTICKDAT,*LPGTICKDAT;

////////////////////////////////////////////////////////////////////////////
//盘口数据
typedef struct gdat_KPDAT
{ 
	char  Code[8];	//股票代码
	float Yclose;	//昨日收盘价
	float Open;		//开盘价
	float High;		//最高价
	float Low;		//最低价
	float Close;	//收盘价
	DWORD Volume;	//成交量
	DWORD Inside;   // 内盘
	DWORD Outside;  // 外盘
	float Amount;	//成交金额
	float Buyp[5];	// 五个叫买价
	DWORD Buyv[5];	// 对应五个叫买价的五个买盘
	float Sellp[5];	// 五个叫卖价
	DWORD Sellv[5];	// 对应五个叫卖价的五个卖盘
}GKPDAT,*LPGKPDAT;

//////////////////////////////////////////////////////////////////////////////
//分时图数据
typedef struct gdat_Fs{
	NTime Time;				 //时间
    float pricefs;			 //分时价
    float priceav;			 //均线价
    UINT  vol;				 //成交量
    UINT  wBuyv;			 //委买量	
    UINT  wSellv;			 //委卖量	
}GFSDAT, *LPGFSDAT;

/////////////////////////////////////////////////////////////////////////////
//即时大单数据
typedef struct gdat_Big{
    char  code[8];			 //股票编码
	char  name[20];			 //股票名称	
	float bfb;				 //相对于流通盘的换手率
	NTime Time;				 //时间
	float price;			 //价格
    DWORD vol;				 //成交量
    DWORD bs;				 //买卖标志
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


//系统函数
int WINAPI GdatOpen();		//连接引擎
int WINAPI GdatClose();		//关闭引擎

//全场函数
int WINAPI GdatFlush(LPGKPDAT pkp);															//刷新当天数据
int WINAPI GdatMultiKp(LPGKPDAT pkp, char codearr[][100], int codenum);							//多股盘口
int WINAPI GdatFlushKline(char* Code, int kltype, int reqno, BYTE nTQ, LPGHISDAT pHisDat);	//快速获取日k线数据
int WINAPI GdatBig(LPGBIGDAT pBigBs);													    //大单


//个股函数
int WINAPI GdatStockInfo(char* Code, LPGSTOCKINFO pStockInfo);							//获取股票基本信息
int WINAPI GdatKline(char* Code, int kltype, int reqno, BYTE nTQ, LPGHISDAT pHisDat);	//获取k线数据
int WINAPI GdatBs(char* Code, int reqno, LPGTICKDAT pTick);								//获取明细分笔数据
int WINAPI GdatHisBs(char* Code, NTime tim, LPGTICKDAT pTick);							//获取历史分笔数据
int WINAPI GdatKp(char* Code, LPGKPDAT pkp);											//获取盘口数据
int WINAPI GdatFs(char* Code, LPGFSDAT pfs);											//获取分时数据

#endif