#include <stdio.h>
#include <windows.h>
#include <assert.h>
#include "FileProc.h"
#include "MyString.h"

#pragma comment(lib,"comdlg32.lib")
#pragma comment(lib,"user32.lib")

//把文件导入到一个缓存buffer里面
int File2Buffer(char *fname,char *buffer)
{ FILE *fp=fopen(fname,"rb");
  if(fp)
  { int read_n;
    char *pOffset=buffer;
	while(read_n=fread(pOffset,1,BUFFER_SIZE,fp))
    { pOffset+=read_n;
    }
	int file_size=pOffset-buffer;
	fclose(fp);
	return file_size;
  } else return 0;
}
//把缓存buffer[0..file_size-1]里面的内容保存到文件
int Buffer2File(char *buffer,int size,char *fname)
{ FILE *fp=fopen(fname,"wb");
  if(fp)
  { int write_n;
	char *pOffset=buffer;
	while(1)
	{ int left_size = size-(pOffset-buffer);
	  int write_ct = BUFFER_SIZE < left_size ? BUFFER_SIZE : left_size;//MIN(size-(pOffset-buffer),BUFFER_SIZE);
	  write_n=fwrite(pOffset,1,write_ct,fp);
	  pOffset+=write_n;
	  if(pOffset>=buffer+size)break;
	}
	int write_size = pOffset-buffer;
	assert(write_size==size);
	fclose(fp);
	return write_size;
  } else return 0;
}
//获取文件大小
unsigned long get_file_size(const char *path)  
{  
    unsigned long filesize = -1;  
    FILE *fp;  
    fp = fopen(path, "r");  
    if(fp == NULL)  
        return filesize;  
    fseek(fp, 0L, SEEK_END);  
    filesize = ftell(fp);  
    fclose(fp);  
    return filesize;  
}


//担心由于命名空间重复造成错误另外建立了一个函数
BOOL GetOpenFileName2(LPSTR szName,LPSTR filter,LPSTR dir,char *title)
{ return GetOpenFileName(szName,filter,dir,title);
}

//打开文件对话框
BOOL GetOpenFileName(LPSTR szName,LPSTR filter,LPSTR dir,char *title)
{	OPENFILENAMEA ofn;
    ZeroMemory(&ofn,sizeof(OPENFILENAME));
	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner         = NULL;//ghApp;
	ofn.lpstrFilter       = NULL;
	//ofn.lpstrFilter       = "Video files (*.mpg; *.mpeg; *.mp4)\0*.mpg; *.mpeg; *.mp4\0\0";
	ofn.lpstrFilter=filter;// "Chess files (*.cnd)\0*.cnd\0\0";
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex      = 1;
	*szName = 0;
	ofn.lpstrFile         = szName;
	ofn.nMaxFile          = MAX_PATH;
	ofn.lpstrInitialDir=dir;//   = NULL;
	ofn.lpstrTitle        = title;
	ofn.lpstrFileTitle    = NULL;
	//ofn.lpstrDefExt       = "MPG";
	ofn.lpstrDefExt       = "CND";
	//ofn.Flags             = OFN_FILEMUSTEXIST|OFN_READONLY|OFN_PATHMUSTEXIST;
	ofn.Flags             = OFN_PATHMUSTEXIST;
//bn(GetOpenFileName((LPOPENFILENAME)&ofn));
	return ::GetOpenFileName((LPOPENFILENAME)&ofn);

} // GetClipFileName

