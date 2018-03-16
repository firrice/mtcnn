#include <windows.h>
#include <string.h>
#include <string> 

//************************************
// Method:    Utf82Unicode将Utf8编码转为unicode编码
// FullName:  Utf82Unicode
// Access:    public 
// Returns:   char* 得到的unicode编码写符串
// Qualifier:
// Parameter: const char * utf 传入的utf编码
// Parameter: size_t * unicode_number 得到的Unicode写串长度
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
// Method:    Unicode2Utf8 unicode编码转为Utf8编码
// FullName:  Unicode2Utf8
// Access:    public 
// Returns:   char* 得么的utf编码字串
// Qualifier:
// Parameter: const char * unicode传入的Unicode编码
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
// Method:    IsTextUTF8判断一个字符是否Utf8编码
// FullName:  IsTextUTF8
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: const char * str字串
// Parameter: long length字符长度
//************************************
bool IsTextUTF8(const char* str,long length){
	int i;
	int nBytes=0;//UFT8可用1-6个字节编码,ASCII用一个字节
	unsigned char chr;
	bool bAllAscii=true; //如果全部都是ASCII, 说明不是UTF-8
	for(i=0;i<length;i++){
		chr= *(str+i);
		if( (chr&0x80) != 0 ) // 判断是否ASCII编码,如果不是,说明有可能是UTF-8,ASCII用7位编码,但用一个字节存,最高位标记为0,o0xxxxxxx
			bAllAscii= false;
		if(nBytes==0) //如果不是ASCII码,应该是多字节符,计算字节数
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
		else //多字节符的非首字节,应为 10xxxxxx
		{
			if( (chr&0xC0) != 0x80 )
			{ 
				return false;
			}
			nBytes--;
		}
	}
	if( nBytes > 0 ) //违返规则
	{
		return false;
	}
	if( bAllAscii ) //如果全部都是ASCII, 说明不是UTF-8
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

// UTF8编码转换到GBK编码
int UTF8ToGBK(unsigned char * lpUTF8Str,unsigned char * lpGBKStr,int nGBKStrLen)
{
 wchar_t * lpUnicodeStr = NULL;
 int nRetLen = 0;

 if(!lpUTF8Str)  //如果UTF8字符串为NULL则出错退出
  return 0;

 nRetLen = ::MultiByteToWideChar(CP_UTF8,0,(char *)lpUTF8Str,-1,NULL,NULL);  //获取转换到Unicode编码后所需要的字符空间长度
 lpUnicodeStr = new WCHAR[nRetLen + 1];  //为Unicode字符串空间
 nRetLen = ::MultiByteToWideChar(CP_UTF8,0,(char *)lpUTF8Str,-1,lpUnicodeStr,nRetLen);  //转换到Unicode编码
 if(!nRetLen)  //转换失败则出错退出
  return 0;

 nRetLen = ::WideCharToMultiByte(CP_ACP,0,lpUnicodeStr,-1,NULL,NULL,NULL,NULL);  //获取转换到GBK编码后所需要的字符空间长度

 if(!lpGBKStr)  //输出缓冲区为空则返回转换后需要的空间大小
 {
  if(lpUnicodeStr)
   delete []lpUnicodeStr;
  return nRetLen;
 }

 if(nGBKStrLen < nRetLen)  //如果输出缓冲区长度不够则退出
 {
  if(lpUnicodeStr)
   delete []lpUnicodeStr;
  return 0;
 }

 nRetLen = ::WideCharToMultiByte(CP_ACP,0,lpUnicodeStr,-1,(char *)lpGBKStr,nRetLen,NULL,NULL);  //转换到GBK编码

 if(lpUnicodeStr)
  delete []lpUnicodeStr;

 return nRetLen;
}
int UTF8ToGBKA(char *str_utf8,char *str)
{ int Lgbk = UTF8ToGBK((unsigned char*)str_utf8,0,0);
  Lgbk = UTF8ToGBK((unsigned char*)str_utf8,(unsigned char*)str,Lgbk);
  str[Lgbk]='\0';//结束符
  return Lgbk;
}

//GBK编码转换到UTF8编码
int GBKToUTF8(unsigned char * lpGBKStr,unsigned char * lpUTF8Str,int nUTF8StrLen)
{
 wchar_t * lpUnicodeStr = NULL;
 int nRetLen = 0;

 if(!lpGBKStr)  //如果GBK字符串为NULL则出错退出
  return 0;

 nRetLen = ::MultiByteToWideChar(CP_ACP,0,(char *)lpGBKStr,-1,NULL,NULL);  //获取转换到Unicode编码后所需要的字符空间长度
 lpUnicodeStr = new WCHAR[nRetLen + 1];  //为Unicode字符串空间
 nRetLen = ::MultiByteToWideChar(CP_ACP,0,(char *)lpGBKStr,-1,lpUnicodeStr,nRetLen);  //转换到Unicode编码
 if(!nRetLen)  //转换失败则出错退出
  return 0;

 nRetLen = ::WideCharToMultiByte(CP_UTF8,0,lpUnicodeStr,-1,NULL,0,NULL,NULL);  //获取转换到UTF8编码后所需要的字符空间长度

 if(!lpUTF8Str)  //输出缓冲区为空则返回转换后需要的空间大小
 {
  if(lpUnicodeStr)
   delete []lpUnicodeStr;
  return nRetLen;
 }

 if(nUTF8StrLen < nRetLen)  //如果输出缓冲区长度不够则退出
 {
  if(lpUnicodeStr)
   delete []lpUnicodeStr;
  return 0;
 }

 nRetLen = ::WideCharToMultiByte(CP_UTF8,0,lpUnicodeStr,-1,(char *)lpUTF8Str,nUTF8StrLen,NULL,NULL);  //转换到UTF8编码

 if(lpUnicodeStr)
  delete []lpUnicodeStr;

 return nRetLen;
}
int GBKToUTF8A(char *str_gbk,char *str)
{ int Lutf8 = GBKToUTF8((unsigned char*)str_gbk,0,0);
  Lutf8 = GBKToUTF8((unsigned char*)str_gbk,(unsigned char*)str,Lutf8);
//  str[Lutf8]='\0';//结束符
  return Lutf8+1;
}
