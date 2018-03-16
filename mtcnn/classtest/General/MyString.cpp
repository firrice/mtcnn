#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "MyString.h"

char strbuf[MAX_STRING_LEN];
int str_find(char *str,char *sub_str,char *end_str,char *res_str)
{ char *p=strstr(str,sub_str);
  if(p)
  { char *pA=p+strlen(sub_str);
	char *p1=strstr(pA,end_str);
	int n;
    if(p1)
	{ n=p1-pA;
	  strncpy(res_str,pA,n);	  
	} else
	{ strcpy(res_str,pA);
	  n=strlen(res_str);
	}
	res_str[n]='\0';
	return n;
  }
  return 0;
}
int str_find(char *str,char *sub_str,char *res_str)
{ char *p=strstr(str,sub_str);
  if(p)
  { strncpy(res_str,str,p-str);
    res_str[p-str]='\0';
	return p-str;
  } else
	  strcpy(res_str,str);
  return 0;
}
void strnicpy(char* str1,char* str2,int i,int n)
{ strncpy(str1,str2+i,n);str1[n]='\0';
}
void ni_replace(char *str,int i,int n,char *sub_str)
{ n_replace(str+i,n,sub_str);
}
void n_replace(char *str,int n,char *sub_str)
{ int len=strlen(str);
  char *pEnd=new char[len-n+1]; pEnd[len-n]='\0';
  strncpy(pEnd,str+n,len-n);
  strcpy(str,sub_str);
  int sub_len=strlen(sub_str);
  strcpy(str+sub_len,pEnd);
  delete pEnd;
}
char *str_replace(char *str,char *sub_str,char *new_str)
{ char *p=strstr(str,sub_str);
  if(p)
  { n_replace(p,strlen(sub_str),new_str);
    return str;
  } return 0;
}
int str_append(char *str,char *new_str,int mode)
{ char *p=strstr(str,new_str);
  if(!p)
  { switch(mode)
    { case 0:
       strcat(str,new_str);
       break;
	  case 1:
	  { int len = strlen(str);
		char *copy_buf=new char[len+1];
		 strcpy(copy_buf,str);
		 sprintf(str,"%s%s",new_str,copy_buf);
		delete copy_buf;
        break;
	  }
    }
    return 1;
  } return 0;
}

void str_no_cap(char *str)
{ int i,n=strlen(str);
  int offset='a'-'A';
  for(i=0;i<n;i++)
  { if(str[i]>='A' && str[i]<='Z') str[i]+=offset;
  }
}
int ExtractFileName(char* filename,char* dir,char* fname)
{ int i,n=strlen(filename);
//  char substr[3];
  for(i=n-1;i>=0;i--)
  { if(filename[i]=='/'||filename[i]=='\\')
     break;
    /*if(i<n-1)
	{ strncpy(substr,filename+i,1);substr[1]='\0';
	  if(strcmp(substr,"\\")==0)
	  {i++;break;}
	}*/
  }
  if(dir)
  { strnicpy(dir,filename,0,i+1);dir[i+1]='\0';
  }
  if(fname)
  { strnicpy(fname,filename,i+1,n-i-1);fname[n-i-1]='\0';
  }
  return(i);
}
int ExtractPathFileExt(char* path_name,char *path,char *name,char *ext)
{ char name_ext[256];
  int res = ExtractFileName(path_name,path,name_ext);
  ExtractFileExt(name_ext,name,ext);
  return res;
}
int ExtractLastPath(char *filename,char* dir,char* lastpath)
{ int i,n=strlen(filename);
  i=n-1;
  while(filename[i]=='/'||filename[i]=='\\')
  { i--;
  }
  if(i>0)
  { char copy[512];
    strcpy(copy,filename);
    copy[i+1]='\0';
    ExtractFileName(copy,dir,lastpath);
  }
  return(i);
}
int ExtractLastPath2(char *filename,char* dir,char* lastpath)
{ int i,n=strlen(filename);
  i=n-1;
  while(i>0 && filename[i]!='/'&&filename[i]!='\\')i--;
  if(i>0)
  { while(filename[i]=='/'||filename[i]=='\\')
     i--;
    char copy[512];
	strcpy(copy,filename);
    copy[i+1]='\0';
    ExtractFileName(copy,dir,lastpath);
  }
  return(i);
}
int replace_file_name(char* filename,char* replace_name,char* new_name)
{ char path[512],fname[256];
  int i=ExtractFileName(filename,path,fname);
  strcpy(new_name,path);
  strcat(new_name,replace_name);
  return i;
}