//对整数序列Buf[iLo,..,iHi]快速排序,原来在delphi里面看到的一种排序方法
void QuickSort2(std::vector <FileVector> &files, int iLo, int iHi, int order)
{ int Lo, Hi;
  Lo = iLo;Hi = iHi;
  FileVector Midval = files[(Lo + Hi)/2];
  if(order==0)//顺序
  { do
	{ while(strcmp(files[Lo].name,Midval.name)<0)Lo++;//满足排序顺序的moves[Lo].Value就不管,直到找出一个低位
      while(strcmp(files[Hi].name,Midval.name)>0)Hi--;//满足排序顺序的moves[Hi].Value就不管,直到找出一个高位
      if(Lo<= Hi)//发现高低一对不满足排序顺序的就
	  { if(Lo< Hi)//交换两者顺序
	    { FileVector SwapColor=files[Lo];//< bug<
          files[Lo]=files[Hi];//< bug<
          files[Hi]=SwapColor;//< bug<
	    }
        Lo++;Hi--;
	  }
	} while(Lo<=Hi);
  } else//逆序
  {
    do
	{ while(strcmp(files[Lo].name,Midval.name)>0)Lo++;//满足排序顺序的moves[Lo].Value就不管,直到找出一个低位
      while(strcmp(files[Hi].name,Midval.name)<0)Hi--;//满足排序顺序的moves[Hi].Value就不管,直到找出一个高位
      if(Lo<= Hi)//发现高低一对不满足排序顺序的就
	  { if(Lo< Hi)//交换两者顺序
	    { FileVector SwapColor=files[Lo];//< bug<
          files[Lo]=files[Hi];//< bug<
          files[Hi]=SwapColor;//< bug<
	    }
        Lo++;Hi--;
	  }
	} while(Lo<=Hi);
  }
  //到此为止,[iLo~Hi]>[Lo~iHi]
  if(Hi>iLo)QuickSort2(files,iLo,Hi,order);//对[iLo~Hi]        区间递归排序
  if(Lo<iHi)QuickSort2(files,Lo,iHi,order);//对        [Lo~iHi]区间递归排序
}
//打开文件对话框
BOOL GetOpenFileNameMult(std::vector <FileVector> &files, LPSTR filter, LPSTR init_dir, char *out_path, int order, char *title)
{	OPENFILENAMEA ofn;
    ZeroMemory(&ofn,sizeof(OPENFILENAME));
	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner         = NULL;//ghApp;
	ofn.lpstrFilter       = NULL;
	//ofn.lpstrFilter       = "Video files (*.mpg; *.mpeg; *.mp4)\0*.mpg; *.mpeg; *.mp4\0\0";
	ofn.lpstrFilter=filter;// "Chess files (*.cnd)\0*.cnd\0\0";
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex      = 1;
	char szName[256*MAX_PATH];
	*szName = 0;
	ofn.lpstrFile         = szName;
	ofn.nMaxFile          = sizeof(szName);//MAX_PATH;
	ofn.lpstrInitialDir=init_dir;//   = NULL;
	ofn.lpstrTitle        = title;
	ofn.lpstrFileTitle    = NULL;
	//ofn.lpstrDefExt       = "MPG";
	ofn.lpstrDefExt       = "CND";
	//ofn.Flags             = OFN_FILEMUSTEXIST|OFN_READONLY|OFN_PATHMUSTEXIST;
	ofn.Flags             = OFN_EXPLORER | OFN_ALLOWMULTISELECT|OFN_PATHMUSTEXIST;
	ofn.lpstrFile[0] = '\0';
	int res =  ::GetOpenFileNameA((LPOPENFILENAMEA)&ofn);
	//
	char *p = szName + ofn.nFileOffset; //把指针移到第一个文件
	while( *p )
	{ FileVector a;
	  strcpy(a.name,p);
	  files.push_back(a);
      //lstrcat(szFileName, szPath);  //给文件名加上路径  
      //lstrcat(szFileName, p);    //加上文件名  
      //lstrcat(szFileName, TEXT("\n")); //换行   
      p += lstrlenA(p) +1;     //移至下一个文件
	}
	//
	if(files.size()>0)
	if(order>=0)//-1是不排序,0是顺序,1是逆序
	{ QuickSort2(files,0,files.size()-1,order);
#ifdef _DEBUG
	  int k;
	  for(k=0;k<(int)files.size()-1;k++)
	  { if(order==0)//顺序
		 assert(strcmp(files[k].name,files[k+1].name)<=0);//顺序验证
	    else//逆序
		 assert(strcmp(files[k].name,files[k+1].name)>=0);//逆序验证
	  }
	  for(k=0;k<(int)files.size();k++)
	  { printf("%s\n",files[k].name);
	  }
#endif
	}
	//
	if(out_path)//输出路径out_path
	{ if(files.size()>1)
	  { 
	    strcpy(out_path,szName);//
	    strcat(out_path,"\\");
	  } else
	  { ExtractFileName(szName,out_path,NULL);
	  }
	}
	//
	return res;

} // GetClipFileName

