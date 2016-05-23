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

	worked with Spencer Wilson
	used: github.com  account sean rivera used pager-lru
 */

#include <stdio.h> 
#include <stdlib.h>
//#include <queue.h>
#include "simulator.h"
#include  <limits.h>
static int WINDOW=350;

static int timestamps[MAXPROCESSES][MAXPROCPAGES];
static int pageLastRemovedAt[MAXPROCESSES][MAXPROCPAGES];
static int lastPageCalled[MAXPROCESSES];
static int revelentPageFreq[MAXPROCESSES][MAXPROCPAGES][MAXPROCPAGES];
static int betRevlentPage[MAXPROCESSES][MAXPROCPAGES][2];


static int tick;
static int prevtick;
static int numberofpagesout=0;

int arrayMin(Pentry q[MAXPROCESSES],int nproc,int npage,int *minPageTwo){
	int i;
	int min=INT_MAX;
	int minp=-100;
	int minPTwo=-100;
	int minTwo=INT_MAX;
	for(i=0; i < MAXPROCPAGES;i++){
		if(revelentPageFreq[nproc][npage][i]<min && i!=npage && q[nproc].pages[i]){
			minPTwo=minp;
			minTwo=min;
			min=revelentPageFreq[nproc][npage][i];
			minp=i;
		}else if(revelentPageFreq[nproc][npage][i]<minTwo && i !=npage && q[nproc].pages[i]){
			minTwo=revelentPageFreq[nproc][npage][i];
			minPTwo=i;

		}
	}
	*minPageTwo=minPTwo;
	
	return minp;
}


int arrayMax(int nproc, int npage,int *secondMax){
	int i;
	int max=INT_MIN;
	int maxp=-100;
	int secMax=INT_MIN;
	int secMaxp=-100;
	for(i=0; i <MAXPROCPAGES;i++){
		if(revelentPageFreq[nproc][npage][i]>max && i!=npage){

			secMax=max;
			secMaxp=maxp;
			max=revelentPageFreq[nproc][npage][i];
			maxp=i;



		}else if(revelentPageFreq[nproc][npage][i]>secMax && i!=npage){
			secMax=revelentPageFreq[nproc][npage][i];
			secMaxp=i;
			
		}
		
	}
	*secondMax=secMaxp;
	return maxp;
}


void effChecker(int z, int page){
	int curOut=pageLastRemovedAt[z][page];
	int y;
	int in=0;
	int mtick=0;
	    for(y=0; y < MAXPROCPAGES; y++){
		if(y!=page && timestamps[z][y]>mtick){
			mtick=timestamps[z][y];
		}
		if(timestamps[z][y]!=0 && timestamps[z][y]<curOut){
			
			if(tick-100>prevtick){prevtick=tick;}
			printf("cur tick is: %i\n",tick);
			printf("inefficent proc is %i and page is %i\n",z,page);
			printf("page that should have been removed is %i\n\n",y);
			printf("that pages tick is: %i\n",timestamps[z][y]);
			in=1;
			
		}
	    }
	if(in==1){
		printf("The max tick was: %i\n",mtick);
		//getchar();
	}

}


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


int lru(Pentry q[MAXPROCESSES],int nproc,int npage){
	int pagetmp;
	int min =INT_MAX;
	int minPage;

	for(pagetmp=0; pagetmp < MAXPROCPAGES; pagetmp++){

		if((npage!= pagetmp) && (q[nproc].pages[pagetmp]) && (timestamps[nproc][pagetmp]<min)){
			minPage=pagetmp;
			min=timestamps[nproc][pagetmp];

		}


	}
	return minPage;
}



