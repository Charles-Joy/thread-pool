#include "func.h"
//服务器断开，客户端正常退出是这个
int main(int argc,char* argv[])
{
	if(argc!=3)
	{
		printf("./server IP PORT\n");
		return -1;
	}
	int sfd=socket(AF_INET,SOCK_STREAM,0);
	if(-1==sfd)
	{
		perror("socket");
		return -1;
	}
	struct sockaddr_in ser;
	bzero(&ser,sizeof(ser));
	ser.sin_family=AF_INET;
	ser.sin_port=htons(atoi(argv[2]));//将端口转换为网络字节序
	ser.sin_addr.s_addr=inet_addr(argv[1]);//将点分十进制的ip地址转为32位的网络字节序
	int ret;
	ret=connect(sfd,(struct sockaddr*)&ser,sizeof(ser));
	if(-1==ret)
	{
		perror("connect");
		return -1;
	}
	int len;
	char buf[1000]={0};
	ret=recv(sfd,&len,sizeof(len),0);
	if(0==ret)
	{
		printf("server close\n");
		goto end;
	}
	ret=recv(sfd,buf,len,0);
	if(0==ret)
	{
		printf("server close\n");
		goto end;
	}
	int fd;
	fd=open(buf,O_RDWR|O_CREAT,0666);
	off_t f_size;
	ret=recv_n(sfd,(char*)&len,sizeof(len));
	if(-1==ret)
	{
		printf("server close\n");
		goto end;
	}
	ret=recv_n(sfd,(char*)&f_size,len);//接文件长度
	if(-1==ret)
	{
		printf("server close\n");
		goto end;
	}
	printf("%ld\n",f_size);
	float f=0;
	int j=1;
	time_t now,last;
	now=time(NULL);
	last=now;
	//以时间打印百分比
	while(1)
	{
		ret=recv_n(sfd,(char*)&len,sizeof(len));
		if(-1==ret)
		{
			printf("%5.2f%s\n",f/f_size*100,"%");
			break;
		}
		else if(len>0)
		{
			f=f+len;
			time(&now);
			if(now-last>=1)
			{
				printf("%5.2f%s\r",f/f_size*100,"%");
				fflush(stdout);
				last=now;
			}
			ret=recv_n(sfd,buf,len);
			write(fd,buf,len);
			if(-1==ret)
			{
				printf("%5.2f%s\n",f/f_size*100,"%");
				break;
			}
		}else{
			printf("          \r");
			printf("%d%s\n",100,"%");
			break;
		}
	}
end:
	close(fd);
	close(sfd);
}
