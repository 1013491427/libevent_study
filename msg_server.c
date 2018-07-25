#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<time.h>
#include<string.h>


#define TEXT_SIZE   1024

typedef struct msg_buf
{
	long msg_type;
	int  status;
	char time[100];
	char mtext[TEXT_SIZE];
}msgbufStr;


int main(int argc,char **argv)
{
	key_t key_msg = 0;
	int msgid = 0;	
	int msg_len = 0;
	int ret = 0;
	time_t tv;
	struct tm  *time_tmp;
	
	msgbufStr  msg_buf;
	
	/*init msg_id*/
	key_msg = ftok("./tmp",0x01);
	printf("ftok key_msg is %d\r\n",key_msg);
	msgid = msgget(key_msg,0666|IPC_CREAT);
	printf("msgget msgid is %d\r\n",msgid);
	if(msgid < 0)
	{
		printf("msgget error \r\n");
		exit(-1);
	}	
	
	
	
	msg_buf.msg_type = 3;
	strcpy(msg_buf.mtext,"send first msg");
	msg_len = sizeof(msgbufStr) - sizeof(long);
	
	msgsnd(msgid,&msg_buf,msg_len,0);
	memset(msg_buf.time,0,100);
	memset(msg_buf.mtext,0,TEXT_SIZE);
	
	while(1)
	{
		sleep(1);
		tv = time(0);
		time_tmp = localtime(&tv);
		sprintf(msg_buf.time,"hour:%02d,minute:%02d,minute:%02d",time_tmp->tm_hour,time_tmp->tm_min,time_tmp->tm_sec);
		ret = msgsnd(msgid,&msg_buf,msg_len,0);
		printf("ret is %d,  %s,###buf is %s\r\n",ret,msg_buf.time,msg_buf.mtext);
		memset(msg_buf.time,0,100);
		memset(msg_buf.mtext,0,TEXT_SIZE);
		strcpy(msg_buf.mtext,"send first msg");
	}
	
	return 0;
} 


