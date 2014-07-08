#include <stdio.h>
#include <string.h>
#include <windows.h>

bool verinc(char *line,const char *substr,const char *mask)
{
 if (strstr(line,substr))
  {
   int v;
   sscanf(line,mask,&v);
   v++;
   sprintf(line,mask,v);
   return true;
  }
 return false;
}

int main(int argc,const char *argv[])
{
 if (argc<=1) return -1;
 FILE *fr=fopen(argv[1],"rt");
 if (!fr)
  {
   fr=fopen(argv[1],"wt");if (!fr) return -2;
   fprintf(fr,"#define VERSION_MAJOR 0\n");
   fprintf(fr,"#define VERSION_MINOR 0\n");
   fprintf(fr,"#define VERSION_BUILD 0\n");
   fprintf(fr,"#define VERSION_QFE 1\n");
   fprintf(fr,"#define VERSION_BUILD_DATE \"01/06/2005\"\n");
   fprintf(fr,"#define VERSION_BUILD_TIME \"08:00:14UTC\"\n");
   fclose(fr);
   fr=fopen(argv[1],"rt");
  }
 WIN32_FIND_DATA ffd;
 if (FindFirstFile(argv[1],&ffd)==INVALID_HANDLE_VALUE) return -4;
 char flnmO[520];
 strncpy(flnmO, argv[1], 260);
 flnmO[259]=0;
 strncat(flnmO, ".verinc", 260);
 flnmO[259]=0;
 remove(flnmO);
 FILE *fw=fopen(flnmO,"wt");if (!fw) return -3;
 while (!feof(fr))
  {
   static char line[1000];
   if (!fgets(line,1000,fr)) break;
   verinc(line,"#define VERSION_QFE ","#define VERSION_QFE %i\n");
   if (strstr(line,"VERSION_BUILD_DATE "))
    {
     SYSTEMTIME st;
     GetSystemTime(&st);
     sprintf(line,"#define VERSION_BUILD_DATE \"%02u/%02u/%02u\"\n", st.wDay, st.wMonth, st.wYear);
    }
   else if (strstr(line,"VERSION_BUILD_TIME "))
    {
     SYSTEMTIME st;
     GetSystemTime(&st);
     sprintf(line,"#define VERSION_BUILD_TIME \"%02u:%02u:%02uUTC\"\n", st.wHour, st.wMinute, st.wSecond);
    }
   fputs(line,fw);
  }
 fclose(fr);fclose(fw);
 remove(argv[1]);rename(flnmO,argv[1]);
 HANDLE hf=CreateFile(argv[1],FILE_WRITE_ATTRIBUTES,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
 SetFileTime(hf,&ffd.ftCreationTime,&ffd.ftLastAccessTime,&ffd.ftLastWriteTime);
 CloseHandle(hf);
 return 0;
}
