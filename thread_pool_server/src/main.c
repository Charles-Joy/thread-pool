#include "factory.h"
void* thread_handle(void* p)
{
	pfac pf=(pfac)p;
	pque_t pq=&pf->que;
	pnode_t pcur;
	while(1)
	{
		pthread_mutex_lock(&pq->que_mutex);
		if(!pq->que_size)
		{
			pthread_cond_wait(&pf->cond,&pq->que_mutex);
		}
		que_get(pq,&pcur);
		pthread_mutex_unlock(&pq->que_mutex);
		tran_file(pcur->new_fd);
		free(pcur);
	}
}
int main(int argc,char* argv[])
{
	if(argc!=5)
	{
		printf("./server IP PORT THREAD_NUM CAPACITY\n");
		return -1;
	}
	fac f;//主要数据结构
	bzero(&f,sizeof(f));
	f.pthread_num=atoi(argv[3]);
	int capacity=atoi(argv[4]);
	factory_init(&f,thread_handle,capacity);
	int sfd=socket(AF_INET,SOCK_STREAM,0);
	if(-1==sfd)
	{
		perror("socket");
		return -1;
	}
	int reuse=1;
	int ret;
	ret=setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));
	if(-1==ret)
	{
		perror("setsockopt");
		return -1;
	}
	struct sockaddr_in ser;
	bzero(&ser,sizeof(ser));
	ser.sin_family=AF_INET;
	ser.sin_port=htons(atoi(argv[2]));//将端口转换为网络字节序
	ser.sin_addr.s_addr=inet_addr(argv[1]);//将点分十进制的ip地址转为32位的网络字节序
	ret=bind(sfd,(struct sockaddr*)&ser,sizeof(ser));
	if(-1==ret)
	{
		perror("bind");
		return -1;
	}
	factory_start(&f);
	listen(sfd,100);
	int new_fd;
	pnode_t pnew;
	pque_t pq=&f.que;
	while(1)
	{
		new_fd=accept(sfd,NULL,NULL);
		pnew=(pnode_t)calloc(1,sizeof(node_t));
		pnew->new_fd=new_fd;
		pthread_mutex_lock(&pq->que_mutex);
		que_set(pq,pnew);
		pthread_mutex_unlock(&pq->que_mutex);
		pthread_cond_signal(&f.cond);//来一个任务让条件变量成立一次
	}
}
