#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define BUFFER_SIZE sizeof(int)

int main(int argc, char *argv){
	int shmid;
	int *shmPtr;
	key_t key;
	char *s;
	int newCount = 16;
	int shmind;

	key = ftok("./README.md", 'a');
	
	shmid = shmget(key, BUFFER_SIZE * newCount, 0666);
	printf("shmid: %d\n", shmid);

	if(shmid == -1){
		perror("bin_adder: Error: shmget error.");
	}
	printf("this is id %d", shmid);
	shmPtr = shmat(shmid, NULL, 0);

	if(shmPtr == (int *) -1){
		perror("bin_adder: Error: shmat error.");
		exit(1);
	}
	
	for(shmind = 0; shmind < newCount; shmind++){
		printf("shmptr: %d\n", shmPtr[shmind]);
	}


	return 0;

}
