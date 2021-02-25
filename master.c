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
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>
#include <errno.h>
#define BUFFER_SIZE sizeof(int)

/////////////////////////////////////////////////////////////////////////////
//Sources:
//stackoverflow.com/questions/2377811/tracking-the-death-of-a-child-process
//powerof2func: geeksforgeeks.com
//SIGALRM: linuxhint.com/sigalarm_alarm_c_language/
//SharedMemory: tutorialspoint.com
//execl: linux.die.net/man/3/execl
//
//
//
//
//
//
/////////////////////////////////////////////////////////////////////////////


int noOfChildren = 20;	//no. of process using the s option 20 default, 19 children 1 parent/main
int timeTermination = 100; //time given until termination 100 default
pid_t *pidList;
int pidCount = 0;
int shmid;		//holds the shmid
int *shmPtr;		//pointer for the shared memory

//Not Done
void help(){
	printf("Help: \n");
	printf("Usage: ./master [-h] [-s i] [-t time] datafile \n");
	printf("This program gets the sum of all the numbers in the given datafile, \n");
	printf("by the use of bin_adder program accessing a shared memory. \n");
	printf("Example: ./master -s 10 nums.txt \n");
	printf("Options:  \n");
	printf("-h\t\t displays help information on how this program is to be called.\n");
	printf("-s\t\t takes an argument i, which sets the number of processes that are \n");
	printf("  \t\t allowed to run at a time. The default and max is 20, so given  \n");
	printf("  \t\t i, i-1 child processes is allowed. \n");
	printf("-t\t\t takes an argument time in secs, this will set the time for how \n");
	printf("  \t\t long the program will run. It will terminate after the given time. \n");


}


//makes the number of integers in the file a power of 2
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

//gets the depth of the tree
int getDepth(int n){
	int depth = log(n) / log(2);
	return depth;
}

int checkProcess(){
	int n = 0;
	int i;
	for(i = 0; i < noOfChildren; i++){
		if(pidList[i] != 0){
			n = 1;
			return n;
		}
	}
	return n;

}

//checks which index is available for use
int freeIndex(){
	int i;
	for (i =0; i < noOfChildren; i++){
		if(pidList[i] == 0){
			return i;
		}

	}
}


//checks the number of processes given is valid, defaults to 20, 19 children
int checkSArgument(int n){
	n--;
	if(n > 19){
		printf("The maximum allowed number of children is 19. \n");
		n = 19;
		return n;
	} else {
		return n;
	}
}

//referred to stackoverflow
void resetPid(pid_t pid){
	int i;
	for(i = 0; i < noOfChildren; i++){
		if(pidList[i] == pid){
			pidList[i] = 0;
		}
	}
}
//referred to stackoverflow
void mySigchldHandler(int sig ){
	pid_t p;
	int status;

	while((p=waitpid( -1, &status, WNOHANG))!= -1){
		pidCount--;
		resetPid(p);
	}
}

//traverse the pidList and kill all
void killPid(){
	int i;
	for (i = 0; i < noOfChildren; i++){
		if(pidList[i] == 0){
			//already dead
		}else{
			kill(pidList[i], SIGKILL);
		}
	}
}


//when ctrl+c is hit process will terminate and clean up
void ctrlC(){
	printf("Process terminated.\n");
	//don't forget to add kill and free all later 
	shmdt(shmPtr);
	shmctl(shmid, IPC_RMID, NULL);
	killPid();
	free(pidList);
	exit(0);
}

//when the time given is up, process will terminate and clean up
void timesUp(){
	printf("The time given is up. Process will termintate.\n");
	//don't forget to add kill and free all later 
	shmdt(shmPtr);
	shmctl(shmid, IPC_RMID, NULL);
	killPid();
	free(pidList);
	exit(0);
}

