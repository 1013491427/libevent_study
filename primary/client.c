#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include<unistd.h>
#include<event.h>


#define PORT       8888
#define SERVER_IP  "127.0.0.1"

int connect_to_server(void)
{
	int sock_fd = 0;
	int status = 0;
	struct sockaddr_in  server_addr;
	
	sock_fd = socket(AF_INET,SOCK_STREAM,0);
	if(-1 == sock_fd)
	{
		return sock_fd;
	}
	
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	status = inet_aton(SERVER_IP,&server_addr.sin_addr);
	if(0 == status)
	{
		errno = EINVAL;
		return -1;
	}
	
	status = connect(sock_fd,(struct sockaddr*)&server_addr,sizeof(server_addr));
	if(-1 == status)
	{
		close(sock_fd);
		return -1;
	}
	evutil_make_socket_nonblocking(sock_fd);
	
	return sock_fd;
}

void cmd_msg_callback(int fd,short events,void *arg)
{
	char msg[1024] = {0};
	int ret = 0;
	int sock_fd = 0;

	ret = read(fd,msg,sizeof(msg));
	if(0 >= ret)
	{
		perror("cmd read fail");
		exit(1);
	}
	
	sock_fd = *((int*)arg);
	write(sock_fd,msg,ret);
}

 
void sock_read_callback(int fd, short events, void *arg)
{
    char msg[1024];
 
    //为了简单起见，不考虑读一半数据的情况
    int len = read(fd, msg, sizeof(msg)-1);
    if( len <= 0 )
    {
        perror("read fail ");
        exit(1);
    }
 
    msg[len] = '\0';
 
    printf("recv %s from server\n", msg);
}


int main(int argc,char **argv)
{
	int sock_fd = 0;
	struct event_base* base = NULL;
	struct event * ev_sockfd = NULL;
	struct event * ev_read_cmd = NULL;
	
	/*connect to server*/
	sock_fd = connect_to_server();
	if(-1 == sock_fd)
	{
		perror("connect_to_server  fail");
		return -1;
	}
	
	/*event init*/
	base  = event_base_new();	
	if(NULL == base)
	{
		printf("event_base_new fail \r\n");
		return -1;
	}
	
	ev_sockfd = event_new(base,sock_fd,EV_READ|EV_PERSIST,sock_read_callback,NULL);
	if(NULL == ev_sockfd)
	{
		printf("event_new  ev_sockfd fail \r\n");
		return -1;		
	}	
	event_add(ev_sockfd,NULL);
	printf("event_add sockfd  success \r\n");
	
	/*创建事件，从终端中读取信息*/
	ev_read_cmd = event_new(base,STDIN_FILENO,EV_READ|EV_PERSIST,cmd_msg_callback,(void*)&sock_fd);
	if(NULL == ev_read_cmd)
	{
		printf("event_new  ev_read_cmd fail \r\n");
		return -1;		
	}	
	event_add(ev_read_cmd,NULL);
	printf("event_add ev_read_cmd  success \r\n");
	
	event_base_dispatch(base);
	
	return 0;
}




