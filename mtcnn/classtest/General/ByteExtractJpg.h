#ifndef __EXTRACT_JPG_H__
#define __EXTRACT_JPG_H__

//#include "WinGraph.h"
#include "..\\CvPlus\\jpeg\\jpgbase.h"
//
int extract_jpg(char *p,tImageJPGA *pJpg, int *pSize,int *res=NULL);
//已知jpg图片大小解析图像数据
int extract_jpg(char *p,int jpg_buffer_size,tImageJPGA *pJpg);

#endif
