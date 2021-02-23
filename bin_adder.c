#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define BUFFER_SIZE sizeof(int)

int getDepth(int n){
	int depth = log(n) / log(2);
	return depth;
}

int depthIndex(int ogDepth, int depth){
	//depth 5, index+1
	//depth 4, index+2
	//	3, index+4
	//	2, index+8
	//	1
	//
	int depthDiff = ogDepth - depth;
	int adderIndex = 0;
	printf("diff: %d\n", depthDiff);
	if(depth == 0){
		return 1;
		
	}else {
			adderIndex = pow(2, depthDiff);
	}
	printf("newadder: %d\n", adderIndex);
	return adderIndex;
}

int main(int argc, char *argv[]){
	sleep(2);
	int shmid;
	int *shmPtr;
	key_t key;
	char *s;
	int index = atoi(argv[1]);
	int newCount = atoi(argv[3]);
	int shmind;
	int adderIndex;
	int depth = atoi(argv[2]);
	int originalDepth;

	//connect to shared memory created by main
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
	
	////////////////////////////////////////////////////////////
	//
	
	/*for(shmind = 0; shmind < newCount; shmind++){
		printf("shmptr: %d\n", shmPtr[shmind]);
	}*/
	originalDepth = getDepth(newCount);
	adderIndex = index + depthIndex(originalDepth, depth);
	printf("index: %d \n", adderIndex);

	shmPtr[index] = shmPtr[index] + shmPtr[adderIndex];
	
	printf("sum: %d \n", shmPtr[index]);
	return 0;

}