int ExtractFileExt(char* filename,char *name,char *ext)
{ int i,n=strlen(filename);
//  char substr[3];
  for(i=n-1;i>=0;i--)
  { if(filename[i]=='.')
     break;
    /*if(i<n-1)
	{ strncpy(substr,filename+i,1);substr[1]='\0';
	  if(strcmp(substr,"\\")==0)
	  {i++;break;}
	}*/
  }
  if(i>=0)
  { strnicpy(name,filename,0,i);name[i]='\0';
	strnicpy(ext,filename,i,n-i);
    ext[n-i]='\0';
  } else
  { strcpy(name,filename);
	ext[0]='\0';
  }
  return(i);
}

int AppendFileName(char* filename,char *Append,char* NewExt,char *NewName)
{ char name[255],ext[8];
  ExtractFileExt(filename,name,ext);
  strcat(name,Append);
  strcat(name,NewExt);
  strcpy(NewName,name);
  return strlen(NewName);
}
int AppendFileName(char* filename,char *Append,char *NewName)
{ char name[255],ext[8];
  ExtractFileExt(filename,name,ext);
  strcat(name,Append);
  strcat(name,ext);
  strcpy(NewName,name);
  return strlen(NewName);
}
int AppendFileName2(char *path,char *filename,char *NewExt,char *NewName)
{ char name[255],ext[8],path0[512];
  ExtractPathFileExt(filename,path0,name,ext);
  sprintf(NewName,"%s\\%s%s",path,name,NewExt);
  return strlen(NewName);
}

/*Replaces a file's extension, which is assumed to be everything after the
last dot ('.') character.
@param file the name of a file
@param extn a new extension for \a file; should not include a dot (i.e.
	\c "jpg", not \c ".jpg") unless the new file extension should contain
	two dots.
@return Returns a new string formed as described above.  If \a file does
	not have an extension, this function simply adds one.*/
char* replace_extension( const char* file, const char* extn )
{
	char* new_file, * lastdot;

	new_file = (char *)calloc( strlen( file ) + strlen( extn ) + 2,  sizeof( char ) );
	strcpy( new_file, file );
	lastdot = strrchr( new_file, '.' );
	if( lastdot )
		*(lastdot + 1) = '\0';
	else
		strcat( new_file, "." );
	strcat( new_file, extn );

	return new_file;
}
void replace_extension( const char* file, const char* extn, char *new_file )
{
    char *lastdot;
    strcpy( new_file, file );
    lastdot = strrchr( new_file, '.' );
    if( lastdot )
        *(lastdot + 1) = '\0';
    else
        strcat( new_file, "." );
    strcat( new_file, extn );
}

/*A function that removes the path from a filename.  Similar to the Unix
basename command.
@param pathname a (full) path name
@return Returns the basename of \a pathname.*/
char* basename2( char* pathname )
{
	char* base, * last_slash;

	last_slash = strrchr( pathname, '/' );
	if( ! last_slash )
	{
		base = (char *)calloc( strlen( pathname ) + 1, sizeof( char ) );
		strcpy( base, pathname );
	}
	else
	{
		base = (char *)calloc( strlen( last_slash++ ), sizeof( char ) );
		strcpy( base, last_slash );
	}

	return base;
}

char *search_last_fname(char *fullname)
{ char *filename = strrchr( fullname, '\\' );
  if( filename == NULL )
   filename = strrchr( fullname, '/' );
  if( filename == NULL )
   filename = fullname;
  else filename++;
  return filename;
}

int SplitString(char *str,char *needle,
				char *new_str,char **stack)
{ strcpy(new_str,str);
  char *p=new_str;
  int count=0;
  int lll = strlen(new_str);
  if(str[0]!=10)
  {
	  stack[count++]=new_str;
	  char *pSplit = strstr( new_str, needle);
	  while( pSplit != NULL ) 
	  { pSplit[0]='\0';
	  //    printf( "%s\n ", new_str);
	  p = pSplit + strlen(needle); 
	  stack[count++] = p;
	  pSplit = strstr( p, needle); 
	  }
  }
  return count;
}
int SplitString2(char *str,char needle)
{ int i=strlen(str)-1;
  while(i>0)
  { if(str[i]==needle)break;
    i--;
  }
  if(i>0)str[i]='\0';
  return i;
}
int remove_needle(char *new_str,char **stack,int n)
{
    if(n==0)
        return 0;
    int max_L = (stack[n-1]+strlen(stack[n-1])-new_str);
    if(max_L<=0)
        return 0;

    char *new_str2=new char[max_L+1],**stack2=new char*[256];

    int count = 0;
    stack2[0] = new_str2;
	int i;
    for(i=0;i<max_L;i++)
    {
        if(i==0 || new_str[i-1]!=0 || new_str[i]!=0)
        {
           new_str2[count] = new_str[i];
           count++;
        }
    }
    new_str2[count]=0;
    count++;

    /*for(i=0;i<count;i++)
    {
        if(new_str2[i])
            printf("%c",new_str2[i]);
        else
            printf("#");
    }
    printf("\n");*/

    int ns=0;
    for(i=0;i<count;i++)
    {
        new_str[i] = new_str2[i];
        //printf("%c",new_str[i]);
        if((i==0 || new_str[i-1]==0) && new_str[i]!=0 && new_str[i]!=10)
        {
            //printf("(%c)",new_str[i]);
            if(strlen(new_str+i)>0)
            {
                stack[ns]=new_str+i;
                ns++;
            }
        }
    }
    new_str[count]='\0';

    /*for(i=0;i<ns;i++)
    {
        printf("%s  ",stack[i]);
    }*/

    delete stack2;
    delete new_str2;

    return ns;
}

