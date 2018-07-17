#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
 
#include <unistd.h>
#include <event.h>

#define PORT       8888
#define SERVER_IP  "127.0.0.1"

int server_init(void)
{
	int sock_fd = 0;
	int status = 0;
	int ret = 0;
	struct sockaddr_in  server_addr;
	
	sock_fd = socket(AF_INET,SOCK_STREAM,0);
	if(-1 == sock_fd)
	{
		return sock_fd;
	}
	
	evutil_make_listen_socket_reuseable(sock_fd);
	
	
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	
	ret = bind(sock_fd,(struct sockaddr*)&server_addr,sizeof(server_addr));
	if(0 > ret)
	{
		goto error;
	}	
	
	ret = listen(sock_fd,10);
	if(0 > ret)
	{
		goto error;
	}
	//跨平台统一接口，将套接字设置为非阻塞状态
    evutil_make_socket_nonblocking(sock_fd);
	
	
	return sock_fd;

error:
	evutil_closesocket(sock_fd);
	return -1;
}


void sock_read_callback(int fd, short events, void *arg)
{
     char msg[4096];
    struct event *ev = (struct event*)arg;
    int len = read(fd, msg, sizeof(msg) - 1);
 
 
    if( len <= 0 )
    {
        printf("some error happen when read\n");
        event_free(ev);
        close(fd);
        return ;
    }
 
    msg[len] = '\0';
    printf("recv the client msg: %s", msg);
 
    char reply_msg[4096] = "I have recvieced the msg: ";
    strcat(reply_msg + strlen(reply_msg), msg);
 
    write(fd, reply_msg, strlen(reply_msg) );
}


void client_connect_callback(int fd,short events,void *arg)
{
    evutil_socket_t sockfd;
    struct sockaddr_in client;
	struct event_base* base = NULL;
	struct event *ev = NULL;
	
	
    socklen_t len = sizeof(client);
 
    sockfd =accept(fd, (struct sockaddr*)&client, &len );
    evutil_make_socket_nonblocking(sockfd);
 
    printf("accept a client %d\n", sockfd);
 
    base = (struct event_base*)arg;
 
    //仅仅是为了动态创建一个event结构体
	ev = event_new(NULL, -1, 0, NULL, NULL);
    //将动态创建的结构体作为event的回调参数
    event_assign(ev, base, sockfd, EV_READ | EV_PERSIST,
                 sock_read_callback, (void*)ev);
 
    event_add(ev, NULL);

}

 



int main(int argc,char **argv)
{
	int sock_fd = 0;
	struct event_base* base =  NULL;
	struct event * ev_sockfd = NULL;
	
	/*connect to server*/
	sock_fd = server_init();
	if(-1 == sock_fd)
	{
		perror("server_init  fail");
		return -1;
	}
	
	/*event init*/
	base  = event_base_new();	
	if(NULL == base)
	{
		printf("event_base_new fail \r\n");
		return -1;
	}
	
	
	/*创建事件，接收连接*/
	ev_sockfd = event_new(base,sock_fd,EV_READ|EV_PERSIST,client_connect_callback,base);
	if(NULL == ev_sockfd)
	{
		printf("event_new  ev_sockfd fail \r\n");
		return -1;		
	}	
	event_add(ev_sockfd,NULL);
	
	printf("event_add sockfd  success \r\n");
	event_base_dispatch(base);
	
	return 0;
}







