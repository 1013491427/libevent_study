#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>

union semun
{
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                               (Linux-specific) */
};


int sem_P(int semid, int sennum)
{
	struct sembuf sops = {sennum,-1,SEM_UNDO};
	return semop(semid,&sops,1);
}

int sem_V(int semid, int sennum)
{
	struct sembuf sops = {sennum,+1,SEM_UNDO};
	return semop(semid,&sops,1);
}


int main(int argc,char **argv)
{
	key_t  sem_key = 0;
	int semid = 0;
	int ret = 0;
	union semun arg;
	
	/*ftok get key*/
	sem_key = ftok("./tmp",0x123);
	if(0>sem_key)
	{
		perror("ftok");
		exit(-1);
	}		
	
	/*semget  get id*/
	semid = semget(sem_key, 3, IPC_CREAT|0666);
	if(-1 == semid)
	{
		perror("semget");
		exit(-1);
	}
	
	/*set init valut to sem*/
	arg.val = 1;
	ret = semctl(semid,0,SETVAL,arg);
	if(0>ret)
	{
		perror("semctl");
		semctl(semid,0,IPC_RMID,arg);
		exit(-1);
	}
	
	/*get sem val*/
	ret = semctl(semid,0,GETVAL,arg);
	printf("semctl get val is %d\r\n",ret);
	
	/*semop P operation*/
	ret = sem_P(semid, 0);
	/*get sem val*/
	ret = semctl(semid,0,GETVAL,arg);
	printf("after P operation semctl get val is %d\r\n",ret);
	
	
	/*semop V operation*/
	ret = sem_V(semid, 0);
	/*get sem val*/
	ret = semctl(semid,0,GETVAL,arg);
	printf("after V operation semctl get val is %d\r\n",ret);
}


