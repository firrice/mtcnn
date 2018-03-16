#ifndef __FILEPROC__
#define __FILEPROC__

#include <windows.h>
#include <stdio.h>

#define MAX_FILE_LINE_LEN  512//一行字符最多512字节

#define BUFFER_SIZE (0x2000)
#define MAX_FILE_SIZE (20*1024*1024)

#include "iostream"
#include <vector>
//using namespace std;
//typedef char FileVector[MAX_PATH];
typedef struct
{ char name[MAX_PATH];
} FileVector;

//把文件导入到一个缓存buffer里面
int File2Buffer(char *fname,char *buffer);
//把缓存buffer[0..file_size-1]里面的内容保存到文件
int Buffer2File(char *buffer,int size,char *fname);
//获取文件大小
unsigned long get_file_size(const char *path);

//担心由于命名空间重复造成错误另外建立了一个函数
BOOL GetOpenFileName2(LPSTR szName,LPSTR filter,LPSTR dir,char *title=NULL);
//打开文件对话框
BOOL GetOpenFileName(LPSTR szName,LPSTR filter,LPSTR dir,char *title=NULL);
//保存文件对话框
BOOL GetSaveFileName2(LPSTR szName,LPSTR filter,LPSTR dir);

//察看文件是否存在并得到文件信息FindFileData
int FileExist(LPSTR szName,WIN32_FIND_DATAA &FindFileData);
int FileExist(LPSTR szName,WIN32_FIND_DATAA *FindFileData=NULL);
int FileExist_slash(LPSTR szName,WIN32_FIND_DATAA *FindFileData=NULL);
//寻找文件夹下及其子文件夹下的所有带postfix后缀名的文件
int AllFilesRec(char *path_name, char *postfix, std::vector <FileVector> &files);
//在文件fp里扫描一行记录到Line数组
char* ScanALine(FILE *fp,char *Line);
//扫描一个文本文件把每一行字符串记录下来
int ScanAFile(char *file_name,char Lines[][MAX_FILE_LINE_LEN]);

//以下是获得当前路径的API函数调用方法(注意要#include<windows.h>)
//char CurDirectory[MAX_STRING_LEN];
//GetCurrentDirectory(MAX_STRING_LEN,CurDirectory);

//以下是判断文件是否存在的API函数用法
//char szName[129];
//if(GetOpenFileName(szName,//输出文件名
//                   "Chess files (*.cnd)\0*.cnd\0\0",//文件过滤器
//				   NULL))//文件目录
//{ WIN32_FIND_DATA FindFileData;
//  if(!FileExist(szName,FindFileData))SaveBoard(szName);//文件不存在才保存
//   else
//   if(bn("文件已经存在是否覆盖?","警告!"))SaveBoard(szName);
//}
//遍历得到路径path_name下,扩展名为filterExt的所有文件files[0..nCount-1],返回文件个数nCount
//不想用控件,不要MFC那一套,那你就得罗嗦点了://"D:\\*.txt"
int BrowPathFiles(char *path_name, char *filterExt, std::vector<FileVector> &files);
int BrowPathFiles_old(char *path_name,char *filterExt,char files[][MAX_PATH]);
int BrowPathFiles(char *path_name,char *filterExt,char files[][MAX_PATH]);
std::vector<FileVector> BrowPathFiles(char *path_name, char *filterExt);
typedef DWORD (CALLBACK *FILE_BROW_DEAL_PROC) (void *pParams);
int BrowPathFiles(char *path_name,char *filterExt,FILE_BROW_DEAL_PROC one_loop_fun);
typedef DWORD (CALLBACK *FILE_BROW_DEAL_PROC2) (FileVector *filev,void *pParams);
int BrowPathFilesFun(char *path_name,char *filterExt,FILE_BROW_DEAL_PROC2 one_loop_fun,void *pParam);
 //用法如下:
 /*if(GetOpenFileName(szName,//输出文件名
                    "Chess files (*.cnd; *.fen)\0*.cnd; *.fen\0\0",//文件过滤器
				    NULL))//文件目录
 { char path_name[256],fname[150],files[200][MAX_PATH];
   ExtractFileName(szName,path_name,fname);//分离路径path_name和文件名fname
   int i,FileNumber=BrowPathFiles(path_name,"*.fen",files);
   //得到文件名列表字符串ListFiles
   char ListFiles[2048]="",buf[256];
   sprintf(buf,"共%d个文件\n",FileNumber);
   strcat(ListFiles,buf);
   for(i=0;i<FileNumber;i++)
   { sprintf(buf,"%d: %s\n",i,files[i]);
     strcat(ListFiles,buf);
   }
   bn(ListFiles,"");
 }*/
//寻找文件夹路径下的子文件夹并存在std::vector<FileVector>
int BrowPathDirectory(char *path_name, std::vector<FileVector> &files);

//选择路径对话框
int SelectDirDlg(char Dir[]);

//从文件夹路径中提取文件夹名或从文件路径中提取文件名
void PicDocNameFromDocRoot(char *root_name,char *doc_name);

void GetModulePath(char *module_path,int back_debug=0);

//读取文件fp的一行到str
int fread_line(FILE *fp,char *str);

//删除整个目录
int DeleteOneDirectory(char *full_path, int del_dir=1);
//删除path下所有目录,慎用!
void DelAllDirectorys(char *path);

//打开文件对话框
BOOL GetOpenFileNameMult(std::vector <FileVector> &files, LPSTR filter, LPSTR init_dir, char *out_path, int order, char *title = NULL);

//将绝对路径修正成相对路径
int update_path(char *path,char *work_path);

//创建多级目录
int CreatMultDir(char *pszDir);

int GetLatestPathFiles(char *path_name, char *filterExt, char *latest_name, char *start_str);

///目录是否存在的检查：
int  CheckFolderExist(char *strPath);

void get_full_directory(char *fname, char *full_name);

#endif