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
	char *shmPtr;
	int newCount;		//holds the power of two count
	key_t key;
	char c;
	int n;

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


	//make shared memory
	key = 1234;
	shmid = shmget(key, BUFFER_SIZE * newCount, IPC_CREAT | 0666);	
	if(shmid < 0 ){
		perror("master: Error: shmget error, creation failure.");
		exit(1);
	}

	//attach to shared memory
	shmPtr = shmat(shmid, NULL, 0);
	if (shmPtr == (char *) -1){
		perror("master: Error: smat error, attachment failure.");
		exit(1);
	}
	
	//open file and transfer to shared memory
	fp=fopen("nums.txt", "r");
	if (fp == NULL){
		perror("master: Error: There is a problem opening the file.");
		return 0;
	}else{
		/*for(n = getc(fp); n != EOF; n = getc(fp)){
			if (n != '\n'){
				//memcpy(shmPtr, n, sizeof(int));
				memcpy(shmPtr, n, 1);
			}
		}*/
	}
	fclose(fp);
	printf("this is id: %d", shmid);
	memcpy(shmPtr, "Hello World",11);

	char *s;
	s = shmPtr;
	s += 11;
	*s = 0;
	//////////////////////////////////////////////////////////////////////*/
	

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

	
	//printf("count %d", countInt);

}
