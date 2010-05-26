//
// Encrypt.h
//

#if !defined(ENCRYPT_H__INCLUDED_)
#define ENCRYPT_H__INCLUDED_

#pragma once
AKM_BEGIN_VERSIONED_NAMESPACE_DECL

//////////////////////////////////////////////////////////////////////////

//MD5 ������
class AKM_Export CMD5
{
	//��������
private:
	unsigned long int				state[4];
	unsigned long int				count[2];
	unsigned char					buffer[64];
	unsigned char					PADDING[64];
	
	//��������
public:
	//���캯��
	CMD5() { MD5Init(); }
	
	//���ܺ���
public:
	//���ս��
	void MD5Final(unsigned char digest[16]);
	//������ֵ
	void MD5Update(unsigned char * input, unsigned int inputLen);
	
	//�ڲ�����
private:
	//��ʼ��
	void MD5Init();
	//��λ����
	void MD5_memset(unsigned char * output, int value, unsigned int len);
	//��������
	void MD5_memcpy(unsigned char * output, unsigned char * input, unsigned int len);
	//ת������
	void MD5Transform(unsigned long int state[4], unsigned char block[64]);
	//���뺯��
	void Encode(unsigned char * output, unsigned long int * input, unsigned int len);
	//���뺯��
	void Decode(unsigned long int *output, unsigned char * input, unsigned int len);
};

//////////////////////////////////////////////////////////////////////////

//MD5 ������ 
class AKM_Export CMD5Encrypt
{
public:
	//��������
	static void EncryptData(LPCTSTR pszSrcData, TCHAR szMD5Result[33]);
};

//////////////////////////////////////////////////////////////////////////

//��������
class AKM_Export CXOREncrypt
{
public:
	//��������
	static WORD EncryptData(LPCTSTR pszSrcData, LPTSTR pszEncrypData, WORD wSize);
	//�⿪����
	static WORD CrevasseData(LPCTSTR pszEncrypData, LPTSTR pszSrcData, WORD wSize);
};

//////////////////////////////////////////////////////////////////////////

AKM_END_VERSIONED_NAMESPACE_DECL

#endif // ENCRYPT_H__INCLUDED_