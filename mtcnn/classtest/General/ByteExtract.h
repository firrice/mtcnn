#ifndef __BYTE_EXTRACT_H__
#define __BYTE_EXTRACT_H__


//
int extract_int(char *p,int *x);

//֧������UTF8�����ַ���
//int extract_string_UTF8(void *buf,char *string);
//��֧�����Ľ����ַ���
int extract_string(void *buf,char *str);
//
int extract_float(char *p,float *x);
int extract_double(char *p,double *x);
//�������������ṹ��
int extract_struct(char *p,void *pStruct,int size);


int push_int(char *p,int i,int x);
int push_float(char *p,int i,float x);
int push_double(char *p,int i,double x);
int push_string(char *p,int i,char *str);
//int push_string_UTF8(char *p,int i,char *str);
//ѹ�ṹ�������
int push_struct(char *p,int i,void *pStruct,int size);

#endif
