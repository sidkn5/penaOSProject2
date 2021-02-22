#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define BUFFER_SIZE sizeof(int)


void sOption(){
	//execl();	

}
//Not Done
void help(){
	printf("Help: \n");

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

int main(int argc, char *argv[]){
	int option;		//option used by the user
	int noOfChildren;	//no. of children using the s option
	FILE *fp;
	int countInt = 0;	//count no. of int
	int shmid;
	int *shmPtr;
	int newCount;		//holds the power of two count
	key_t key;		//key to the shared memory 1234
	char c;
	int n;
	char padder[10];	//holds the int before transfering it to ai
	int shmind = 0;		//shm index

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

	printf("old count %d \n",countInt);
	newCount = powerOfTwo(countInt);
	printf("new count %d \n", newCount);
	fclose(fp);


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
	
	//open file and transfer ints to shared memory
	fp=fopen("nums.txt", "r");
	if (fp == NULL){
		perror("master: Error: There is a problem opening the file.");
		return 0;
	}else{
		while((c =fgetc(fp)) != EOF){
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
	fclose(fp);

	for(shmind = 0; shmind < newCount; shmind++){
		printf("shmptr: %d\n",shmPtr[shmind]);
	}
	//printf("this is id: %d", shmid);
	//memcpy(shmPtr, "Hello World",11);
	//
	//shmctl(shmid, IPC_RMID, 0);

	/////////////////////////////////////////////////////////////////////*/
	

	/*while((option = getopt(argc, argv, "hst:")) != 1){
		switch(option){
			case 'h':
			help();
			break;

			case 's':
			//noOfChildren = optarg;
			sOption();
			break;

			case 't'"

		}

	}*/

	
}
