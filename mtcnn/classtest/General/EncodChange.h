#ifndef __ENCODE_CHANGE_H__
#define __ENCODE_CHANGE_H__

//#include "windows.h"  
#include <string> 
//using namespace std;
//************************************
// Method:    Utf82Unicode��Utf8����תΪunicode����
// FullName:  Utf82Unicode
// Access:    public 
// Returns:   char* �õ���unicode����д����
// Qualifier:
// Parameter: const char * utf �����utf����
// Parameter: size_t * unicode_number �õ���Unicodeд������
//************************************
char* Utf82Unicode(const char* utf, size_t *unicode_number);

//************************************
// Method:    Unicode2Utf8 unicode����תΪUtf8����
// FullName:  Unicode2Utf8
// Access:    public 
// Returns:   char* ��ô��utf�����ִ�
// Qualifier:
// Parameter: const char * unicode�����Unicode����
//************************************
char* Unicode2Utf8(const char* unicode); 
//************************************
// Method:    IsTextUTF8�ж�һ���ַ��Ƿ�Utf8����
// FullName:  IsTextUTF8
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: const char * str�ִ�
// Parameter: long length�ַ�����
//************************************
bool IsTextUTF8(const char* str,long length);

std::string UTF8ToGBK(const std::string& strUTF8);
std::string GBKToUTF8(const std::string& strGBK);

// UTF8����ת����GBK����
int UTF8ToGBK(unsigned char * lpUTF8Str,unsigned char * lpGBKStr,int nGBKStrLen);
int UTF8ToGBKA(char *str_utf8,char *str);

//GBK����ת����UTF8����
int GBKToUTF8(unsigned char * lpGBKStr,unsigned char * lpUTF8Str,int nUTF8StrLen);
int GBKToUTF8A(char *str_gbk,char *str);

#endif
