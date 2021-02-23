#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#define BUFFER_SIZE sizeof(int)


void sOption(){
	//execl();	

}
//Not Done
void help(){
	printf("Help: \n");

}
void ctrlC(){
	printf("Process terminated.\n");
	exit(0);
}

void timesUp(){
	printf("The time given is up. Process will termintate.\n");
	exit(0);
}

int powerOfTwo(int n){
	if(n>0 && !(n & (n-1))){
		return n;
	}else {
		while(!(n>0 && !(n & (n-1)))){
			n++;
		}
		return n;
	}
}
/*
int countIntFile(){
	int countInt = 0;
	char c;
	FILE *fp;
	fp=fopen("nums.txt", "r");
	if (fp == NULL){
		perror("master: Error: There is a problem opening the file.");
		return 0;
	}else{
		for(c = getc(fp); c != EOF; c = getc(fp)){
			if (c == '\n'){
				countInt++;
			}
		}
	}
	fclose(fp);
	return countInt;
}*/
int main(int argc, char *argv[]){
	int option;		//option used by the user
	int noOfChildren = 20;	//no. of children using the s option 20 default
	int timeTermination = 100; //time given until termination 100 default
	FILE *fp1;
	int countInt = 0;	//count no. of int
	int shmid;
	int *shmPtr;
	int newCount;		//holds the power of two count
	key_t key;		//key to the shared memory 1234
	char c;
	int n;
	char padder[10];	//holds the int before transfering it to ai
	int shmind = 0;		//shm index


	//Signals
	signal(SIGINT, ctrlC);
	signal(SIGALRM, timesUp);
	
	///////////////////////////////////////////////////////////////////////
	
	//open file and count the number of int
	

	fp1=fopen("nums.txt", "r");
	if (fp1 == NULL){
		perror("master: Error: There is a problem opening the file.");
		return 0;
	}else{
		for(c = getc(fp1); c != EOF; c = getc(fp1)){
			if (c == '\n'){
				countInt++;
			}
		}
	}
	fclose(fp1);
	//countInt = 12;
	//countInt = countIntFile();
	printf("old count %d \n",countInt);
	newCount = powerOfTwo(countInt);
	printf("new count %d \n", newCount);

	/////////////////////////////////////////////////////////////////////
	
	//getopt for options
	printf("before while\n");	
	while((option = getopt(argc, argv, "hs:t:")) != -1){
		switch(option){
			case 'h':
				help();
				break;

			case 's':
				noOfChildren = atoi(optarg);
				printf("no of children %d\n", noOfChildren);
				break;

			case 't':
				timeTermination = atoi(optarg);
				printf("time %d\n", timeTermination);
				alarm(timeTermination);
				break;

			default:
				printf("Please refer to the help menu for proper use.\n");
				exit(0);

		}

	}
	printf("after while\n");	
	//sleep(10);		test the alarm
	/////////////////////////////////////////////////////////////////////
	

	////////////////make shared memory///////////////////////////////////
	key = ftok("./README.md", 'a');

	printf("key  %d \n", key);
	shmid = shmget(key, BUFFER_SIZE * newCount, IPC_CREAT | 0666);
	if(shmid < 0 ){
		perror("master: Error: shmget error, creation failure.");
		exit(1);
	}
	printf("shmid %d\n", shmid);
	//attach to shared memory
	shmPtr = shmat(shmid, NULL, 0);
	if (shmPtr == (int *) -1){
		perror("master: Error: smat error, attachment failure.");
		exit(1);
	}

	/////////////////////////////////////////////////////////////////////
	
	//set up the array and add 0s
	//open file and transfer ints to shared memory
	fp1=fopen("nums.txt", "r");
	if (fp1 == NULL){
		perror("master: Error: There is a problem opening the file.");
		return 0;
	}else{
		while((c =getc(fp1)) != EOF){
			
			if(c != '\n'){
				//printf("padder %d\n",c);
				padder[n] = c;
				n++;

			} else {
				padder[n] = '\0';
				//reads the padder and converts the string to an int
				shmPtr[shmind] = atoi(padder);
				shmind++;
				n = 0;
			}
		}

	}	


	
	//set the rest to zeros
	for(countInt; countInt < newCount; countInt++){
		shmPtr[shmind] = 0;
		shmind++;
	}
	fclose(fp1);

	for(shmind = 0; shmind < newCount; shmind++){
		printf("shmptr: %d\n",shmPtr[shmind]);
	}
	///////////////////////////////////////////////////////////////////////
	/* TRYING OUT FORK
	int forkID = fork();
	if(forkID == 0){
	}else{
		printf("I'm in bin adder. \n");
	}
	for(shmind = 0; shmind < newCount; shmind++){
		printf("shmptr: %d\n",shmPtr[shmind]);
	}
	//
	//shmctl(shmid, IPC_RMID, 0);

	/////////////////////////////////////////////////////////////////////*/
	
	shmctl(shmid, IPC_RMID, NULL);
	return 0;
	
}