void pageit(Pentry q[MAXPROCESSES]) { 

    /* This file contains the stub for an LRU pager */
    /* You may need to add/remove/modify any part of this file */
	//printf("%ld",processes[0]->pid);
    /* Static vars */
    static int initialized = 0;
    //static int tick = 1; // artificial time
    
	//printf("here\n");
    /* Local vars */
    int proctmp;
int minPageTwo;
	int secondMinPageOne;
	int secondMinPageTwo;
    int pagetmp;
	int secondMaxPage;
	int secondsecondMaxPage;
	int pc;
	int page;
	static int procount=0;
	int min;
	int pageOne;
	int predOne;
	int predTwo;
	int predThree;

		int swapRow=0;
		int swapCol=0;
		int tmppage;
		int proc;
	int pageTwo;
	int minPage;
	int i;
	int minPageOne;
    /* initialize static vars on first run */
    if(!initialized){
	for(proctmp=0; proctmp < MAXPROCESSES; proctmp++){
	    for(pagetmp=0; pagetmp < MAXPROCPAGES; pagetmp++){
				timestamps[proctmp][pagetmp] = 0;
				pageLastRemovedAt[proctmp][pagetmp]=0; 
				numberofpagesout=0;
		for(i=0; i<2;i++){
			betRevlentPage[proctmp][pagetmp][i]=0;

		}
		for(i=0;i<MAXPROCPAGES;i++){
			revelentPageFreq[proctmp][pagetmp][i]=0;
		}
	    }
		lastPageCalled[proctmp]=0;
	}
	tick=1;
	prevtick=0;
	initialized = 1;
    }
    
	
	//pager();
	for(proc=0; proc <MAXPROCESSES; proc++){
		//printf("1\n");	
		if(q[proc].active){
			
			//printf("2\n");
			pc = q[proc].pc;
			page=pc/PAGESIZE;
			/*if(procount>=4){

				pager(q);
			}*/

			timestamps[proc][page] = tick;
			
			


			tmppage=lastPageCalled[page];
			betRevlentPage[proc][tmppage][1]=betRevlentPage[proc][tmppage][0];
			betRevlentPage[proc][tmppage][0]=page;


			predOne=betRevlentPage[proc][page][0];
			predTwo=betRevlentPage[proc][predOne][0];
			predThree=betRevlentPage[proc][predTwo][0];


			revelentPageFreq[proc][tmppage][page]++;
			lastPageCalled[proc]=page;

			pageOne=arrayMax(proc,page,&secondMaxPage);
			pageTwo=arrayMax(proc,pageOne,&secondsecondMaxPage);

			minPageOne=arrayMin(q,proc,page,&secondMinPageOne);
			minPageTwo=arrayMin(q,proc,pageOne,&secondMinPageTwo);


			minPage=lru(q,proc,page);
			pagein(proc,page);
			pagein(proc,pageTwo);
			pagein(proc,pageOne);
			/*if(secondMaxPage!=-100 && secondMaxPage!=secondMinPageOne){
				pagein(proc,secondMaxPage);
				if(secondMinPageOne!=-100){
					pageout(proc, secondMinPageOne);
				}
			}
			if(secondMaxPage!=-100 && secondMaxPage!=secondMinPageTwo){
				pagein(proc,secondsecondMaxPage);*
				if(secondMinPageTwo!=-100){
					pageout(proc,secondMinPageTwo);
				}
			}*/
			//pagein(proc,predThree);
			//pagein(proc,predTwo);
			//pagein(proc,predOne);


			if(!q[proc].pages[page]){
				effChecker(proc,page);
				if(!pagein(proc,page)){
					pageout(proc,minPage);
					pageout(proc,minPageOne);
					pageout(proc,minPageTwo);
					if(secondMinPageTwo!=-100){
						pageout(proc,secondMinPageTwo);
					}
					if(secondMinPageOne!=-100){
						pageout(proc, secondMinPageOne);
					}
					

				}else if(!pagein(proc,pageTwo)){
					pageout(proc,minPage);
					pageout(proc,minPageOne);
					if(secondMinPageTwo!=-100){
						pageout(proc,secondMinPageTwo);
					}
					if(secondMinPageOne!=-100){
						pageout(proc, secondMinPageOne);
					}
					

				}else if(!pagein(proc,pageOne)){
					//pageout(proc,minPage);
					pageout(proc,minPage);
					pageout(proc,minPageOne);
					pageout(proc,minPageTwo);
					//pageout(proc,minPageOne);
					if(secondMinPageTwo!=-100){
						pageout(proc,secondMinPageTwo);
					}
					if(secondMinPageOne!=-100){
						pageout(proc, secondMinPageOne);
					}
					
				}

			}

			
				


		}else{//remove all things related to the process if not active
			for(i=0;i<MAXPROCPAGES;i++){
				pageout(proc,i);
				procount++;
			}

		}
 		
	}



    /* TODO: Implement LRU Paging */
	//pager(q);

    /* advance time for next pageit iteration */
	pager(q);
    tick++;
} 
