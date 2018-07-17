#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <event.h>


void Ontime(int sock, short event, void *arg)
{
	struct timeval tv;
	static int i = 0;
	printf("hello world i = %d \r\n",i++);

	tv.tv_sec = 1;
	tv.tv_usec = 0;
	event_add((struct event*)arg,&tv);
	
}



int main(int argc, char **argv)
{
	struct  event  ev_time;
 	struct  timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 1;
	

	
	event_init();

	evtimer_set(&ev_time,Ontime,&ev_time);
	
	event_add(&ev_time,&tv);		

	event_dispatch();

}


