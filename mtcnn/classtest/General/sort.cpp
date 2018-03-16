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
//����������Buf[iLo,..,iHi]��������,ԭ����delphi���濴����һ�����򷽷�
void QuickSort2(int *Buf,int iLo,int iHi)
{ int Lo, Hi;
  int Midval;
  int SwapColor;
  Lo = iLo;Hi = iHi;
  Midval = Buf[(Lo + Hi)/2];
  do
  { while(Buf[Lo]>Midval)Lo++;//��������˳���moves[Lo].Value�Ͳ���,ֱ���ҳ�һ����λ
    while(Buf[Hi]<Midval)Hi--;//��������˳���moves[Hi].Value�Ͳ���,ֱ���ҳ�һ����λ
    if(Lo<= Hi)//���ָߵ�һ�Բ���������˳��ľ�
    { if(Lo< Hi)//��������˳��
	  { SwapColor=Buf[Lo];//< bug<
        Buf[Lo]=Buf[Hi];//< bug<
        Buf[Hi]=SwapColor;//< bug<
	  }
      Lo++;Hi--;
    }
  } while(Lo<=Hi);
  //����Ϊֹ,[iLo~Hi]>[Lo~iHi]
  if(Hi>iLo)QuickSort2(Buf,iLo,Hi);//��[iLo~Hi]        ����ݹ�����
  if(Lo<iHi)QuickSort2(Buf,Lo,iHi);//��        [Lo~iHi]����ݹ�����
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
//����������Buf[iLo,..,iHi]��������,ԭ����delphi���濴����һ�����򷽷�
void QuickSort2(double *Buf,int iLo,int iHi)
{ int Lo, Hi;
  double Midval;
  double SwapVal;
  Lo = iLo;Hi = iHi;
  Midval = Buf[(Lo + Hi)/2];
  do
  { while(Buf[Lo]<Midval)Lo++;//��������˳���moves[Lo].Value�Ͳ���,ֱ���ҳ�һ����λ
    while(Buf[Hi]>Midval)Hi--;//��������˳���moves[Hi].Value�Ͳ���,ֱ���ҳ�һ����λ
    if(Lo<= Hi)//���ָߵ�һ�Բ���������˳��ľ�
    { if(Lo< Hi)//��������˳��
	  { SwapVal=Buf[Lo];//< bug<
        Buf[Lo]=Buf[Hi];//< bug<
        Buf[Hi]=SwapVal;//< bug<
	  }
      Lo++;Hi--;
    }
  } while(Lo<=Hi);
  //����Ϊֹ,[iLo~Hi]>[Lo~iHi]
  if(Hi>iLo)QuickSort2(Buf,iLo,Hi);//��[iLo~Hi]        ����ݹ�����
  if(Lo<iHi)QuickSort2(Buf,Lo,iHi);//��        [Lo~iHi]����ݹ�����
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
//����id[0..n-1]��p[0..n-1]��������
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
    //��֤
    for(i=1;i<n;i++)
    {
        assert(p[id[i-1]]>=p[id[i]]);
    }
}
//�۰����
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
