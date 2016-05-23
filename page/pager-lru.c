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
//#include <queue.h>
#include "simulator.h"
#include  <limits.h>

static int timestamps[MAXPROCESSES][MAXPROCPAGES];
static int tick;
static int WINDOW=350;

FILE *fp;

void pager(Pentry q[MAXPROCESSES]){
	int f;
	int t;
	int min=INT_MAX;
	int minI=0;
	int minIT;
	int max=0;
	int maxI=0;

	for(f=0;f<MAXPROCESSES;f++){
		
		for(t=0;t<MAXPROCPAGES;t++){
			if(q[f].pages[t]){//if swapped in
				if(timestamps[f][t]<min){
					min=timestamps[f][t];
					minI=t;
					minIT=f;

				}
				if(timestamps[f][t]>max){
					max=timestamps[f][t];
					maxI=t;
				}

			}

			
			
		}
	if(tick-min>WINDOW){
		pageout(f,t);
	}
		min=INT_MAX;
	minI=0;
	minIT;
	max=0;
	maxI=0;

	}




}

void pageit(Pentry q[MAXPROCESSES]) { 
    //fp= fopen("data.txt", "a");
    /* This file contains the stub for an LRU pager */
    /* You may need to add/remove/modify any part of this file */
	//printf("%ld",processes[0]->pid);
    /* Static vars */
    static int initialized = 0;
    //static int tick = 1; // artificial time
    //static int timestamps[MAXPROCESSES][MAXPROCPAGES];
	//printf("here\n");
    /* Local vars */
    int proctmp;
    int pagetmp;
	int pc;
	int page;
	int i;
	int min;


		int swapRow=0;
		int swapCol=0;
		int tmppage;
		int proc;
    /* initialize static vars on first run */
    if(!initialized){
	for(proctmp=0; proctmp < MAXPROCESSES; proctmp++){
	    for(pagetmp=0; pagetmp < MAXPROCPAGES; pagetmp++){
				timestamps[proctmp][pagetmp] = 0; 
	    }
	}
	tick=1;
	initialized = 1;
    }
    
	

	for(proc=0; proc <MAXPROCESSES; proc++){
		//printf("1\n");	
		if(q[proc].active){
			//printf("2\n");
			pc = q[proc].pc;
			page=pc/PAGESIZE;
			//fprintf(fp, "%i \t %i \t %i \n",proc, page, q[proc].pages[page]);
			if(!q[proc].pages[page]){
				//printf("333333333333333333333\n");

				if(!pagein(proc,page)){
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
					if(pageout(proc,minIndex)){
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
					}
						
				}/*else{
					break;
					printf("test\n");//getchar();
				}*/
			}
			timestamps[proc][page] = tick;//come back
				


		}else{
			for(i=0;i<MAXPROCPAGES;i++){
				pageout(proc,i);
			}
		}
 		
	}


	//fclose(fp);	
    /* TODO: Implement LRU Paging */


    /* advance time for next pageit iteration */
    tick++;
	pager(q);
} 
