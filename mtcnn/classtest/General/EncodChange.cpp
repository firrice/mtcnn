#include <windows.h>
#include <string.h>
#include <string> 

//************************************
// Method:    Utf82Unicode��Utf8����תΪunicode����
// FullName:  Utf82Unicode
// Access:    public 
// Returns:   char* �õ���unicode����д����
// Qualifier:
// Parameter: const char * utf �����utf����
// Parameter: size_t * unicode_number �õ���Unicodeд������
//************************************
char* Utf82Unicode(const char* utf, size_t *unicode_number)  
{   
	if(!utf || !strlen(utf))  
	{  
		*unicode_number = 0;  
		return NULL;  
	}  
	int dwUnicodeLen = MultiByteToWideChar(CP_UTF8,0,utf,-1,NULL,0);  
	size_t num = dwUnicodeLen*sizeof(wchar_t);  
	wchar_t *pwText = (wchar_t*)malloc(num);  
	memset(pwText,0,num);  
	MultiByteToWideChar(CP_UTF8,0,utf,-1,pwText,dwUnicodeLen);  
	*unicode_number = dwUnicodeLen - 1;  
	return (char*)pwText;  
}  

//************************************
// Method:    Unicode2Utf8 unicode����תΪUtf8����
// FullName:  Unicode2Utf8
// Access:    public 
// Returns:   char* ��ô��utf�����ִ�
// Qualifier:
// Parameter: const char * unicode�����Unicode����
//************************************
char* Unicode2Utf8(const char* unicode)  
{  
	int len;  
	len = WideCharToMultiByte(CP_UTF8, 0, (const wchar_t*)unicode, -1, NULL, 0, NULL, NULL);  
	char *szUtf8 = (char*)malloc(len + 1);  
	memset(szUtf8, 0, len + 1);  
	WideCharToMultiByte(CP_UTF8, 0, (const wchar_t*)unicode, -1, szUtf8, len, NULL,NULL);  
	return szUtf8;  
} 
//************************************
// Method:    IsTextUTF8�ж�һ���ַ��Ƿ�Utf8����
// FullName:  IsTextUTF8
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: const char * str�ִ�
// Parameter: long length�ַ�����
//************************************
bool IsTextUTF8(const char* str,long length){
	int i;
	int nBytes=0;//UFT8����1-6���ֽڱ���,ASCII��һ���ֽ�
	unsigned char chr;
	bool bAllAscii=true; //���ȫ������ASCII, ˵������UTF-8
	for(i=0;i<length;i++){
		chr= *(str+i);
		if( (chr&0x80) != 0 ) // �ж��Ƿ�ASCII����,�������,˵���п�����UTF-8,ASCII��7λ����,����һ���ֽڴ�,���λ���Ϊ0,o0xxxxxxx
			bAllAscii= false;
		if(nBytes==0) //�������ASCII��,Ӧ���Ƕ��ֽڷ�,�����ֽ���
		{
			if(chr>=0x80){
				if(chr>=0xFC&&chr<=0xFD)
					nBytes=6;
				else if(chr>=0xF8)
					nBytes=5;
				else if(chr>=0xF0)
					nBytes=4;
				else if(chr>=0xE0)
					nBytes=3;
				else if(chr>=0xC0)
					nBytes=2;
				else{
					return false; 
				}
				nBytes--;
			}
		}
		else //���ֽڷ��ķ����ֽ�,ӦΪ 10xxxxxx
		{
			if( (chr&0xC0) != 0x80 )
			{ 
				return false;
			}
			nBytes--;
		}
	}
	if( nBytes > 0 ) //Υ������
	{
		return false;
	}
	if( bAllAscii ) //���ȫ������ASCII, ˵������UTF-8
	{
		return false;
	}
	return true;
}
//using namespace std;
std::string UTF8ToGBK(const std::string& strUTF8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);

	unsigned short * wszGBK = new unsigned short[len + 1];

	memset(wszGBK, 0, len * 2 + 2);

	MultiByteToWideChar(CP_UTF8, 0, (LPCCH)strUTF8.c_str(), -1, (LPWSTR)wszGBK, len);

	len = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
	char *szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, (LPWSTR)wszGBK, -1, szGBK, len, NULL, NULL);
	//strUTF8 = szGBK;  
	std::string strTemp(szGBK);
	delete[]szGBK;
	delete[]wszGBK;
	return strTemp;
}

std::string GBKToUTF8(const std::string& strGBK)
{
	std::string strOutUTF8 = "";
	WCHAR * str1;
	int n = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);
	str1 = new WCHAR[n];
	MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, str1, n);
	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
	char * str2 = new char[n];
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
	strOutUTF8 = str2;
	delete[]str1;
	str1 = NULL;
	delete[]str2;
	str2 = NULL;
	return strOutUTF8;
}