//保存文件对话框
/*BOOL GetSaveFileName(LPSTR szName,LPSTR filter,LPSTR dir)
{	OPENFILENAME ofn;
    ZeroMemory(&ofn,sizeof(SAVEFILENAME));
	ofn.lStructSize       = sizeof(SAVEFILENAME);
	ofn.hwndOwner         = NULL;//ghApp;
	ofn.lpstrFilter       = NULL;
	//ofn.lpstrFilter       = "Video files (*.mpg; *.mpeg; *.mp4)\0*.mpg; *.mpeg; *.mp4\0\0";
	ofn.lpstrFilter=filter;// "Chess files (*.cnd)\0*.cnd\0\0";
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex      = 1;
	*szName = 0;
	ofn.lpstrFile         = szName;
	ofn.nMaxFile          = MAX_PATH;
	ofn.lpstrInitialDir=dir;//   = NULL;
	ofn.lpstrTitle        = NULL;
	ofn.lpstrFileTitle    = NULL;
	//ofn.lpstrDefExt       = "MPG";
	ofn.lpstrDefExt       = "CND";
	//ofn.Flags             = OFN_FILEMUSTEXIST|OFN_READONLY|OFN_PATHMUSTEXIST;
	ofn.Flags             = OFN_PATHMUSTEXIST;
//bn(GetOpenFileName((LPOPENFILENAME)&ofn));
	return GetOpenFileName((LPOPENFILENAME)&ofn);
} // GetClipFileName*/
BOOL GetSaveFileName2(LPSTR szName,LPSTR filter,LPSTR dir)
{	OPENFILENAMEA ofn;
    ZeroMemory(&ofn,sizeof(OPENFILENAME));
	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner         = NULL;//ghApp;
	ofn.lpstrFilter       = NULL;
	//ofn.lpstrFilter       = "Video files (*.mpg; *.mpeg; *.mp4)\0*.mpg; *.mpeg; *.mp4\0\0";
	ofn.lpstrFilter=filter;// "Chess files (*.cnd)\0*.cnd\0\0";
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex      = 1;
	*szName = 0;
	ofn.lpstrFile         = szName;
	ofn.nMaxFile          = MAX_PATH;
	ofn.lpstrInitialDir=dir;//   = NULL;
	ofn.lpstrTitle        = NULL;
	ofn.lpstrFileTitle    = NULL;
	//ofn.lpstrDefExt       = "MPG";
	ofn.lpstrDefExt       = "CND";
	//ofn.Flags             = OFN_FILEMUSTEXIST|OFN_READONLY|OFN_PATHMUSTEXIST;
	ofn.Flags             = OFN_PATHMUSTEXIST;
//bn(GetOpenFileName((LPOPENFILENAME)&ofn));
	return ::GetSaveFileName((LPOPENFILENAME)&ofn);
} 
//察看文件是否存在并得到文件信息FindFileData
int FileExist(LPSTR szName,WIN32_FIND_DATAA &FindFileData)
{ //WIN32_FIND_DATAA FindFileData;
  HANDLE hFind;
  hFind = FindFirstFileA(szName,&FindFileData);
  return (hFind != INVALID_HANDLE_VALUE);
}
int FileExist(LPSTR szName,WIN32_FIND_DATAA *FindFileData)
{ char fname[512];
  strcpy(fname,szName);
  int len = strlen(szName);
  while (szName[len - 1] == '\\' || szName[len - 1] == '/')len--;
  fname[len]='\0';
  
  WIN32_FIND_DATAA _FindFileData;
  HANDLE hFind;
  hFind = FindFirstFileA(fname,FindFileData ? FindFileData : &_FindFileData);
  return (hFind != INVALID_HANDLE_VALUE);
}
int FileExist_slash(LPSTR szName,WIN32_FIND_DATAA *FindFileData)
{
	WIN32_FIND_DATAA _FindFileData;
	HANDLE hFind;
	char szNameCut[MAX_PATH];
	strcpy(szNameCut,szName);
	szNameCut[strlen(szName)-1] = '\0';
	hFind = FindFirstFileA(szNameCut,FindFileData ? FindFileData : &_FindFileData);
	return (hFind != INVALID_HANDLE_VALUE);
}
///////////////////
//在文件fp里扫描一行记录到Line数组
char* ScanALine(FILE *fp,char *Line)
{ int count=0;
  char c;
  while(1)
  { c=fgetc(fp);
    if(c=='\n')
	{ Line[count]='\0';
	  break;
	}
    Line[count]=c;
	count++;
  }
  return Line;
}
//扫描一个文本文件把每一行字符串记录下来
int ScanAFile(char *file_name,char Lines[][MAX_FILE_LINE_LEN])
{ FILE *fp=fopen(file_name,"r");
  if(fp)
  { int count=0;//,read_num;
    char *p;
    do
	{ //read_num=fscanf(fp,"%s",Lines[count]);
	  p=fgets(Lines[count],MAX_FILE_LINE_LEN,fp);
	  count++;
	} while(p);
	//
	fclose(fp);
	return count;
  } return -1;
}
//以下是获得当前路径的API函数调用方法(注意要#include<windows.h>)
//char CurDirectory[MAX_STRING_LEN];
//GetCurrentDirectory(MAX_STRING_LEN,CurDirectory);

