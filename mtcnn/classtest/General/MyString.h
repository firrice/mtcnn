#ifndef __MYSTRING2__
#define __MYSTRING2__

#define MAX_STRING_LEN 256
extern char strbuf[MAX_STRING_LEN];

#ifdef __cplusplus  
extern "C" {  
#endif  
//从str里面截取字符串res_str
int str_find(char *str,char *sub_str,char *end_str,char *res_str);
//从str里面截取sub_str前面的字符串res_str
//int str_find(char *str,char *sub_str,char *res_str);

//从str1截取从第i个字符开始长度为n的子字串str2
void strnicpy(char* str1,char* str2,int i,int n);
//替换字符串str+n为sub_str
void ni_replace(char *str,int i,int n,char *sub_str);
 void n_replace(char *str,int n,char *sub_str);
//替换str里的字串sub_str为new_str
char *str_replace(char *str,char *sub_str,char *new_str);
//往str里的字串后面添加new_str
//int str_append(char *str,char *new_str,int mode=0);

//将字符串中所有大写字符转换成小写
void str_no_cap(char *str);

//抽取路径和文件名
int ExtractFileName(char* filename,char* dir,char* fname);
//抽取最后一个路径名
int ExtractLastPath(char* filename,char* dir,char* lastpath);
int ExtractLastPath2(char *filename,char* dir,char* lastpath);
//替换文件名,保留路径
int replace_file_name(char* filename,char* replace_name,char* new_name);

//抽取文件名和扩展名
int ExtractFileExt(char* filename,char *name,char *ext);
int ExtractPathFileExt(char* path_name,char *path,char *name,char *ext);
//将文件名filename后面加上后缀Append,并换上一个新的扩展名NewExt
int AppendFileName(char* filename,char *Append,char *NewExt,char *NewName);
//将文件名filename后面加上后缀Append
//int AppendFileName(char* filename,char *Append,char *NewName);
//将路径path下生成文件名filename后面加上扩展名NewExt,得到NewName
int AppendFileName2(char *path,char *filename,char *NewExt,char *NewName);

/*Replaces a file's extension, which is assumed to be everything after the
last dot ('.') character.
@param file the name of a file
@param extn a new extension for \a file; should not include a dot (i.e.
	\c "jpg", not \c ".jpg") unless the new file extension should contain
	two dots.
@return Returns a new string formed as described above.  If \a file does
	not have an extension, this function simply adds one.*/
extern char* replace_extension( const char* file, const char* extn );
//void replace_extension( const char* file, const char* extn, char *new_file );
//寻找全路径字符串文件名的末尾文件名指针
char *search_last_fname(char *fullname);

/*A function that removes the path from a filename.  Similar to the Unix
basename command.
@param pathname a (full) path name
@return Returns the basename of \a pathname.*/
extern char* basename2( char* pathname );

//将字符串str分割成new_str,stack[0..return-1]
int SplitString(char *str,char *needle,
				char *new_str,char **stack);
int SplitString2(char *str,char needle);
int remove_needle(char *new_str,char **stack,int n);

//判断字符串是数字
int is_num_char(char c);
//判断字符串是字母
int is_cap_char(char c);

//颠倒字符串
void str_reverse(char *str,int n);

//Buffer末尾加上\0结束变成字符串
void copy2(char *buf,int n1,char *str);

//字符串分段成字符串组
int create_split_strings(char *str,char *sp_strs[64]);
//释放字符串组
void release_split_strings(char *sp_strs[64],int n);

//将ids[0..ni-1]+str1,sp,str2,sp,str3打包给buffer[]
int compress_strings(int *ids,int ni,
				     char *str1,char *str2,char *str3,
				     char *sp,
				     char *buffer);

int extract_strings(char *buffer,
					int *ids,int ni,
					char *str1,char *str2,char *str3,char *sp);

//让字符串以换行结尾改成以\0结尾
void end_str(char *str);

//左右修剪字符串str空格成str2
void trim(char *str,char *str2);

#ifdef __cplusplus  
}
#endif  

#endif
