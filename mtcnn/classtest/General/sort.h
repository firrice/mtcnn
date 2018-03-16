#ifndef _SORT_H_
#define _SORT_H_

//��p[low..high]һ��Ϊ��
int Partion(int *p,int low,int high);
void QuickSort(int *p,int low,int high);
//����������Buf[iLo,..,iHi]��������,ԭ����delphi���濴����һ�����򷽷�
void QuickSort2(int *Buf,int iLo,int iHi);
void QuickSort2(double *Buf,int iLo,int iHi);

void InsertSort(int *p,int n);
void InsertSort(float *p,int n);
//����id[0..n-1]��p[0..n-1]��������
void InsertSort(float *p,int *id,int n);

//�۰����
int BinFind(int *p,int n,int c);

#endif