//以下是判断文件是否存在的API函数用法
//char szName[129];
//if(GetOpenFileName(szName,//输出文件名
//                   "Chess files (*.cnd)\0*.cnd\0\0",//文件过滤器
//				   NULL))//文件目录
//{ WIN32_FIND_DATAA FindFileData;
//  if(!FileExist(szName,FindFileData))SaveBoard(szName);//文件不存在才保存
//   else
//   if(bn("文件已经存在是否覆盖?","警告!"))SaveBoard(szName);
//}

//遍历得到路径path_name下,扩展名为filterExt的所有文件files[0..nCount-1],返回文件个数nCount
//不想用控件,不要MFC那一套,那你就得罗嗦点了://"D:\\*.txt"
int BrowPathFiles_old(char *path_name,char *filterExt,char files[][MAX_PATH])
{ char path_ext_name[MAX_PATH];
  //先拼接得到带扩展名的路径名path_ext_name字符串
  strcpy(path_ext_name,path_name);strcat(path_ext_name,filterExt);
  WIN32_FIND_DATAA FileData; 
  HANDLE hSearch;
  int nCount=0;
  BOOL fFinished = FALSE;
  // Start searching for path_ext_name files in the current directory. 
  hSearch = FindFirstFileA(path_ext_name, &FileData);//先搜索本路径下的第一个文件FileData.cFileName
  if (hSearch != INVALID_HANDLE_VALUE)
  { strcpy(files[nCount],FileData.cFileName);nCount++;//添加第一个文件
	while (FindNextFileA(hSearch, &FileData))
	{ strcpy(files[nCount],FileData.cFileName);nCount++;}
    if (GetLastError() != ERROR_NO_MORE_FILES){ MessageBoxA(NULL,"Couldn't find next file.","你好！",MB_OK);}
    // Close the search handle. 
    if (!FindClose(hSearch)) 
	{ MessageBoxA(NULL,"Couldn't close search handle.","你好！",MB_OK); }
  } else//否则没找到本路径下的第一个文件,即没找到任何文件
  { MessageBoxA(NULL,"No file found.","你好！",MB_OK); }

//  char buf[20]="";
//  sprintf(buf,"有%d个TXT文件",nCount);
//  MessageBoxA(NULL,buf,"你好！",MB_OK);
  return nCount;
}
int BrowPathFiles(char *path_name, char *filterExt, std::vector<FileVector> &files)
{ 
  char path_ext_name[MAX_PATH];
  //先拼接得到带扩展名的路径名path_ext_name字符串
  strcpy(path_ext_name,path_name);strcat(path_ext_name,"*.*");
  WIN32_FIND_DATAA FileData;
  HANDLE hSearch;
  int nCount=0;
  BOOL fFinished = FALSE;
  char fname[512],ext_name[512];
  str_no_cap(filterExt);
  // Start searching for path_ext_name files in the current directory. 
  hSearch = FindFirstFileA(path_ext_name, &FileData);//先搜索本路径下的第一个文件FileData.cFileName
  if (hSearch != INVALID_HANDLE_VALUE)
  { //
	//strcpy(files[nCount],FileData.cFileName);nCount++;//添加第一个文件
	while (FindNextFileA(hSearch, &FileData))
	if((FileData.dwFileAttributes&0xff)!=FILE_ATTRIBUTE_DIRECTORY) 
	{ FileVector afile;
	  ExtractFileExt(FileData.cFileName,fname,ext_name);
	  str_no_cap(ext_name);
	  if(strcmp(ext_name,".") && (strstr(filterExt,ext_name) || strstr(filterExt,"*.*")))
	  { strcpy(afile.name,FileData.cFileName);
	    files.push_back(afile);
	  }
	}
    if (GetLastError() != ERROR_NO_MORE_FILES)
	{ //MessageBoxA(NULL,"Couldn't find next file.","你好！",MB_OK);
	}
    // Close the search handle. 
    if (!FindClose(hSearch)) 
	{ 
	//	MessageBoxA(NULL,"Couldn't close search handle.","你好！",MB_OK);
	}
  } else//否则没找到本路径下的第一个文件,即没找到任何文件
  { //MessageBoxA(NULL,"No file found.","你好！",MB_OK);
  }

  return files.size();
}
int BrowPathFiles(char *path_name,char *filterExt,char files[][MAX_PATH])
{ char path_ext_name[MAX_PATH];
  //先拼接得到带扩展名的路径名path_ext_name字符串
  strcpy(path_ext_name,path_name);strcat(path_ext_name,"*.*");
  WIN32_FIND_DATAA FileData;
  HANDLE hSearch;
  int nCount=0;
  BOOL fFinished = FALSE;
  char fname[512],ext_name[512];
  str_no_cap(filterExt);
  // Start searching for path_ext_name files in the current directory. 
  hSearch = FindFirstFileA(path_ext_name, &FileData);//先搜索本路径下的第一个文件FileData.cFileName
  if (hSearch != INVALID_HANDLE_VALUE)
  { //
	//strcpy(files[nCount],FileData.cFileName);nCount++;//添加第一个文件
	while (FindNextFileA(hSearch, &FileData))
	if((FileData.dwFileAttributes&0xff)!=FILE_ATTRIBUTE_DIRECTORY) 
	{ ExtractFileExt(FileData.cFileName,fname,ext_name);
	  str_no_cap(ext_name);
	  if(strcmp(ext_name,".") && (strstr(filterExt,ext_name) || strstr(filterExt,"*.*")))
	  { strcpy(files[nCount],FileData.cFileName);nCount++;
	  }
	}
	if (GetLastError() != ERROR_NO_MORE_FILES){ MessageBoxA(NULL, "Couldn't find next file.", "你好！", MB_OK); }
    // Close the search handle. 
    if (!FindClose(hSearch)) 
	{
		MessageBoxA(NULL, "Couldn't close search handle.", "你好！", MB_OK);
	}
  } else//否则没找到本路径下的第一个文件,即没找到任何文件
  {
	  MessageBoxA(NULL, "No file found.", "你好！", MB_OK);
  }

//  char buf[20]="";
//  sprintf(buf,"有%d个TXT文件",nCount);
//  MessageBoxA(NULL,buf,"你好！",MB_OK);
  return nCount;
}

