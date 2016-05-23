/*
 * File: pager-lru.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains an lru pageit
 *      implmentation.
 */

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
//#include <queue.h>
#include "simulator.h"
#include  <limits.h>




#define TRUE  1
#define FALSE 0
 static long pageIn[MAXPROCESSES][MAXPROCPAGES];
FILE *fp;
FILE *in;
FILE *fin;
FILE *out;
static int letsSee[MAXPROCESSES][5000];
static int intCount[MAXPROCESSES][MAXPROCPAGES];
int sumOf(int row){
	int sum=0;
	int i;
	for(i=0;i<MAXPROCPAGES;i++){

		sum=sum+intCount[row][i];

	}
	return sum;
}

int pagesAve=0;
//string fileName="Data.txt";

/*
int pagein(int process, int page) { 
    if (process<0 || process>=procs 
     || !processes[process]
     || !processes[process]->active
     || page<0 || page>=processes[process]->npages)
	return FALSE; 
    if (processes[process]->pages[page]>=0) 
	return TRUE; /* on its way */           /* 
    if (pagesavail==0) 
	return FALSE; 
    if (processes[process]->pages[page]>=-PAGEWAIT ) 
	return FALSE; /* not yet out */         /*
    sim_log(LOG_PAGE,"process=%2d page=%3d start pagein\n",process,page);
    if (pages) fprintf(pages,"%ld,%d,%d,%ld,%ld,coming\n",
	sysclock,process,page,processes[process]->pid, processes[process]->kind); 
    processes[process]->pages[page]=PAGEWAIT; pagesavail--; return TRUE; 
} 
*/

int writeToFile(){
	//remove(fileName);
	int proctmp;
	int pagetmp;
	fp= fopen("Data.txt", "w");
	for(proctmp=0; proctmp < MAXPROCESSES; proctmp++){
	//printf("here");
	    for(pagetmp=0; pagetmp < MAXPROCPAGES; pagetmp++){
			//("here1");
			fprintf(fp, "%i\t", intCount[proctmp][pagetmp]);
		}
		fprintf(fp, "\n");
	}

	fclose(fp);
	return 0;


}


int npageIn(int nprocces,int npage){
	if(nprocces <0 || nprocces >=MAXPROCESSES){
		return FALSE;
	}
	if(pageIn[nprocces][npage]>=0){
		return TRUE;
	}
	if(pagesAve==0){
		return FALSE;
	}
	if(pageIn[nprocces][npage]>=-PAGEWAIT){
		return FALSE;
	}
	pageIn[nprocces][npage]=PAGEWAIT;
	pagesAve--;
	return TRUE;

}
/*
int pageout(int process, int page) { 
    if (process<0 || process>=procs 
     || !processes[process]
     || !processes[process]->active
     || page<0 || page>=processes[process]->npages) 
	return FALSE; 
    if (processes[process]->pages[page]<0) 
	return TRUE; /* on its way out */ /*
    if (processes[process]->pages[page]>0) 
	return FALSE; /* not available to swap out */ /*
sim_log(LOG_PAGE,"process=%2d page=%3d start pageout\n",process,page);
    if (pages) fprintf(pages,"%ld,%d,%d,%ld,%ld,going\n",
	sysclock,process,page,processes[process]->pid, processes[process]->kind); 
    processes[process]->pages[page]=-1; return TRUE;
} 


*/

int npageOut(int nprocces, int npage){
	if(nprocces <0 || nprocces >=MAXPROCESSES){
		return FALSE;
	}
	if(pageIn[nprocces][npage]<0){
		return TRUE;

	}
	if(pageIn[nprocces][npage]>0){
		return FALSE;
	}
	pageIn[nprocces][npage]=-1;
	return TRUE;

}