// UTF8����ת����GBK����
int UTF8ToGBK(unsigned char * lpUTF8Str,unsigned char * lpGBKStr,int nGBKStrLen)
{
 wchar_t * lpUnicodeStr = NULL;
 int nRetLen = 0;

 if(!lpUTF8Str)  //���UTF8�ַ���ΪNULL������˳�
  return 0;

 nRetLen = ::MultiByteToWideChar(CP_UTF8,0,(char *)lpUTF8Str,-1,NULL,NULL);  //��ȡת����Unicode���������Ҫ���ַ��ռ䳤��
 lpUnicodeStr = new WCHAR[nRetLen + 1];  //ΪUnicode�ַ����ռ�
 nRetLen = ::MultiByteToWideChar(CP_UTF8,0,(char *)lpUTF8Str,-1,lpUnicodeStr,nRetLen);  //ת����Unicode����
 if(!nRetLen)  //ת��ʧ��������˳�
  return 0;

 nRetLen = ::WideCharToMultiByte(CP_ACP,0,lpUnicodeStr,-1,NULL,NULL,NULL,NULL);  //��ȡת����GBK���������Ҫ���ַ��ռ䳤��

 if(!lpGBKStr)  //���������Ϊ���򷵻�ת������Ҫ�Ŀռ��С
 {
  if(lpUnicodeStr)
   delete []lpUnicodeStr;
  return nRetLen;
 }

 if(nGBKStrLen < nRetLen)  //���������������Ȳ������˳�
 {
  if(lpUnicodeStr)
   delete []lpUnicodeStr;
  return 0;
 }

 nRetLen = ::WideCharToMultiByte(CP_ACP,0,lpUnicodeStr,-1,(char *)lpGBKStr,nRetLen,NULL,NULL);  //ת����GBK����

 if(lpUnicodeStr)
  delete []lpUnicodeStr;

 return nRetLen;
}
int UTF8ToGBKA(char *str_utf8,char *str)
{ int Lgbk = UTF8ToGBK((unsigned char*)str_utf8,0,0);
  Lgbk = UTF8ToGBK((unsigned char*)str_utf8,(unsigned char*)str,Lgbk);
  str[Lgbk]='\0';//������
  return Lgbk;
}

//GBK����ת����UTF8����
int GBKToUTF8(unsigned char * lpGBKStr,unsigned char * lpUTF8Str,int nUTF8StrLen)
{
 wchar_t * lpUnicodeStr = NULL;
 int nRetLen = 0;

 if(!lpGBKStr)  //���GBK�ַ���ΪNULL������˳�
  return 0;

 nRetLen = ::MultiByteToWideChar(CP_ACP,0,(char *)lpGBKStr,-1,NULL,NULL);  //��ȡת����Unicode���������Ҫ���ַ��ռ䳤��
 lpUnicodeStr = new WCHAR[nRetLen + 1];  //ΪUnicode�ַ����ռ�
 nRetLen = ::MultiByteToWideChar(CP_ACP,0,(char *)lpGBKStr,-1,lpUnicodeStr,nRetLen);  //ת����Unicode����
 if(!nRetLen)  //ת��ʧ��������˳�
  return 0;

 nRetLen = ::WideCharToMultiByte(CP_UTF8,0,lpUnicodeStr,-1,NULL,0,NULL,NULL);  //��ȡת����UTF8���������Ҫ���ַ��ռ䳤��

 if(!lpUTF8Str)  //���������Ϊ���򷵻�ת������Ҫ�Ŀռ��С
 {
  if(lpUnicodeStr)
   delete []lpUnicodeStr;
  return nRetLen;
 }

 if(nUTF8StrLen < nRetLen)  //���������������Ȳ������˳�
 {
  if(lpUnicodeStr)
   delete []lpUnicodeStr;
  return 0;
 }

 nRetLen = ::WideCharToMultiByte(CP_UTF8,0,lpUnicodeStr,-1,(char *)lpUTF8Str,nUTF8StrLen,NULL,NULL);  //ת����UTF8����

 if(lpUnicodeStr)
  delete []lpUnicodeStr;

 return nRetLen;
}
int GBKToUTF8A(char *str_gbk,char *str)
{ int Lutf8 = GBKToUTF8((unsigned char*)str_gbk,0,0);
  Lutf8 = GBKToUTF8((unsigned char*)str_gbk,(unsigned char*)str,Lutf8);
//  str[Lutf8]='\0';//������
  return Lutf8+1;
}