std::vector<FileVector> BrowPathFiles(char *path_name, char *filterExt)
{
	std::vector<FileVector> files;
  char path_ext_name[MAX_PATH];
  //先拼接得到带扩展名的路径名path_ext_name字符串
  strcpy(path_ext_name,path_name);strcat(path_ext_name,"*.*");
  WIN32_FIND_DATAA FileData;
  HANDLE hSearch;
  int nCount=0;
  BOOL fFinished = FALSE;
  char fname[512],ext_name[512];
  str_no_cap(filterExt);
  // Start searching for path_ext_name files in the current directory. 
  hSearch = FindFirstFileA(path_ext_name, &FileData);//先搜索本路径下的第一个文件FileData.cFileName
  if (hSearch != INVALID_HANDLE_VALUE)
  { //
	//strcpy(files[nCount],FileData.cFileName);nCount++;//添加第一个文件
	while (FindNextFileA(hSearch, &FileData))
	if((FileData.dwFileAttributes&0xff)!=FILE_ATTRIBUTE_DIRECTORY) 
	{ FileVector afile;
	  ExtractFileExt(FileData.cFileName,fname,ext_name);
	  str_no_cap(ext_name);
	  if(strcmp(ext_name,".") && (strstr(filterExt,ext_name) || strstr(filterExt,"*.*")))
	  { strcpy(afile.name,FileData.cFileName);
	    files.push_back(afile);
	  }
	}
	if (GetLastError() != ERROR_NO_MORE_FILES){ MessageBoxA(NULL, "Couldn't find next file.", "你好！", MB_OK); }
    // Close the search handle. 
    if (!FindClose(hSearch)) 
	{
		MessageBoxA(NULL, "Couldn't close search handle.", "你好！", MB_OK);
	}
  } else//否则没找到本路径下的第一个文件,即没找到任何文件
  {
	  MessageBoxA(NULL, "No file found.", "你好！", MB_OK);
  }

  return files;
}
//typedef DWORD (CALLBACK *FILE_BROW_DEAL_PROC) (void *pParams);
int BrowPathFiles(char *path_name,char *filterExt,FILE_BROW_DEAL_PROC one_loop_fun)
{ int count=0;
  char path_ext_name[MAX_PATH];
  //先拼接得到带扩展名的路径名path_ext_name字符串
  strcpy(path_ext_name,path_name);strcat(path_ext_name,"*.*");
  WIN32_FIND_DATAA FileData;
  HANDLE hSearch;
  BOOL fFinished = FALSE;
  char fname[512],ext_name[512];
  str_no_cap(filterExt);
  // Start searching for path_ext_name files in the current directory. 
  hSearch = FindFirstFileA(path_ext_name, &FileData);//先搜索本路径下的第一个文件FileData.cFileName
  if (hSearch != INVALID_HANDLE_VALUE)
  { //
	while (FindNextFileA(hSearch, &FileData))
	if((FileData.dwFileAttributes&0xff)!=FILE_ATTRIBUTE_DIRECTORY) 
	{ FileVector afile;
	  ExtractFileExt(FileData.cFileName,fname,ext_name);
	  str_no_cap(ext_name);
	  if(strcmp(ext_name,".") && (strstr(filterExt,ext_name) || strstr(filterExt,"*.*")))
	  { strcpy(afile.name,FileData.cFileName);
	    //files.push_back(afile);
	    if(one_loop_fun)
		{ one_loop_fun(afile.name);
		  count++;
		}
	  }
	}
	if (GetLastError() != ERROR_NO_MORE_FILES){ MessageBoxA(NULL, "Couldn't find next file.", "你好！", MB_OK); }
    // Close the search handle. 
    if (!FindClose(hSearch)) 
	{
		MessageBoxA(NULL, "Couldn't close search handle.", "你好！", MB_OK);
	}
  } else//否则没找到本路径下的第一个文件,即没找到任何文件
  {
	  MessageBoxA(NULL, "No file found.", "你好！", MB_OK);
  }

  return count;
}
//typedef DWORD (CALLBACK *FILE_BROW_DEAL_PROC2) (FileVector *filev,void *pParams);
int BrowPathFilesFun(char *path_name,char *filterExt,FILE_BROW_DEAL_PROC2 one_loop_fun,void *pParam)
{ int count=0;
  char path_ext_name[MAX_PATH];
  //先拼接得到带扩展名的路径名path_ext_name字符串
  strcpy(path_ext_name,path_name);strcat(path_ext_name,"*.*");
  WIN32_FIND_DATAA FileData;
  HANDLE hSearch;
  BOOL fFinished = FALSE;
  char fname[512],ext_name[512];
  str_no_cap(filterExt);
  // Start searching for path_ext_name files in the current directory. 
  hSearch = FindFirstFileA(path_ext_name, &FileData);//先搜索本路径下的第一个文件FileData.cFileName
  if (hSearch != INVALID_HANDLE_VALUE)
  { //
	while (FindNextFileA(hSearch, &FileData))
	if((FileData.dwFileAttributes&0xff)!=FILE_ATTRIBUTE_DIRECTORY) 
	{ FileVector afile;
	  ExtractFileExt(FileData.cFileName,fname,ext_name);
	  str_no_cap(ext_name);
	  if(strcmp(ext_name,".") && (strstr(filterExt,ext_name) || strstr(filterExt,"*.*")))
	  { strcpy(afile.name,FileData.cFileName);
	    //files.push_back(afile);
	    if(one_loop_fun)
		{ one_loop_fun(&afile,pParam);
		  count++;
		}
	  }
	}
	if (GetLastError() != ERROR_NO_MORE_FILES){ MessageBoxA(NULL, "Couldn't find next file.", "你好！", MB_OK); }
    // Close the search handle. 
    if (!FindClose(hSearch)) 
	{
		MessageBoxA(NULL, "Couldn't close search handle.", "你好！", MB_OK);
	}
  } else//否则没找到本路径下的第一个文件,即没找到任何文件
  ;//{ MessageBoxA(NULL,"No file found.","你好！",MB_OK); }

  return count;
}
//寻找文件夹路径下的子文件夹并存在std::vector<FileVector>
int BrowPathDirectory(char *path_name, std::vector<FileVector> &files)
{ 
/*	std::vector<FileVector> files;*/
	char path_ext_name[MAX_PATH];
	//先拼接得到带扩展名的路径名path_ext_name字符串
	strcpy(path_ext_name,path_name);
	strcat(path_ext_name,"*.*");
	WIN32_FIND_DATAA FileData;
	HANDLE hSearch;
	int nCount=0;
	BOOL fFinished = FALSE;
	char fname[512],ext_name[512];
	// Start searching for path_ext_name files in the current directory. 
	hSearch = FindFirstFileA(path_ext_name, &FileData);//先搜索本路径下的第一个文件FileData.cFileName
	if (hSearch != INVALID_HANDLE_VALUE)
	{ //
		//strcpy(files[nCount],FileData.cFileName);nCount++;//添加第一个文件
		while (FindNextFileA(hSearch, &FileData))
			if((FileData.dwFileAttributes&0xff)==FILE_ATTRIBUTE_DIRECTORY) 
			{
				FileVector afile;
				ExtractFileExt(FileData.cFileName,fname,ext_name);
				str_no_cap(ext_name);
				if(strcmp(ext_name,".")&&strcmp(ext_name,".."))
				{ 
					strcpy(afile.name,FileData.cFileName);
					files.push_back(afile);
				}
			}
			if (GetLastError() != ERROR_NO_MORE_FILES){ MessageBoxA(NULL, "Couldn't find next file.", "你好！", MB_OK); }
			// Close the search handle. 
			if (!FindClose(hSearch)) 
			{ 
				MessageBoxA(NULL, "Couldn't close search handle.", "你好！", MB_OK);
			}
	} 
	else//否则没找到本路径下的第一个文件,即没找到任何文件
	{ 
		MessageBoxA(NULL, "No file found.", "你好！", MB_OK);
	}	
	return files.size();
}
//寻找文件夹下及其子文件夹下的所有带postfix后缀名的文件
int AllFilesRec(char *path_name, char *postfix, std::vector <FileVector> &files)
{
	char path_ext_name[MAX_PATH];
  //先拼接得到带扩展名的路径名path_ext_name字符串
  strcpy(path_ext_name,path_name);
  strcat(path_ext_name,"*.*");
  WIN32_FIND_DATAA FileData;
  HANDLE hSearch;
  int nCount=0;
  BOOL fFinished = FALSE;
  char fname[512],ext_name[512];
  str_no_cap(postfix);
  // Start searching for path_ext_name files in the current directory. 
  hSearch = FindFirstFileA(path_ext_name, &FileData);//先搜索本路径下的第一个文件FileData.cFileName
  if (hSearch != INVALID_HANDLE_VALUE)
  { //
	//strcpy(files[nCount],FileData.cFileName);nCount++;//添加第一个文件
	while (FindNextFileA(hSearch, &FileData))
	if((FileData.dwFileAttributes&0xff)==FILE_ATTRIBUTE_DIRECTORY) //得到文件夹
	{ 
		//FileVector file;
		ExtractFileExt(FileData.cFileName,fname,ext_name);
		str_no_cap(ext_name);
		if(strcmp(ext_name,".") && strcmp(ext_name,"..") && strstr(postfix,ext_name))
		{
			int file_num=0;
			char son_path_ext_name[MAX_PATH];//定义子文件夹路径名
			strcpy(son_path_ext_name,path_name);
			strcat(son_path_ext_name,FileData.cFileName);
			strcat(son_path_ext_name,"\\");
			file_num = AllFilesRec(son_path_ext_name,postfix,files);
		}
	}
	else//得到文件
	{ 
		FileVector afile;
		ExtractFileExt(FileData.cFileName,fname,ext_name);
		str_no_cap(ext_name);
		if(strcmp(ext_name,".") && strstr(postfix,ext_name) && strcmp(ext_name,""))
		{ 
			strcpy(afile.name,path_name);
			strcat(afile.name,FileData.cFileName);
			files.push_back(afile);
		}
	}
    if (GetLastError() != ERROR_NO_MORE_FILES){ MessageBoxA(NULL,"Couldn't find next file.","你好！",MB_OK);}
    // Close the search handle. 
    if (!FindClose(hSearch)) 
	{
		MessageBoxA(NULL, "Couldn't close search handle.", "你好！", MB_OK);
	}
  } else//否则没找到本路径下的第一个文件,即没找到任何文件
  {
	  MessageBoxA(NULL, "No file found.", "你好！", MB_OK);
  }
  return files.size();
}


