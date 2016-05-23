#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	
#include <string.h>
#include <time.h>
#include "queue.h"
#include "util.h"
#include "multi-lookup.h"
//sbufsize is buffer size go back and check this latter
//dnslookupall()?

pthread_mutex_t queueLock;
pthread_mutex_t	outputLock;
pthread_mutex_t	countLock;

queue hostNames;
char *outputFile;

int requesterCount = 0;
int queueCountReq=0;
char* outputFileName;
int queueCount=0;

void *requester(void* inputFile){

	
	char* inputFileName=inputFile;
	//printf("in requester %s\n",inputFileName);
	FILE* input= NULL;
	input = fopen((char*)inputFileName,"r");
	char* hostName= malloc(BUFFERSIZE*sizeof(char));

	//printf("mallc\n");
	if(!input){
		printf("Error in opening file: %s\n",inputFileName);
		exit(EXIT_FAILURE);

	}
	//printf("before while\n");
	while(fscanf(input,INPUTFS,hostName)>0){
		//printf("In while\n");
		int full=QUEUE_FAILURE;
		//printf("Adding Hostname %s\n", hostName); 


		char *temp=strdup(hostName);

		while(full==QUEUE_FAILURE){
			pthread_mutex_lock(&queueLock);
			full=queue_push(&hostNames,(void*)temp);
			queueCount++;
			printf("Requester Thread added %d hostnames to the queue.\n",queueCount);
			pthread_mutex_unlock(&queueLock);
			

			if(full==QUEUE_FAILURE){

				usleep(rand()%100000);
			}
		}
	}
	//printf("After While loop\n");
	fclose(input);

	free(hostName);
	return NULL;

}




void *resolver(){

	FILE* outputFile=NULL;     
	char* hostName=NULL;
	char ipAddress[INET6_ADDRSTRLEN];
	//printf("in resolver");
	while(1){

		pthread_mutex_lock(&queueLock);
		hostName=queue_pop(&hostNames);
		queueCountReq++;
		printf("Resolver Thread processed %d hostnames to the queue.\n",queueCountReq);
		pthread_mutex_unlock(&queueLock);
		if(hostName!=NULL){
			//int count=0;
			//ipAddress=malloc(255*sizeof(char*))
			
				if(dnslookup(hostName,ipAddress,sizeof(ipAddress))==UTIL_FAILURE){
					fprintf(stderr,"dnslookup failed %s\n",hostName);
				}/*else{
					printf("dnslookup worked %s\n",hostName);
				}*/
			//printf("%s",ipAddress);
		
			pthread_mutex_lock(&outputLock);

			outputFile= fopen(outputFileName,"a");
		
			if(!outputFile){

				printf("Error in Opening Output File");
				exit(EXIT_FAILURE);
			}
			fprintf(outputFile, "%s,%s\n",hostName,ipAddress);

		/*for(int i =0; i < addressCount; i++){

			fpintf(outputFile," - %s",ipArray[i]);
			if(ipArray[i]!=NULL){
				free(ip(Array));  
			}
		}*/
		//fprintf(outputFile, "\n");

		//if(ipArray!=NULL){
		//	free(ipArray);
		//}
			fclose(outputFile);
			pthread_mutex_unlock(&outputLock);
			free(hostName);

		}

	}


	return NULL;


}







int main(int argc, char *argv[]){
	//printf("inside of main\n");
	(void)argc;
	(void) argv;
	
	int fileCount=argc-2;
	//printf("1\n");
	outputFileName=argv[argc-1];
	//printf("here\n");
	queue_init(&hostNames, QUEUEMAXSIZE);
	//printf("2\n");

	pthread_mutex_init(&queueLock,NULL);
	pthread_mutex_init(&outputLock,NULL);
	pthread_mutex_init(&countLock,NULL);
	//printf("3\n");
	srand(time(NULL));
	//int numCores = sysconf( _SC_NPROCESSORS_ONLN ) * 2;
    	int numThread = /*(numCores < MAX_RESOLVER_THREADS) ? numCores :*/ MAX_RESOLVER_THREADS; 	
	int i;
	//printf("4\n");
	pthread_t resolverT[MAX_RESOLVER_THREADS];
	pthread_t requesterT[fileCount];
			

	//create theads
	//printf("here\n");
	for(i=0;i<fileCount;i++){
		pthread_create(&(requesterT[i]), NULL, requester, (void*) argv[i+1]);
		
	}

	for(i =0;i <numThread;i++){
		pthread_create(&(resolverT[i]),NULL, resolver,NULL);
	
	}
	



	//free everyting
	for(i=0;i<numThread;i++){
		pthread_join(resolverT[i],NULL);

	}

	for(i =0; i <fileCount;i++){
		pthread_join(requesterT[i],NULL);
	}
	

	pthread_mutex_destroy(&queueLock);

	pthread_mutex_destroy(&outputLock);
	queue_cleanup(&hostNames);

	return 0;
}


