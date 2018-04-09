#ifndef __FACTORY_H__
#define __FACTORY_H__
#include "head.h"
#include "work_que.h"
typedef void* (*pfunc)(void*);
typedef struct{
	pthread_t *pthid;//线程id存储的起始地址
	pthread_cond_t cond;
	que_t que;
	int pthread_num;//线程数目
	int start_flag;//线程启动标志
	pfunc thread_func;//线程函数
}fac,*pfac;
typedef struct{
	int len;//控制数据
	char buf[1000];
}train;
#define FILENAME "file"
void factory_init(pfac,pfunc,int);
void factory_start(pfac);
int send_n(int,char*,int);
void tran_file(int);
#endif