#include<shlobj.h>
#pragma comment(lib,"shell32.lib")
//【 在 lgwzxm (大眼眶) 的大作中提到: 】 
//: 在vc中打开文件对话框可以用CFileDialog类的DoModul()实现，但不知道选择路 
//: 径对话框如何实现，请各位热心人帮帮忙，请在寄一份给原作者后面打个勾，谢 
//: 谢了！ 
//选择路径对话框
int SelectDirDlg(char Dir[]) 
{ 
	BROWSEINFOA bi;  
	ITEMIDLIST *pidl;  
	
	bi.hwndOwner = NULL;  
	bi.pidlRoot = NULL;  
	bi.pszDisplayName = Dir;  
	bi.lpszTitle = "选择一个目录";  
	bi.ulFlags = BIF_RETURNONLYFSDIRS;  
	bi.lpfn = NULL;  
	bi.lParam = 0;  
	bi.iImage = 0;  
    
	pidl = SHBrowseForFolderA( &bi ); 
	//Display "Select Folder" dialog box,
	//Get the folder name and convert it into a ITEMLIST   
	//data structure.
	
	if ( pidl)  
	{ 
      if (SHGetPathFromIDListA( pidl, Dir ))// Retrieve folder name from ITEMLIST structure.  
	  { return 1;
	  } else
	  { Dir[0] = 0;
	    return 0;
	  }
	} else
	{ Dir[0] = 0;
	  return 0;
	}
}

