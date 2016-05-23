#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#define BUFFER_SIZE 1024
	
static char receive[BUFFER_SIZE];

int main(){
	int ret=-2;
	int device=-1;
	char userAnswer = 'r';
	char prevUserAnswer='r';
	char stringToSend[BUFFER_SIZE];

	printf("starting device tester \n");
	device=open("/dev/simple_character_device",O_RDWR);
	printf("here\n");
	if(device<0){
		printf("shit got fucked up\n");
		return 0;
	}
	//printf("messae to be sent to kernel module %d \n",device);
	//scanf("%[^n]%*c", stringToSend);
	//printf("scanf worked\n");
	//ret=write(device, stringToSend,strlen(stringToSend));

	//printf("%d\n",ret);
	//ret=read(device,receive,BUFFER_SIZE);
	//printf("%s\n",receive);

	while(userAnswer!='e'){
		printf("Please enter r to read, w to write, or e to exit\n");
		scanf(" %c",&userAnswer);
		
		if(userAnswer !='e'){

			if(userAnswer!= 'w' && userAnswer!= 'r'){
				userAnswer=prevUserAnswer;
			}
			prevUserAnswer=userAnswer;
			if(userAnswer=='w'){
				printf("messae to be sent to kernel module %d \n",device);
				scanf(" %[^n]%*c", stringToSend);
				ret=write(device, stringToSend,strlen(stringToSend));
				printf("scanf worked\n");
			}else{
				ret=read(device,receive,BUFFER_SIZE);
				printf("%s\n",receive);	
			}
		}
		
	}

	//printf("messae to be sent to kernel module %d \n",device);
	//scanf("%[^n]%*c", stringToSend);
	//printf("scanf worked\n");
	//ret=write(device, stringToSend,strlen(stringToSend));

	//printf("%d\n",ret);
	//ret=read(device,receive,BUFFER_SIZE);
	//printf("%s\n",receive);



	return 0;
	
}

