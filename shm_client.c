#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<unistd.h>


#define SHM_MEMORY  1024



int main(int argc, char **argv)
{
	int ret = 0;
	int shm_id = 0;
	key_t shm_key = 0;
	char *shm_addr = NULL;
	char buf[SHM_MEMORY] = {0};
	
	
	/*ftok key_t init*/
	shm_key = ftok("./tmp",0x125);
	if(0>shm_key)
	{
		perror("ftok");
		exit(-1);
	}
	
	/*shmget, get memary*/
	shm_id = shmget(shm_key,0,0);
	if(-1 == shm_id)
	{
		perror("shmget");
		exit(-1);
	}	
	
	
	/*shmat, attach memory*/
	shm_addr = (char *)shmat(shm_id,NULL,0);
	if(NULL == shm_addr)
	{
		perror("shmat");
		exit(-1);
	}
	
	printf("####%s \r\n",shm_addr);
	/*copy memory*/
	while(0)
	{
		
	}	
	
	
	/*detach with addr*/
	ret = shmdt(shm_addr);
	
	exit(0);
}