void pageit(Pentry q[MAXPROCESSES]) { 
    in= fopen("pageindata.txt", "a");
    out=fopen("pageoutdata.txt", "a");
	fin=fopen("findata.txt","a");
    /* This file contains the stub for an LRU pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;

    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES];
  
	//printf("here\n");
    /* Local vars */
    int proctmp;
    int pagetmp;
	int pc;
	int page;
	int min;

static int lol;
		int swapRow=0;
		int swapCol=0;
		int tmppage;
		int proc;
    /* initialize static vars on first run */
    if(!initialized){
	for(proctmp=0; proctmp < MAXPROCESSES; proctmp++){
	    for(pagetmp=0; pagetmp < MAXPROCPAGES; pagetmp++){
				timestamps[proctmp][pagetmp] = 0; 
				intCount[proctmp][pagetmp] =0;
				pageIn[proctmp][pagetmp]=0;
				intCount[proctmp][pagetmp]=0;
	    }
	}
	lol=0;
	pagesAve=100;
	initialized = 1;
    }
    	//predicts allage()
	for(proctmp=0; proctmp < MAXPROCESSES; proctmp++){
	if(q[proctmp].active){
	    for(pagetmp=0; pagetmp < MAXPROCPAGES; pagetmp++){	
		if(pageIn[proctmp][pagetmp]!=0 && !(pageIn[proctmp][pagetmp]<-PAGEWAIT))			

			pageIn[proctmp][pagetmp]--;
			if(pageIn[proctmp][pagetmp]<-PAGEWAIT){

				pagesAve++;
			}

		}
	}

	}
	

	for(proc=0; proc <MAXPROCESSES; proc++){
		//printf("1\n");	
		if(q[proc].active){
			//printf("2\n");
			pc = q[proc].pc;
			page=pc/PAGESIZE;
			if(lol<5000){

				letsSee[proc][lol]=page;
			}
			lol++;
			//fprintf(fp, "%i \t %i \t %i \n",proc, page, q[proc].pages[page]);
			//fprintf(fin,"%i \t %i \t %i \t",tick, proc,page);
			if(!q[proc].pages[page]){
				
				//fprintf(fin, "0\n");
				//printf("333333333333333333333\n");
				//fprintf(in,"%i \t %i \t %i \t",tick, proc,page);
				if(!pagein(proc,page)){
					//fprintf(in, "0\n");
					/*if(!npageIn(proc,page)){
						printf("page in workerd");


					}else{

						printf("npagein didnt work");getchar();


					}*/
					//int print=sumOf(proc);
					//printf("page in failed at %i\n",pagesAve);
					//printf("44444444444444444444444444444444444444\n");
					//getchar();
					min=INT_MAX;
					int minIndex=0;

					//printf("now\n");
	   			 	for(pagetmp=0; pagetmp < MAXPROCPAGES; pagetmp++){
						//printf("now3\n");
						if((page!= pagetmp) && (timestamps[proc][pagetmp]>0) && (timestamps[proc][pagetmp]<min)){

							min=timestamps[proc][pagetmp];

							minIndex=pagetmp;


						}
					}
					//fprintf(out, "%i \t %i \t %i \t",tick,proc,page);
					if(pageout(proc,minIndex)){
						//fprintf(out,"1\n");
						intCount[proc][page]++;
						/*if(npageOut(proc,minIndex)){

							printf("workerd");
						}else{printf("page out didnt work");getchar();}*/
						//printf("%i\n",timestamps[proc][minIndex]);
						timestamps[proc][minIndex]=0;
						//printf("awwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww %i\n",minIndex);
						//if(!pagein(proc,page)){
							//printf("failed");
							//getchar(); 
						//}else{
							//printf("suc\n");getchar();break;
						//}
						//printf("here");
						//getchar();
					}else{
						//fprintf(out,"0\n");
						/*if(!npageOut(proc,minIndex)){

							printf("page out workerd");
						}else{printf("page out didnt work");getchar();}*/				
					}		
				}else{
					//fprintf(in, "1\n");
					intCount[proc][page]++;
					/*if(npageIn(proc,page)){
						printf("page in workerd");


					}else{

						printf("npagein didnt work");getchar();


					}
					
		
				}*/
/*else{
					break;
					printf("test\n");//getchar();
				}*/
				}
			}else{//fprintf(fin, "1\n");
				}
			timestamps[proc][page] = tick;//come back
				


		}
 		
	}
	int new;
	int newnew;
	if(lol==5000){
		printf("hre");
		for(newnew=0;newnew < MAXPROCESSES;newnew++){
			for(new=0;new<5000;new++){
				fprintf(fin,"%i \t",letsSee[newnew][new]);
			}
			fprintf(fin,"\n");
		}

	}

	//fclose(fp);	
    /* TODO: Implement LRU Paging */
	fclose(out);
	fclose(in);
	fclose(fin);
	//writeToFile();
    /* advance time for next pageit iteration */
    tick++;
} 
