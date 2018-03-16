#include <assert.h>

#include "sort.h"

int Partion(int *p,int low,int high)
{  
	while(low<high)
	{   
		int pivokey;
		pivokey=p[low];
		
		while(low<high&&p[high]>=pivokey)  --high;
		int temp;
		temp=p[low];
		p[low]=p[high];
		p[high]=temp;
		while(low<high&&p[low]<=pivokey)++low;
		temp=p[low];
		p[low]=p[high];
		p[high]=temp;
	}
	return low;
}
void QuickSort(int *p,int low,int high)
{ 
	if(low<high)
	{   
		int pivo;
		pivo=Partion(p, low,high);
		QuickSort(p,low,pivo-1);
		QuickSort(p,pivo+1,high);
	}
}
//对整数序列Buf[iLo,..,iHi]快速排序,原来在delphi里面看到的一种排序方法
void QuickSort2(int *Buf,int iLo,int iHi)
{ int Lo, Hi;
  int Midval;
  int SwapColor;
  Lo = iLo;Hi = iHi;
  Midval = Buf[(Lo + Hi)/2];
  do
  { while(Buf[Lo]>Midval)Lo++;//满足排序顺序的moves[Lo].Value就不管,直到找出一个低位
    while(Buf[Hi]<Midval)Hi--;//满足排序顺序的moves[Hi].Value就不管,直到找出一个高位
    if(Lo<= Hi)//发现高低一对不满足排序顺序的就
    { if(Lo< Hi)//交换两者顺序
	  { SwapColor=Buf[Lo];//< bug<
        Buf[Lo]=Buf[Hi];//< bug<
        Buf[Hi]=SwapColor;//< bug<
	  }
      Lo++;Hi--;
    }
  } while(Lo<=Hi);
  //到此为止,[iLo~Hi]>[Lo~iHi]
  if(Hi>iLo)QuickSort2(Buf,iLo,Hi);//对[iLo~Hi]        区间递归排序
  if(Lo<iHi)QuickSort2(Buf,Lo,iHi);//对        [Lo~iHi]区间递归排序
}


int Partion(double *p,int low,int high)
{  
	while(low<high)
	{   
		double pivokey;
		pivokey=p[low];
		
		while(low<high&&p[high]>=pivokey)  --high;
		double temp;
		temp=p[low];
		p[low]=p[high];
		p[high]=temp;
		while(low<high&&p[low]<=pivokey)++low;
		temp=p[low];
		p[low]=p[high];
		p[high]=temp;
	}
	return low;
}
void QuickSort(double *p,int low,int high)
{ 
	if(low<high)
	{   
		int pivo;
		pivo=Partion(p, low,high);
		QuickSort(p,low,pivo-1);
		QuickSort(p,pivo+1,high);
	}
}
//对整数序列Buf[iLo,..,iHi]快速排序,原来在delphi里面看到的一种排序方法
void QuickSort2(double *Buf,int iLo,int iHi)
{ int Lo, Hi;
  double Midval;
  double SwapVal;
  Lo = iLo;Hi = iHi;
  Midval = Buf[(Lo + Hi)/2];
  do
  { while(Buf[Lo]<Midval)Lo++;//满足排序顺序的moves[Lo].Value就不管,直到找出一个低位
    while(Buf[Hi]>Midval)Hi--;//满足排序顺序的moves[Hi].Value就不管,直到找出一个高位
    if(Lo<= Hi)//发现高低一对不满足排序顺序的就
    { if(Lo< Hi)//交换两者顺序
	  { SwapVal=Buf[Lo];//< bug<
        Buf[Lo]=Buf[Hi];//< bug<
        Buf[Hi]=SwapVal;//< bug<
	  }
      Lo++;Hi--;
    }
  } while(Lo<=Hi);
  //到此为止,[iLo~Hi]>[Lo~iHi]
  if(Hi>iLo)QuickSort2(Buf,iLo,Hi);//对[iLo~Hi]        区间递归排序
  if(Lo<iHi)QuickSort2(Buf,Lo,iHi);//对        [Lo~iHi]区间递归排序
}

void InsertSort(int *p,int n)
{ int i,j,temp;
  for(i=1;i<n;i++)
  { temp=p[i];
	j=i-1;
    while(j>=0 && temp<p[j])
	{ p[j+1]=p[j];j--;
	}
	p[j+1]=temp;
  }
}
void InsertSort(float *p,int n)
{ int i,j;
  float temp;
  for(i=1;i<n;i++)
  { temp=p[i];
    j=i-1;
    while(j>=0 && temp<p[j])
    { p[j+1]=p[j];j--;
    }
    p[j+1]=temp;
  }
}
//根据id[0..n-1]对p[0..n-1]数组排序
void InsertSort(float *p,int *id,int n)
{ 
    int i,j,tmp_id;
    float temp;
    for(i=0;i<n;i++)
        id[i] = i;
    for(i=1;i<n;i++)
    { 
        tmp_id = id[i];
        temp=p[tmp_id];
        j=i-1;
        while(j>=0 && temp>p[id[j]])
        { 
            id[j+1]=id[j];j--;
        }
        id[j+1]=tmp_id;
    }
    //验证
    for(i=1;i<n;i++)
    {
        assert(p[id[i-1]]>=p[id[i]]);
    }
}
//折半查找
int BinFind(int *p,int n,int c)
{ int low=0,high=n-1;
  while(low<=high)
  { int mid=(low+high)/2;
    if(p[mid]==c)
		return mid;
	else
	if(c>p[mid])
	{ low=mid+1;
	} else//>c
	{ high=mid-1;
	}
  }
  return -1;
}
