#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFFER_SIZE sizeof(int)

int main(int argc, char *argv){
	int shmid;
	char *shmPtr;
	key_t key;
	char *s;
	int newCount = 16;

	key = 1234;
	shmid = shmget(key, BUFFER_SIZE * newCount, 0666);
	if(shmid < 0){
		perror("bid_adder: Error: shmget error.");
	}
	printf("this is id %d", shmid);
	shmPtr = shmat(shmid, NULL, 0);

	if(shmPtr == (char *) -1){
		perror("bin_adder: Error: shmat error.");
		exit(1);
	}

	printf("this is a test. \n");



	return 0;

}
