#ifdef  __cplusplus
extern "C" {
#endif

#ifdef OS_MSWIN
int		WINAPI	fPcm_Mp3ConvertALaw(char * szSourceFile, char * szTargetFile);
int		WINAPI	fPcm_AdpcmToMp3(char * szSourceFile, char * szTargetFile);
int		WINAPI	fPcm_Mp3ConvertULaw(char * szSourceFile, char * szTargetFile);
DWORD		WINAPI	fPcm_MemMp3ToPcm16(char *pSource, DWORD dwSourceSize, char *pTarget, DWORD dwTargetSize);
int		WINAPI	fPcm_ALawConvertMp3(char*szSourceFile, char*szTargetFile);
DWORD	WINAPI	fPcm_MemGSMToPcm16(char *pSource, DWORD dwSourceSize, char *pTarget, DWORD dwTargetSize);
int		WINAPI	fPcm_ULawConvertGSM(char*szSourceFile, char*szTargetFile);
int		WINAPI	fPcm_AdpcmToGsm(char * szSourceFile, char * szTargetFile);
#endif

int		WINAPI  fPcm_GetWaveFormat(char *szFileName);
int		WINAPI	fPcm_GetLastErrMsg(char * szErr);
int		WINAPI	fPcm_ALawConvertPcm16(char*szSourceFile, char*szTargetFile);
int		WINAPI	fPcm_ALawConvertPcm8(char*szSourceFile, char*szTargetFile);
DWORD		WINAPI	fPcm_MemAdpcmToPcm16(char * pSource, DWORD dwSourceSize, char * pTarget, DWORD dwTargetSize);
DWORD		WINAPI	fPcm_ConvertFskCID(LPSTR pFSKBuf, int nFskLen, LPSTR  pszCIDNumber, LPSTR pszTime, LPSTR pszName, int nMode);
DWORD   	WINAPI  fPcm_MemAdpcmToALAW(char * pSource, DWORD dwSourceSize, char * pTarget, DWORD dwTargetSize);
DWORD   	WINAPI  fPcm_MemAdpcmToULAW(char * pSource, DWORD dwSourceSize, char * pTarget, DWORD dwTargetSize);
DWORD		WINAPI	fPcm_MemPcm16ToAlaw(char * pSource, DWORD dwSourceSize, char * pTarget, DWORD dwTargetSize);
int		WINAPI	fPcm_AdpcmToAlaw(char *pSource, char * pTarget);
int		WINAPI	fPcm_GC8Convert(char * szSourceFile, char * szTargetFile, int nTargetFormat);
int     	WINAPI  fPcm_Vox6KTo8K(char * szSourceFile, char * szTargetFile);
int     	WINAPI  fPcm_Vox8KTo6K(char * szSourceFile, char * szTargetFile);
int		WINAPI	fPcm_Pcm16ConvertALaw(char * szSourceFile, char * szTargetFile);
int		WINAPI 	fPCM_AlawConvertGC8 (char*szSoureFile, char* szTargetFile);
int		WINAPI	fPcm_MemAlawToPcm8(char * szSource, int nSourceLen, char * szTarget, int nTargetLen);
int		WINAPI	fPcm_MemPcm8ToAlaw(char * szSource, int nSourceLen, char * szTarget, int nTargetLen);
int		WINAPI	fPcm_MemAlawToPcm16(char * szSource, int nSourceLen, char * szTarget, int nTargetLen);
int		WINAPI	fPcm_G729AConvert(char * szSourceFile, char * szTargetFile, int nTargetFormat);
		
int	    	WINAPI  fPcm_MemAlawToUlaw(char * szSource, int nSourceLen, char * szTarget, int nTargetLen);
int     	WINAPI  fPcm_MemUlawtoAlaw(char * szSource, int nSourceLen, char * szTarget, int nTargetLen);
int		WINAPI	fPcm_UlawToAlaw(char*szSourceFile, char*szTargetFile);
int		WINAPI	fPcm_AlawToUlaw(char*szSourceFile, char*szTargetFile);

void		WINAPI	fPcm_Close();


int		WINAPI fPCM_Pcm16ConvertG729A(char*szSoureFile, char* szTargetFile);  
 
#ifdef __cplusplus
}
#endif