int is_num_char(char c)
{ return (c>='0' && c<='9');
}
int is_cap_char(char c)
{ return (c>='A' && c<='Z');
}

void str_reverse(char *str,int n)
{ int i;
  for(i=0;i<n/2;i++)
  { char t=str[n-1-i];
    str[n-1-i]=str[i];
	str[i]=t;
  }
}

void copy2(char *buf,int n1,char *str)
{ int i;
  for(i=0;i<n1;i++)
  { str[i]=buf[i];
  }
  str[n1]='\0';
}

int create_split_strings(char *str,char *sp_strs[64])
{ int L=strlen(str);
  int i,count=0,j;
  for(i=0;i<L;i++)
  { for(j=i;j<L&&str[j]==' ';j++);
    if(j<L)
    { int str_len=0;
      for(j=i;j<L&&str[j]!=' ';j++,str_len++);
	  if(str_len>0)
	  { sp_strs[count]=new char[str_len+1];
	    strncpy(sp_strs[count],str+i,str_len);
		sp_strs[count][str_len]='\0';
	    count++;
	  }
	  i+=str_len;
	}
  }
  return count;
}

void release_split_strings(char *sp_strs[64],int n)
{ int i;
  for(i=0;i<n;i++)
  { delete sp_strs[i];
  }
}

int compress_strings(int *ids,int ni,
				     char *str1,char *str2,char *str3,
				     char *sp,
				     char *buffer)
{ int *bi=(int*)buffer;
  int i;
  for(i=0;i<ni;i++)
  { bi[i]=ids[i];
  }
  int lsp = strlen(sp);
  char *buffer_str = buffer+ni*sizeof(int);
  //sprintf(buffer_str,"%s%s%s%s%s",str1,sp,str2,sp,str3);
  if(str1)
  { strcpy(buffer_str,str1);
    buffer_str+=strlen(str1);
  }
  if(str2)
  { if(str1)
    { strcpy(buffer_str,sp);
      buffer_str+=lsp;
    }
	strcpy(buffer_str,str2);
	buffer_str+=strlen(str2);
  }
  if(str3)
  { if(str2 || str1)
    { strcpy(buffer_str,sp);
      buffer_str+=lsp;
    }
	strcpy(buffer_str,str3);
	buffer_str+=strlen(str3);
  }
  return  buffer_str - buffer;//ni*sizeof(int) + l1 +ls+ l2 +ls+ l3;
}

int extract_strings(char *buffer,
					int *ids,int ni,
					char *str1,char *str2,char *str3,char *sp)
{ int *bi=(int*)buffer;
  int i;
  for(i=0;i<ni;i++)
  { ids[i] = bi[i];
  }
  char *buffer_str = buffer+ni*sizeof(int);
  char new_str[2048],*stacks[8];
  int n = SplitString(buffer_str,sp,new_str,stacks);
  int count=0;
  if(str1)
  { while(strlen(stacks[count])<1 && count<n)count++;
    if(count<n)
	{ strcpy(str1,stacks[count]);
      str1[strlen(stacks[count])]='\0';
	  count++;
	}
  }
  if(str2)
  { while(strlen(stacks[count])<1 && count<n)count++;
    if(count<n)
	{ strcpy(str2,stacks[count]);
      str2[strlen(stacks[count])]='\0';
	  count++;
	}
  }
  if(str3)
  { while(strlen(stacks[count])<1 && count<n)count++;
    if(count<n)
	{ strcpy(str3,stacks[count]);
      str3[strlen(stacks[count])]='\0';
	  count++;
	}
  }
  return count;
}

void end_str(char *str)
{ int ci=0;
  while(str[ci]!='\n')ci++;
  str[ci]='\0'; //fscanf(fp,"\n");
}
void trim(char *str,char *str2)
{ int ci=0;
  while(str[ci]==' ')ci++;
  int L = strlen(str);
  int ci2=L-1;
  while(str[ci2]==' ')
  { ci2--;
  }
  //ci--ci2
  int i;
  for(i=ci;i<=ci2;i++)
  { str[i-ci]=str[i];
  }
  str[i-ci]='\0';
}