//从文件夹路径中提取文件夹名或从文件路径中提取文件名
void PicDocNameFromDocRoot(char *root_name,char *doc_name)
{
	std::string strname = root_name;
	std::string::iterator iter = strname.end();
	
	while(iter!=strname.begin() && (*iter)!='\\')
	{
		--iter;
	}
	strname.erase(strname.begin(),iter+1);
	
	strcpy(doc_name,strname.c_str());
	doc_name[strlen(doc_name)]='\0';
}

void GetModulePath(char *module_path,int back_debug)
{
  char module_file[512],exe_name[256];
  GetModuleFileNameA( NULL, module_file, 512 );
  ExtractFileName(module_file,module_path,exe_name);
  str_no_cap(module_path);
  if(strstr(module_path,"debug")||strstr(module_path,"release"))
  {  char back_dir[512],last[256];
	 ExtractLastPath2(module_path,back_dir,last);
	 if (strstr(back_dir, "x64"))
		 ExtractLastPath2(back_dir, module_path, last);//strcat(module_path, "../");
	 else
		 strcpy(module_path, back_dir);
  }
}
//读取文件fp的一行到str
int fread_line(FILE *fp,char *str)
{ fgets(str,512,fp);
  int l=strlen(str);
  str[l-1]='\0';
  return l;
}
//删除整个目录
int DeleteOneDirectory(char *full_path,int del_dir)
{ std::vector <FileVector> files;
  BrowPathFiles(full_path,"*.*",files);
  int j;
  for(j=0;j<(int)files.size();j++)
  { char full_file[512];
    sprintf(full_file,"%s\\%s",full_path,files[j].name);
    DeleteFileA(full_file);
  }
  if (del_dir)
    RemoveDirectoryA(full_path);
  printf("path %s is deleted!\n",full_path);
  return files.size();
}
//删除path下所有目录,慎用!
void DelAllDirectorys(char *path)
{ std::vector <FileVector> paths;
  BrowPathDirectory(path,paths);
  int i;
  for(i=0;i<(int)paths.size();i++)
  { char full_path[512];
    sprintf(full_path,"%s\\%s\\",path,paths[i].name);
	DeleteOneDirectory(full_path);
  }
}

