
/*
 *Student: Sean Dela Pena
 *Professor: Dr. Sanjiv Bhatia
 *Assignment2: Concurrent Unix Processes and shared memory
 *github: github.com/sidkn5
 *Date: 2/25/2021
 *
*/

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
	if(depth == 0){
		return 1;
		
	}else {
		adderIndex = pow(2, depthDiff);
		return adderIndex;
	}
	return adderIndex;
}

int main(int argc, char *argv[]){
	int shmid;				
	int *shmPtr;
	key_t key;
	char *s;
	int index = atoi(argv[1]);		//index given by execl call
	int newCount = atoi(argv[3]);		//count of the integers given by execl call
	int shmind;
	int adderIndex;				//index that will be added to the first index
	int depth = atoi(argv[2]);		//depth of tree given by execl call
	int originalDepth;			//will hold the original depth of the tree

	//printf("binadder depth %d,  index  %d\n   newcount  %d", depth, index, newCount);
	//connect to shared memory created by main
	key = ftok("./README.md", 'a');
	
	shmid = shmget(key, BUFFER_SIZE * newCount, 0666);

	if(shmid == -1){
		perror("bin_adder: Error: shmget error.");
	}
	shmPtr = shmat(shmid, NULL, 0);

	if(shmPtr == (int *) -1){
		perror("bin_adder: Error: shmat error.");
		exit(1);
	}
	
	////////////////////////////////////////////////////////////
	//

	//check to display the integers
	/*for(shmind = 0; shmind < newCount; shmind++){
		printf("shmptr: %d\n", shmPtr[shmind]);
	}*/

	originalDepth = getDepth(newCount);
	adderIndex = index + depthIndex(originalDepth, depth);

	shmPtr[index] = shmPtr[index] + shmPtr[adderIndex];
	//printf("sum: %d indexFINAL:%d \n", shmPtr[index], index);
	shmdt(shmPtr);
	return 0;

}
