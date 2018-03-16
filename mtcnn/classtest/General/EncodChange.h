#ifndef __ENCODE_CHANGE_H__
#define __ENCODE_CHANGE_H__

//#include "windows.h"  
#include <string> 
//using namespace std;
//************************************
// Method:    Utf82Unicode将Utf8编码转为unicode编码
// FullName:  Utf82Unicode
// Access:    public 
// Returns:   char* 得到的unicode编码写符串
// Qualifier:
// Parameter: const char * utf 传入的utf编码
// Parameter: size_t * unicode_number 得到的Unicode写串长度
//************************************
char* Utf82Unicode(const char* utf, size_t *unicode_number);

//************************************
// Method:    Unicode2Utf8 unicode编码转为Utf8编码
// FullName:  Unicode2Utf8
// Access:    public 
// Returns:   char* 得么的utf编码字串
// Qualifier:
// Parameter: const char * unicode传入的Unicode编码
//************************************
char* Unicode2Utf8(const char* unicode); 
//************************************
// Method:    IsTextUTF8判断一个字符是否Utf8编码
// FullName:  IsTextUTF8
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: const char * str字串
// Parameter: long length字符长度
//************************************
bool IsTextUTF8(const char* str,long length);

std::string UTF8ToGBK(const std::string& strUTF8);
std::string GBKToUTF8(const std::string& strGBK);

// UTF8编码转换到GBK编码
int UTF8ToGBK(unsigned char * lpUTF8Str,unsigned char * lpGBKStr,int nGBKStrLen);
int UTF8ToGBKA(char *str_utf8,char *str);

//GBK编码转换到UTF8编码
int GBKToUTF8(unsigned char * lpGBKStr,unsigned char * lpUTF8Str,int nUTF8StrLen);
int GBKToUTF8A(char *str_gbk,char *str);

#endif