//将绝对路径修正成相对路径
int update_path(char *path,char *work_path)
{ if(!FileExist(path) || !strstr(path,":"))
  { if(work_path && !strstr(path,":"))
    { char full_path[512];
      sprintf(full_path,"%s\\%s",work_path,path);
	  if(FileExist(full_path))
	  { strcpy(path,full_path);//修改成相对路径
	    return 1;
	  }
    }
  }
  return 0;
}

//创建多级目录
int CreatMultDir(char *pszDir)
{
	int i = 0;
	int iRet;
	int iLen = strlen(pszDir);

	//在末尾加/
	if (pszDir[iLen - 1] != '\\' && pszDir[iLen - 1] != '/')
	{
		pszDir[iLen] = '/';
		pszDir[iLen + 1] = '\0';
	}

	// 创建目录
	for (i = 0;i <= iLen;i ++)
	{
		if (pszDir[i] == '\\' || pszDir[i] == '/')
		{ 
			pszDir[i] = '\0';

			//如果不存在,创建
			iRet = ::FileExist(pszDir,0);
			if (iRet==0)
			{
				iRet = ::CreateDirectoryA(pszDir,NULL);
				if (iRet != 0)
				{
					//return -1;
				} 
			}
			//支持linux,将所有\换成/
			pszDir[i] = '/';
		} 
	}
	return 0;
}


int GetLatestPathFiles(char *path_name, char *filterExt, char *latest_name, char *start_str)
{
	int len = strlen(start_str);
	int max = -1;
	char _latest_name[512];
	std::vector<FileVector> files;
	int rn = BrowPathFiles(path_name, filterExt, files);
	for (int i = 0; i < (int)files.size(); i++)
	{
		char name[256],ext[12];
		ExtractFileExt(files[i].name, name, ext);
		char *p = strstr(name, start_str);
		if (p)
		{
			int n = atoi(p + len);
			if (n > max)
			{
				strcpy(_latest_name, files[i].name);
				max = n;
			}
			//printf("%s\n", p + len);
		}
	}
	sprintf(latest_name, "%s/%s", path_name,_latest_name);
	return max;
}

void get_full_directory(char *fname,char *full_name)
{
	if (!strstr(fname, ":"))
	{
		char module_path[512];
		GetModulePath(module_path, 1);
		if (full_name)
		 sprintf(full_name, "%s/%s", module_path, fname);
		else
		{
			char full_name[512];
			sprintf(full_name, "%s/%s", module_path, fname);
			strcpy(fname, full_name);
		}
	}
}