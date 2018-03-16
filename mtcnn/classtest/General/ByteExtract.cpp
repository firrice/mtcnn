#include <string.h>
//#include "EncodChange.h"
//
int extract_int(char *p,int *x)
{
  x[0]=((int *)p)[0];
  return sizeof(int);
}
//
//不支持中文解析字符串
int extract_string(void *buf,char *str)
{ int *pL = (int *)buf;
  int len = pL[0];
  memcpy(str,(char*)(pL+1),len);
  str[len]='\0';
  return sizeof(int)+len;
}
//UTF8支持中文解析字符串
/*int extract_string_UTF8(void *buf,char *str)
{ int *pL = (int *)buf;
  int len = pL[0];
#if 0
  string strutf8 = UTF8ToGBK((char*)(pL+1));
  int Lutf8 = strutf8.length();
  memcpy(str,strutf8.c_str(),Lutf8);
  str[Lutf8-1]='\0';//结束符
#else
  char *utf8_str = new char[len+1];
  memcpy(utf8_str,(char*)(pL+1),len);
  utf8_str[len]='\0';
  //
  //int Lgbk = UTF8ToGBK((unsigned char*)utf8_str,0,0);
  int Lgbk = UTF8ToGBK((unsigned char*)utf8_str,(unsigned char*)str,512);//Lgbk
  str[Lgbk]='\0';//结束符
  //
  delete utf8_str;
#endif
  return sizeof(int)+len;
}*/
//
int extract_float(char *p,float *x)
{
  x[0]=((float *)p)[0];
  return sizeof(float);
}
//
int extract_double(char *p,double *x)
{
  x[0]=((double *)p)[0];
  return sizeof(double);
}
//数据流解析出结构体
int extract_struct(char *p,void *pStruct,int size)
{
  memcpy(pStruct,p,size);
  return size;
}

//
int push_int(char *p,int i,int x)
{
  int *pi = (int *)(p+i);
  pi[0]=x;
  return sizeof(int);
}
int push_float(char *p,int i,float x)
{
  float *pi = (float *)(p+i);
  pi[0]=x;
  return sizeof(float);
}
int push_double(char *p,int i,double x)
{
  double *pi = (double *)(p+i);
  pi[0]=x;
  return sizeof(double);
}
int push_string(char *p,int i,char *str)
{
  int slen = strlen(str);
  int *pL = (int *)(p+i);
  pL[0] = slen;
  strcpy((char *)(pL+1),str);
  return sizeof(int)+slen;
}
/*int push_string_UTF8(char *p,int i,char *str)
{
#if 1
  string strutf8 = GBKToUTF8(str);
  int slen = strlen(strutf8.c_str());
  int *pL = (int *)(p+i);
  pL[0] = slen;
  strcpy((char *)(pL+1),strutf8.c_str());
#else
  int *pL = (int *)(p+i);
  int slen = GBKToUTF8A(str,(char *)(pL+1));
  pL[0] = slen;
#endif
  return sizeof(int)+slen;
}*/
//压结构体进入流
int push_struct(char *p,int i,void *pStruct,int size)
{
  memcpy(p+i,pStruct,size);
  return size;
}
