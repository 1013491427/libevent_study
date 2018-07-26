#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<unistd.h>
#include<time.h>


#define SHM_MEMORY  1024



int main(int argc, char **argv)
{
	int ret = 0;
	int shm_id = 0;
	key_t shm_key = 0;
	char *shm_addr = NULL;
	time_t  tv;
	struct tm  *time_tmp = NULL;
	char time_buf[100] = {0};
	
	
	/*ftok key_t init*/
	shm_key = ftok("./tmp",0x125);
	if(0>shm_key)
	{
		perror("ftok");
		exit(-1);
	}
	
	/*shmget, get memary*/
	shm_id = shmget(shm_key,SHM_MEMORY,IPC_CREAT|IPC_EXCL|0666);
	if(-1 == shm_id)
	{
		perror("shmget");
		exit(-1);
	}	
	printf("shmid is %d \r\n",shm_id);	
	
	/*shmat, attach memory*/
	shm_addr = (char *)shmat(shm_id,NULL,0);
	if(NULL == shm_addr)
	{
		perror("shmat");
		exit(-1);
	}
	
	/*copy something to memory*/
	strcpy(shm_addr,"i love program very much");
	
	tv = time(0);
	
	while(0)
	{
		memset(shm_addr,0,1024);
		tv = time(0);
		time_tmp = localtime(&tv);
		memset(time_buf,0,100);
		sprintf(time_buf,"hour:%d,min:%d:sec:%d",time_tmp->tm_hour,time_tmp->tm_min,time_tmp->tm_sec);
		memcpy(shm_addr,time_buf,strlen(time_buf));
		
	}
	
	/*detach with addr*/
	ret = shmdt(shm_addr);
	
	exit(0);
}

