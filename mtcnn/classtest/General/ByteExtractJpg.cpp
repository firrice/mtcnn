#include "../jpeg/jpgbase.h"//
//#include "WinGraph.h"

//
int extract_jpg(char *p,tImageJPGA *pJpg, int *pSize,int *res)
{
  int *pic_length = (int *)p;//ͼ���ܳ�
  char *jpg_buffer = (char *)(pic_length+1);//ͼ�񻺴�
  unsigned int *phead_code = (unsigned int*)jpg_buffer;
  //jpg_buffer[0]=0xff;jpg_buffer[1]=0xd8;//jpg_buffer[2]=0xff;jpg_buffer[3]=0xe0;
  if(res)
   *res = 0;
  int jpg_buffer_size = pic_length[0];
  if(jpg_buffer_size>0)
  { 
	if(phead_code[0] == 0xe0ffd8ff)//0xe0ffd8ff
	{ //��ѹjpg���浽acc_t
      LoadJPG(jpg_buffer,jpg_buffer_size,pJpg);
	  if(res)
		  *res = 1;
	} else
	{ //phead_code[0] = 0xe0ffd8ff;
	  /*char *jpg_copy=new char[4+jpg_buffer_size];
	  phead_code=(unsigned int *)jpg_copy;
	  phead_code[0] = 0xe0ffd8ff;
	  int i;
	  for(i=0;i<jpg_buffer_size;i++)
	  { jpg_copy[4+i]=jpg_buffer[i];
	  }
	  LoadJPG(jpg_copy,4+jpg_buffer_size,pJpg);
	  delete jpg_copy;*/
	  if(res)//��jpgͼƬ,ʧ��
		  *res = -1;
	}
  }//assert(res==0);//jpg_buffer_size==0��ͼ,����res==0
	 
  if(pSize)
	  pSize[0]=jpg_buffer_size;
  return sizeof(int)+jpg_buffer_size;
}
//��֪jpgͼƬ��С����ͼ������
int extract_jpg(char *p,int jpg_buffer_size,tImageJPGA *pJpg)
{
  if(jpg_buffer_size>0)
  { 
	//��ѹjpg���浽acc_t
    LoadJPG(p,jpg_buffer_size,pJpg);
  }
  return jpg_buffer_size;
}

/*int push_int(char *p,int i,int x)
{
  int *pi = (int *)(p+i);
  pi[0]=x;
  return sizeof(int);
}*/

