#include "windows.h"

typedef struct {
	int handle;			//file handle
	int width;			// pixels per line
	int high;			// total lines
	int bytes;			// bytes per line
}Bmp_Info;

//a block of memory  to hold a image bar
typedef struct{
	char  *ptr;
	int   width;
	int   high;
}Img_Block;

void WINAPI fBmp_GetVersion(char* pVersion);
void WINAPI fBmp_WriteHead(Bmp_Info *bmp_info);
int  WINAPI fBmp_OpenBmp(char *filename,Bmp_Info *bmp_info);
int  WINAPI fBmp_CreatBlock(int width,int high,Img_Block *img_block);
void WINAPI fBmp_ClearBlock(Img_Block *img_block);
void WINAPI fBmp_FreeBlock(Img_Block *img_block);
int  WINAPI fBmp_ReadImgFromBmp(int offset,Img_Block *img_block,Bmp_Info *bmp_info);
int  WINAPI fBmp_WriteImgToBmp(int offset,Img_Block *img_block,Bmp_Info *bmp_info);
int  WINAPI fBmp_CreatBmp(char *filename,int width,int high,Bmp_Info *bmp_info);
void WINAPI fBmp_CloseBmp(Bmp_Info *bmp_info);
void WINAPI fBmp_PrintText(int x,int y,char *text,Img_Block *img_block);
void WINAPI fBmp_PutDot(int x,int y,Img_Block *img_block);
void WINAPI fBmp_EraseDot(int x,int y,Img_Block *img_block);
void WINAPI fBmp_PutLine(int x1,int y1,int x2,int y2,Img_Block *img_block);
void WINAPI fBmp_PutText(int x,int y,char *text,int size,Img_Block *img_block);
void WINAPI fBmp_PutDotB(int x,int y,Bmp_Info *bmp_info);
void WINAPI fBmp_EraseDotB(int x,int y,Bmp_Info *bmp_info);
void WINAPI fBmp_PutLineB(int x1,int y1,int x2,int y2,Bmp_Info *bmp_info);
void WINAPI fBmp_PutTextB(int x,int y,char *text,int size,Bmp_Info *bmp_info);
void WINAPI fBmp_PrintTextB(int x,int y,char *text,Bmp_Info *bmp_info);
void WINAPI fBmp_SetFaxConvertMode(int width,int high,int fax_size,int is_fine,int top,int left,int end);
int  WINAPI fBmp_ConvertBlockToFax(Img_Block *img_block,char *faxname);
int  WINAPI fBmp_ConvertFaxToBmp(char * faxname,char * bmpsname);
void WINAPI fBmp_GetErrMsg(char *buf);
int  WINAPI fBmp_GetErrCode(void);
void WINAPI fBmp_Getxy(int* x,int* y);
int  WINAPI fBmp_test(void);
int  WINAPI fBmp_ConvertPrnToFax(char *prnfile,char *faxfile);
int  WINAPI fBmp_ConvertPrnToFax_Asyn(char *prnfile,char *faxfile);
int  WINAPI fBmp_ConvertTxtToFax(char * lptxtfilename,char * lpfaxfilename,int is_fine,int top,int left,int bottom,int right);
int  WINAPI fBmp_ConvertTxtToFax_Asyn(char * lptxtfilename,char * lpfaxfilename,int is_fine,int top,int left,int bottom,int right);
BOOL WINAPI fBmp_GetPrnToFaxState(LPDWORD lpRetCode, LPDWORD lpProcessedPageNum);
BOOL WINAPI fBmp_GetTxtToFaxState(LPDWORD lpRetCode, LPDWORD lpProcessedPageNum);
int  WINAPI fBmp_GetFaxMode(LPSTR lpFileName);
int  WINAPI fBmp_ConvertBlockToFaxEx(Img_Block *img_block,char *faxname);
int	 WINAPI	fBmp_GetFileAllPage(char * filename);

int WINAPI fBmp_CutTif(char * szTifName, int nHeight);
int WINAPI fBmp_AddTxtToTif(char * szTifName, char *szFaxFrom, char *szFaxTo, char *szFaxSubject, char *szDataTime, char * szTargetFile, DWORD dwReserve);
int WINAPI fBmp_SetHeaderFormat(int nRow, int nFromX, int nFromY, char*szFrom, 
										int nSubX, int nSubY, char*szSubject,
										int nToX, int nToY, char *szTo,
										int nTimeX, int nTimeY, char *szTime);
int WINAPI fBmp_AddTxtToTif_Big(char * szTifName, char *szFaxFrom, char *szFaxTo, char *szFaxSubject, char *szDataTime, char * szTargetFile, DWORD dwReserve);
int WINAPI fBmp_UniteTif(char * szHeadTif, char * szSourceTif, char *szTargetTif, DWORD dwReserve);
int WINAPI fBmp_CutTifHeader(char * szSource, char * szTarget, int nHeight, int nPageNo, DWORD dwReserve);
int WINAPI fBmp_ValidateFaxFile(char * szFile);