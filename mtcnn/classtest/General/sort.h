#ifndef _SORT_H_
#define _SORT_H_

//把p[low..high]一分为二
int Partion(int *p,int low,int high);
void QuickSort(int *p,int low,int high);
//对整数序列Buf[iLo,..,iHi]快速排序,原来在delphi里面看到的一种排序方法
void QuickSort2(int *Buf,int iLo,int iHi);
void QuickSort2(double *Buf,int iLo,int iHi);

void InsertSort(int *p,int n);
void InsertSort(float *p,int n);
//根据id[0..n-1]对p[0..n-1]数组排序
void InsertSort(float *p,int *id,int n);

//折半查找
int BinFind(int *p,int n,int c);

#endif
