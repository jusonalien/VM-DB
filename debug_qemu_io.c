/*************************************************************************
	> File Name: file.c
	> Author:jusonalien 
	> Mail:jusonalien@qq.com 
	> Created Time: Wed 26 Aug 2015 07:16:14 PM KST
	> Description: We use this program to detect and debug the path of 
		       the Qemu disk IO.
 ************************************************************************/

/*---------------------------------*/
/*------DEBUG_QEMU_IO_MODE---------*/



#define  DEBUG_QEMU_IO_MODE        /*---open or close the debug mode*/


#ifdef   DEBUG_QEMU_IO_MODE  

#define  ALLOW_RAW_OPEN                 1
#define  ALLOW_RAW_REOPEN_PREPARE       1
#define  ALLOW_HANDLE_AIOCB_RW_VECTOR   1
#define  ALLOW_HANDLE_AIOCB_RW_LINEAR   1
#define  ALLOW_LAIO_SUBMIT              1
#define  ALLOW_PAIO_SUBMIT              1

#define  PATH_RAW_REOPEN_PREPARE      \
        "/tmp/raw_reopen_prepare.log"
#define  PATH_HANDLE_AIOCB_RW_VECTOR
#define  PATH_RAW_OPEN            
#define  PATH_HANDLE_ATIOCB_RW_RW_LINEAR
#define  PATH_LAIO_SUBMIT             \
        "/tmp/laio-submit.log"
#define  PATH_PAIO_SUBMIT             \
        "/tmp/paio-submit.log"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
/*----------------DEBUG_FUNCTION--------------*/
void printf_debug(char *Path,     char* functionName,
                  int DebugAllow, unsigned int DataSize) 
{
    if (DebugAllow == 0) 
        return ;
    struct tm* p;
    time_t timep;
    time(&timep);
    p = gmtime(&timep);
    char s[200] = "";
    int fd = open(Path,O_RDWR | O_CREAT | O_APPEND,
                  S_IRUSR | S_IWUSR );
    sprintf(s+strlen(s), "%s %d/%d/%d/ %d:%d:%d size:%d\n",
                 functionName,(1900+p->tm_year),(1+p->tm_mon),p->tm_mday,
                 p->tm_hour,      p->tm_min,    p->tm_sec,   DataSize);
    write(fd,s,sizeof(s));
    close(fd);
}
/*----------------------------------------*/
/*----------------------------------------*/

#endif 

#ifdef DEBUG_QEMU_IO_MODE 
#define ALLOW_PATH_FUN1 1  
#define ALLOW_PATH_FUN2 1

char *PathFun1 = "/tmp/fun1.log";
char *PathFun2 = "/tmp/fun2.log";
#define PATH_FUN1  "tmp/fun1.log"
#define PATH_FUN2  "tmp/fun2.log"
#endif 
void fun1() {
   printf_debug(PathFun1,"fun1", ALLOW_PATH_FUN1,0);
}
void fun2() {
   printf_debug(PathFun2,"fun2", ALLOW_PATH_FUN2,0);
}
 



int main()
{
    int cnt = 11111;
    while (cnt) {
        fun1();
        fun2();
        cnt --;
    }
    return 0;
}