int main(int argc, char *argv[]){
	int option;		//option used by the user
	FILE *fp1;		//file pointer
	int countInt = 0;	//count no. of int
	int newCount;		//holds the power of two count
	key_t key;		//key to the shared memory 1234
	char c;		
	int n;
	char padder[10];	//holds the int before transfering it to ai
	int shmind = 0;		//shm index
	int depth = 0;		//depth of the tree, get using log2
	int allowedNumber = 19; //will be set by the user, otherwise default to 19
	char *datafile;		//datafile given by the user

	//Signals
	signal(SIGINT, ctrlC);		//catches ctrl+c
	signal(SIGALRM, timesUp);	//terminates the program when time given is up

	//referred to stackoverflow
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = mySigchldHandler;
	sigaction(SIGCHLD, &sa, NULL);

	////////////////////////////////////////////////////////////////////*/
	
	//If no arguments given, refer the user to the help menu
	if(argc == 1){
		printf("master: Error: Please refer to -h help menu for proper use of program.\n");
		exit(1);
	}


	//////////////////////////////////////////////////////////////////////
	//getopt for options
	while((option = getopt(argc, argv, "hs:t:")) != -1){
		switch(option){
			case 'h':
				help();
				return 0;
				break;

			case 's':
				noOfChildren = atoi(optarg);
				allowedNumber = checkSArgument(noOfChildren);
				printf("No. of children allowed: %d\n", allowedNumber);
				datafile = argv[3];
				break;

			case 't':
				timeTermination = atoi(optarg);
				datafile = argv[3];

				printf("Will terminate in %d sec(s)\n", timeTermination);
				alarm(timeTermination);
				break;

			default:
				printf("Please refer to the help menu for proper use.\n");
				exit(0);

		}

	}
	//sleep(10);		//test the alarm
	///////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////////
	
	//open file and count the number of int
	fp1=fopen(datafile, "r");
	if (fp1 == NULL){
		errno = 2;
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
	newCount = powerOfTwo(countInt);	

	/////////////////////////////////////////////////////////////////////



	////////////////make shared memory///////////////////////////////////
	key = ftok("./README.md", 'a');

	shmid = shmget(key, BUFFER_SIZE * newCount, IPC_CREAT | 0666);
	if(shmid < 0 ){
		perror("master: Error: shmget error, creation failure.");
		exit(1);
	}
	//attach to shared memory
	shmPtr = shmat(shmid, NULL, 0);
	if (shmPtr == (int *) -1){
		perror("master: Error: smat error, attachment failure.");
		exit(1);
	}

	/////////////////////////////////////////////////////////////////////
	
	//set up the array and add 0s
	//open file and transfer ints to shared memory
	fp1=fopen(datafile, "r");
	if (fp1 == NULL){
		perror("master:Error: There is a problem opening the file.");
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
	////////////////////////////////////////////////////////////////////////
	//
	//
	////////////////////////////////////////////////////////////////////////

	//where the forking and execl happens//////////////////////////////////
	depth = getDepth(newCount);
	shmind = 0;
	//needed because execl doesn't take int
	char shmind1[10];
	char depth1[10];
	char newCount1[10];
	sprintf(newCount1, "%d", newCount);

	int i, x,z;
	int indexIncrement=1;
	int forkID;
	int pidCount = 0;
	int pidIndex;
	bool depthComplete = false;
	pidList = malloc(sizeof(pid_t) * noOfChildren);
	int powerOf = 1;
	int tempPid;
	int forCounter = 0;
	int runAgain = 0;

	//go through each depth
	for (i = depth; i > 0; i--){
			
		while(pidCount != 0){
	
			if(checkProcess() != 0){
			pidCount = 0;}
		}
		
		depthComplete = false;
		indexIncrement = pow(2, powerOf);
		sprintf(depth1, "%d", i);
		printf("\n\nAdding depth: %d \n", i);
		do {
			if(pidCount == 0){

				//counts the number of times the execl should run
				for (z = 0; z < newCount; z+= indexIncrement){
					forCounter++;
				}

				//go through array
				for (x =0; x < newCount; x += indexIncrement){
					sleep(1);
					if (pidCount < allowedNumber){
						pidCount++;
						sprintf(shmind1, "%d", x);
						pidIndex = freeIndex();
						pidList[pidIndex] = fork();
							
						if(pidList[pidIndex] == 0){
							sprintf(depth1, "%d", i);
							//printf("Adding depth: %d \n", depth1);
							execl("./bin_adder", "./bin_adder", shmind1, depth1, newCount1, NULL);
						}
					}
				
					if(pidCount == allowedNumber){
						runAgain = forCounter - pidCount;
						for(z=0; z < runAgain; z++){
							if(pidCount > 0)
								pidCount--;
							
						}
					}	

				}
				//1 depth done go back
				depthComplete = true;
			}

		}while(!depthComplete);
		powerOf++;		//increment the power for the index in current depth
	}


	////////////////////////////////////////////////////////////////////////

	
	sleep(2);
	printf("TOTAL SUM: %d\n", shmPtr[0]);	
	shmdt(shmPtr);
	shmctl(shmid, IPC_RMID, NULL);
	free(pidList);
	return 0;	
}
