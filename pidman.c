#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MIN_PID 300
#define MAX_PID 5000

//Prototypes:
void *run(void *p);
int allocate_map(void);
int allocate_pid(void);
void release_pid(int pid);
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;	

//Global variables:
int arr[MAX_PID + 2];



int main(int argc, char *argv[]){
	//initialize variables
	int returnValue,numthreads,i;
	pthread_t *tptr;
	srand(time(NULL));
	
	//allocate map and handle error if it occurs
	returnValue = allocate_map();
	if(returnValue == -1){
		printf("--Error occurred when allocating map--\n");
		exit(-1);
	}
	
	//get the number of threads from the command line
	if (argc > 1)
		numthreads = atoi(argv[1]);
	else
		numthreads = 2;
	
	//declare the thread array of the correct size
	tptr = malloc(sizeof(pthread_t) * numthreads);
	
	//create the threads, handling an error if it arises
	for (i = 0;i < numthreads;i++){
		returnValue = pthread_create(tptr + i,NULL,run,NULL);
		if (returnValue != 0){
			printf("Thread creation failed with return value %d\n", returnValue);
			exit(-1);
		}
	}
	//allow the threads to terminate
	for (i=0; i<numthreads; ++i)
		pthread_join(*(tptr+i), NULL);
	
	free(tptr);
}



//Request a pid, sleep for a random period of time, and then release the pid
void *run(void *p){
	int pid;
	
	//request pid
	pid = allocate_pid();
	while(pid == -1){
		sleep(.5);
		pid = allocate_pid();
	}
	//sleep random time
	sleep((rand() % 9) + 2);
	
	//release pid
	release_pid(pid);
	
	pthread_exit(0);
}

//Creates and initializes a data structure for representing pids;
//returns -1 if unsuccessful, 1 if successful
int allocate_map(void){
	if(arr){
		int i;
		for(i = 0;i <= MAX_PID;i++)
			arr[i] = 0;
		return 1;
	}
	else
		return -1;
}

//Allocates and returns a pid;
//returns -1 if unable to allocate a pid (all pids are in use)
int allocate_pid(void){
	int i;
	for(i = MIN_PID;i <= MAX_PID;i++){
		
		//start of critical section
		pthread_mutex_lock(&count_mutex);
		
		if(arr[i] == 0){
			arr[i] = 1;
			
			pthread_mutex_unlock(&count_mutex);	
			//end critical section
			
			printf("Allocated %i\n",i);
			return i;
		}
		pthread_mutex_unlock(&count_mutex);	
		//end critical section
		
	}
	return -1;
}

//Releases a pid
void release_pid(int pid){
	arr[pid] = 0;
	printf("Released %i\n",pid);
}