#include "factory.h"

void tran_file(int new_fd)
{
	signal(SIGPIPE,SIG_IGN);
	train d;
	strcpy(d.buf,FILENAME);
	d.len=strlen(d.buf);
	//把文件名传输给客户端
	send(new_fd,&d,4+d.len,0);
	int fd;
	fd=open(FILENAME,O_RDONLY);
	if(-1==fd)
	{
		perror("open");
		goto end;
	}
	struct stat buf;
	int ret=fstat(fd,&buf);
	if(-1==ret)
	{
		perror("fstat");
		goto end;
	}
	d.len=sizeof(off_t);//发送文件长度
	memcpy(d.buf,&buf.st_size,d.len);
	send_n(new_fd,(char*)&d,4+d.len);
	while((d.len=read(fd,d.buf,sizeof(d.buf)))>0)//发送文件内容
	{
		ret=send_n(new_fd,(char*)&d,4+d.len);
		if(-1==ret)
		{
			goto end;
		}
	}
	d.len=0;
	send_n(new_fd,(char*)&d,4);//告诉客户端文件发送结束
end:
	close(new_fd);
}
