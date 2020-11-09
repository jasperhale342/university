
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdlib.h> 
#include <stdbool.h> 
#include <sys/time.h>
#define NClerks 4
#define NQUEUE 2
#define BUSQ 1
#define ECONQ 0
#define IDLE 0
 


typedef struct customer_info{
    	int userID;
	int classType;
	int arrivalTime;
	int serviceTime;
} customerInfo;

//When a clerk gets ahold of queueStat they will choose the first customer in a que by changing
//the value of winnerSelected
bool winnerSelected[NQUEUE];
pthread_mutex_t WSLock;
 


/*
Waiting time 
*/
pthread_mutex_t WTbusLock;
pthread_mutex_t WTeconLock;

double WTbus;
double WTecon;

/*
econLock is used for when anything is being enqueued or dequeued from the econQ variable
Same goes for busLock
*/
pthread_mutex_t econLock; //mutex declaration
pthread_mutex_t busLock;

int *busQ;
int *econQ;

//for adding customers onto the queue or off
pthread_cond_t econConvar = PTHREAD_COND_INITIALIZER; //convar initialization
pthread_cond_t busConvar = PTHREAD_COND_INITIALIZER; //convar initialization

//checking the length and status of a que 
pthread_mutex_t qStatusLock;
pthread_mutex_t qlengthLock;

int queueStat[4];
int queueLen[NQUEUE];






// For when the customer selected is ready to be served
pthread_cond_t econServeConvar = PTHREAD_COND_INITIALIZER; // for serving the customer in econ
pthread_cond_t busServeConvar = PTHREAD_COND_INITIALIZER;

pthread_mutex_t econServeLock;
pthread_mutex_t busServeLock;

//clerk waits for the customer to be done
pthread_cond_t econFinConvar = PTHREAD_COND_INITIALIZER; // for serving the customer in econ
pthread_cond_t busFinConvar = PTHREAD_COND_INITIALIZER;

pthread_mutex_t econFinLock;
pthread_mutex_t busFinLock;



//used by customer and clerk to see if the right clerk was signaled from a FinLock broadcast
bool clerkServer[4];
pthread_mutex_t clerkServerLock;


//Ensure that the customer actually gets the broadcast to be served. Clerk may send broadcast
//before customer is waiting on the broadcast signal
bool clerkRecvServe[4];
pthread_mutex_t clerkRecvServeLock;

customerInfo* makeCustomer(int userID, int classType, int arrivalTime, int serviceTime){

	customerInfo* cust = malloc(sizeof(customerInfo));
	cust->userID = userID;
	cust->classType = classType;
	cust->serviceTime = serviceTime;
	cust->arrivalTime = arrivalTime;
	return cust;
		

}

void * customer_entry(void * cus_info);
void * clerk_entry(void * clerkNUM);

//getting times
double getCurrentSimulationTime(struct timeval start_time){
	
	struct timeval cur_time;
	double cur_secs, init_secs;
	
	
	init_secs = (start_time.tv_sec + (double) start_time.tv_usec / 1000000);
	
	
	gettimeofday(&cur_time, NULL);
	cur_secs = (cur_time.tv_sec + (double) cur_time.tv_usec / 1000000);
	
	return cur_secs - init_secs;
}

void dequeue(int que){
	int i = 0;
	if(queueLen[que] == 0){
		return;
	}
	if(que == BUSQ){		
		
		for(i = 0; i < queueLen[que]-1; i ++){
			busQ[i] = busQ[i+1];
		}
		busQ[queueLen[que]-1] = 0;		
		queueLen[que]--;
	}else{
	
		
		for(i = 0; i < queueLen[que]-1; i ++){
			econQ[i] = econQ[i+1];
		}
		econQ[queueLen[que]-1] = 0;		
		queueLen[que]--;
		
	}
	return;

}
void enqueue(int que, int id){
	if(que == BUSQ){
		busQ[queueLen[que]] = id;
		queueLen[que]++;
		return;
		
	}
	econQ[queueLen[que]] = id;
	queueLen[que]++;
	return;
}


int main(int argc, char *argv[]) {

	// initialize all the condition variable and thread lock will be used
	//pthread_mutex_t * muts = {&WTbusLock, &econLock};
   	if (pthread_mutex_init(&WTbusLock, NULL) != 0){ //mutex initialization
        	printf("\n mutex init failed for watingTimeLock\n");
        	return 1;
    	
	}
   	if (pthread_mutex_init(&econLock, NULL) != 0){ //mutex initialization
        	printf("\n mutex init failed for watingTimeLock\n");
        	return 1;
    	
	}
   	if (pthread_mutex_init(&busLock, NULL) != 0){ //mutex initialization
        	printf("\n mutex init failed for watingTimeLock\n");
        	return 1;
    	
	}
   	if (pthread_mutex_init(&qStatusLock, NULL) != 0){ //mutex initialization
        	printf("\n mutex init failed for watingTimeLock\n");
        	return 1;
    	
	}
   	if (pthread_mutex_init(&qlengthLock, NULL) != 0){ //mutex initialization
        	printf("\n mutex init failed for watingTimeLock\n");
        	return 1;
    	
	}
	if (pthread_mutex_init(&WTeconLock, NULL) != 0){ //mutex initialization
        	printf("\n mutex init failed for watingTimeLock\n");
        	return 1;
    	
	}


	if (pthread_mutex_init(&WSLock, NULL) != 0){ //mutex initialization
        	printf("\n mutex init failed for watingTimeLock\n");
        	return 1;
    	
	}
	if (pthread_mutex_init(&econServeLock, NULL) != 0){ //mutex initialization
        	printf("\n mutex init failed for watingTimeLock\n");
        	return 1;
    	
	}
	if (pthread_mutex_init(&busServeLock, NULL) != 0){ //mutex initialization
        	printf("\n mutex init failed for watingTimeLock\n");
        	return 1;
    	
	}
	if (pthread_mutex_init(&busFinLock, NULL) != 0){ //mutex initialization
        	printf("\n mutex init failed for watingTimeLock\n");
        	return 1;
    	
	}
	if (pthread_mutex_init(&econFinLock, NULL) != 0){ //mutex initialization
        	printf("\n mutex init failed for watingTimeLock\n");
        	return 1;
    	
	}
	if (pthread_mutex_init(&clerkServerLock, NULL) != 0){ //mutex initialization
        	printf("\n mutex init failed for watingTimeLock\n");
        	return 1;
    	
	}

	if (pthread_mutex_init(&clerkRecvServeLock, NULL) != 0){ //mutex initialization
        	printf("\n mutex init failed for watingTimeLock\n");
        	return 1;
    	
	}
	
	
	if(argc != 2){
		printf("Two many arguments\n");
		exit(1);
	
	}
	customerInfo* c;
	FILE *fp = fopen(argv[1], "r");
	char buff[1000];

	char *token;
	int customer[4];
	int j = 0;
	double maxBus = 0;
	double maxEcon = 0;
	int maxCustomer = atoi(fgets(buff, 1000, fp));
	econQ = malloc(sizeof(int)*maxCustomer);	
	busQ = malloc(sizeof(int)*maxCustomer);	
	customerInfo* cust[maxCustomer];
	int i = 0;
	for( i = 0; i <maxCustomer; i++){
		fgets(buff, 1000, fp);
		token = strtok(buff, ":,\n");
		while(token!= NULL){
			customer[j] = atoi(token);
			j++;
			if(j==4){
				break;
			}	
			token = strtok(NULL, ":,\n");
		}
		
		j=0;
		if(customer[1] == BUSQ){
			maxBus = maxBus +1;
		} else{
			maxEcon = maxEcon +1;	
		}
		cust[i] = makeCustomer(customer[0], customer[1], customer[2], customer[3]);
		
	}
	
	

	
	pthread_t customId[maxCustomer]; // customer thread array
	pthread_t clerkId[4]; // clerk thread array
	int clerkInfo [4];
	fclose(fp);
	for(i = 0; i< 4; i++){
		clerkInfo[i] = i+1;
		clerkServer[i] = false;
		clerkRecvServe[i] = false;
		
	}


	for(i = 0; i < 4; i++){ // number of clerks
		pthread_create(&clerkId[i], NULL, clerk_entry, (void *)&clerkInfo[i]);
	}
	printf("clerk threads created\n");
	
	//create customer thread
	for(i = 0; i < maxCustomer; i++){ // number of customers
		pthread_create(&customId[i], NULL, customer_entry, (void *)cust[i]); 
	}

	printf("customer threads created\n");
	// wait for all customer threads to terminate
	for (i =0; i< maxCustomer; i++){
		pthread_join(customId[i], NULL);
		
	}

	// destroy mutex & condition variable (optional)
	
pthread_mutex_destroy(&WTbusLock);
pthread_mutex_destroy(&econLock);
pthread_mutex_destroy(&busLock);
pthread_mutex_destroy(&qStatusLock);
pthread_mutex_destroy(&qlengthLock);
pthread_mutex_destroy(&WTeconLock);
pthread_mutex_destroy(&WSLock);
pthread_mutex_destroy(&econServeLock);
pthread_mutex_destroy(&busServeLock);
pthread_mutex_destroy(&econFinLock);
pthread_mutex_destroy(&busFinLock);
pthread_mutex_destroy(&clerkServerLock);
pthread_mutex_destroy(&clerkRecvServeLock);
	

	printf("The average waiting time for all customers in the system is: %.2f seconds. \n", (WTbus + WTecon)/maxCustomer);
	if(maxBus !=0){
		printf("The average waiting time for all business-class customers is: %.2f seconds. \n", WTbus/maxBus);
	}
	if(maxEcon !=0 ){
		printf("The average waiting time for all economy-class customers is: %.2f seconds. \n", WTecon/maxEcon);
	}
	

	return 0;
}

// function entry for customer threads

void * customer_entry(void * cus_info){
	struct timeval start_time;
	int clerk;
	customerInfo * myInfo = (customerInfo*)cus_info;
	usleep((myInfo->arrivalTime)*100000);
	
	printf("A customer arrives: customer ID %d. \n", myInfo->userID);
	
	
	
	//business class
	int que = myInfo->classType;
	int headOfQue = -1;
	//printf("choosing que to use\n");
	if(que == BUSQ){
		pthread_mutex_lock(&busLock);
		
	}else{
		
		pthread_mutex_lock(&econLock);	
			
	} 
	
	
	pthread_mutex_lock(&qlengthLock);
	
	enqueue(que, myInfo->userID);	
	printf ("A customer enters a queue: the queue ID %1d, length of the queue %2d, and customer id %d. \n", que, queueLen[que], myInfo->userID);
	gettimeofday(&start_time, NULL);	
	//double enterTime = getCurrentSimulationTime();	
	pthread_mutex_unlock(&qlengthLock);
	int headofQue = -1;
	int k = 0;
	if(que == BUSQ){
		pthread_mutex_unlock(&busLock);
		
	}else{
		pthread_mutex_unlock(&econLock);	
	} 
	
		
	while (true) {
		if(que == BUSQ){
			
				pthread_mutex_lock(&busLock);
				headOfQue = busQ[0];
				pthread_mutex_unlock(&busLock);
				printf("Business que: customer_%d is waiting. Customer_%d is at the front\n", myInfo->userID, headOfQue);
				pthread_mutex_lock(&busServeLock);
				
				pthread_cond_wait(&busServeConvar, &busServeLock);
				
				pthread_mutex_unlock(&busServeLock);

				
				clerk = queueStat[que];
				
				pthread_mutex_lock(&clerkRecvServeLock);
				clerkRecvServe[clerk-1] = true;
				pthread_mutex_unlock(&clerkRecvServeLock);
				
				pthread_mutex_lock(&WSLock);
				 
				if ( myInfo->userID == headOfQue && !winnerSelected[que]) {
					printf("Business que: customer_%d got selected\n", myInfo->userID);
					pthread_mutex_lock(&busLock);
					pthread_mutex_lock(&qlengthLock);
					dequeue(que);
					printf("Every customer moves up one spot\n");
					pthread_mutex_unlock(&qlengthLock);
					pthread_mutex_unlock(&busLock);
					winnerSelected[que] = true; 
					pthread_mutex_unlock(&WSLock);
					break;
				}
				pthread_mutex_unlock(&WSLock);
			
		}else{
			
				pthread_mutex_lock(&econLock);
				headOfQue = econQ[0];
				pthread_mutex_unlock(&econLock);
				printf("Economy que: customer_%d is waiting. Customer_%d is at the front\n", myInfo->userID, headOfQue);
				pthread_mutex_lock(&econServeLock);
				
				pthread_cond_wait(&econServeConvar, &econServeLock);
				
				pthread_mutex_unlock(&econServeLock);

				
				clerk = queueStat[que];
				
				pthread_mutex_lock(&clerkRecvServeLock);
				clerkRecvServe[clerk-1] = true;
				pthread_mutex_unlock(&clerkRecvServeLock);
				
				pthread_mutex_lock(&WSLock);
		 
				if ( myInfo->userID == headOfQue && !winnerSelected[que]) {
					printf("Economy que: customer_%d got selected\n", myInfo->userID);
					pthread_mutex_lock(&econLock);
					pthread_mutex_lock(&qlengthLock);
					dequeue(que);
					printf("Every customer moves up one spot\n");
					pthread_mutex_unlock(&qlengthLock);
					pthread_mutex_unlock(&econLock);
					winnerSelected[que] = true; 
					pthread_mutex_unlock(&WSLock);
					break;
				}
				pthread_mutex_unlock(&WSLock);
				
			
					
				}
		
	}
			
	
	
	//at this point the customer got selected from a clerk
	usleep(10); 
	if(que == BUSQ){
		double localwt = getCurrentSimulationTime(start_time);
		pthread_mutex_lock(&WTbusLock);
		
		WTbus += localwt;
		pthread_mutex_unlock(&WTbusLock);
		

	}else{
		double localwt = getCurrentSimulationTime(start_time);
		pthread_mutex_lock(&WTeconLock);
		
		WTecon += localwt;
		pthread_mutex_unlock(&WTeconLock);
		
	} 	
	struct timeval startServing;
	gettimeofday(&startServing, NULL);
	double start = (start_time.tv_sec + (double) start_time.tv_usec / 1000000);
	printf("A clerk starts serving a customer: start time %.2lf, the customer ID %2d, the clerk ID %1d. \n", start, myInfo->userID, clerk);
	
	usleep((myInfo->serviceTime)*100000);
	
	double endTime = getCurrentSimulationTime(startServing);
	printf("A clerk finishes serving a customer: end time %.2f, the customer ID %2d, the clerk ID %1d. \n", endTime, myInfo->userID, clerk);
	if(que == BUSQ){
		pthread_mutex_lock(&busFinLock);
		printf("customer_%d is leaving clerk_%d\n", myInfo->userID, clerk);
		pthread_mutex_lock(&clerkServerLock);
		clerkServer[clerk-1] = true;
		pthread_mutex_unlock(&clerkServerLock);

		pthread_cond_broadcast(&busFinConvar);
		pthread_mutex_unlock(&busFinLock);
		
				
	}else{
		pthread_mutex_lock(&econFinLock);
		printf("customer_%d is is leaving clerk_%d\n", myInfo->userID, clerk);
		pthread_mutex_lock(&clerkServerLock);
		clerkServer[clerk-1] = true;	
		pthread_mutex_unlock(&clerkServerLock);

		pthread_cond_broadcast(&econFinConvar);
		pthread_mutex_unlock(&econFinLock);
		
				
	}

	
	printf("Customer_%d has left\n", myInfo->userID);
	
	pthread_exit(NULL);
	
	
	return NULL;
}

// function entry for clerk threads
//

void *clerk_entry(void * clerkNum){
	
	while(true){
		// clerk is idle now
		int que = -1;
		int *clerkID = (int*)clerkNum;
		
		int k, p = 0;
		while(true){
			if(p == 0){

				printf("clerk %d is waiting to serve\n", clerkID[0]);
				p++;
			}
			
			
			if(queueLen[0] !=0 || queueLen[1] != 0){
				
				break;			
			}
			
			usleep(10);
			
		}
		if(queueLen[BUSQ] > 0){
			usleep(10);
			que = BUSQ;
			
			printf("Business que: clerk_%d sees a customer\n", clerkID[0]);
			pthread_mutex_lock(&qStatusLock);
			
			// if the que becomes zero			
			if(queueLen[1] ==0){
				pthread_mutex_lock(&WSLock);
				printf("clerk_%d did not get the customer, going back to wait\n", clerkID[0]);
				winnerSelected[que] = true;
				pthread_mutex_unlock(&WSLock);
				pthread_mutex_unlock(&qStatusLock);
				continue;
			} 
			
			queueStat[que] = clerkID[0];
			
			pthread_mutex_lock(&WSLock);
			
			winnerSelected[que] = false;
			pthread_mutex_unlock(&WSLock);
			
			
			usleep(10);
			
			printf("Business que: clerk_%d is going to serve the next customer\n", clerkID[0]);
			
		
			int y = 0;
			
			//at this point the clerk knows there is still someone on the que and
			//they want to serve them
			while(true){
				if(y < 1){
					//printf("Business que: clerk_%d is sending signal. the ack is %d\n", clerkID[0], clerkRecvServe[clerkID[0]-1]);
					y++;

				}
				pthread_mutex_lock(&busServeLock);
				pthread_cond_broadcast(&busServeConvar);
				//usleep(10);
				if(clerkRecvServe[clerkID[0]-1] || queueLen[1] == 0){
					printf("Business que: clerk_%d is going to serve customer_%d\n", clerkID[0], busQ[0]);
					pthread_mutex_unlock(&busServeLock);
					break;
				}
				pthread_mutex_unlock(&busServeLock);
				
			}
			
			
			pthread_mutex_unlock(&qStatusLock);
					
			
			//now in the state of waiting for the customer to be 				done			
			while(true){
				//k = 0;
				
				//if(k < 2) printf("BUS: clerk_%d waiting for customer reponse\n", clerkID[0]);
				//k++;	
				pthread_mutex_lock(&busFinLock);
							
				pthread_cond_wait(&busFinConvar, &busFinLock);
				
				
				if(clerkServer[clerkID[0]-1]){
					que = -1;
					pthread_mutex_lock(&clerkServerLock);
					clerkServer[clerkID[0]-1] = false;
					
					pthread_mutex_unlock(&clerkServerLock);
					pthread_mutex_unlock(&busFinLock);
					
					// I am done serving that customer
					k=0;
					break;
				}
				
				pthread_mutex_unlock(&busFinLock);
				
			}
			pthread_mutex_lock(&clerkRecvServeLock);
			clerkRecvServe[clerkID[0]-1] = false;
			pthread_mutex_unlock(&clerkRecvServeLock);
			if (k==0)printf("BUS: clerk_%d is going back to start\n", clerkID[0]);
			
		}else{
			usleep(10);
			que = ECONQ;
			
			printf("Economy que: clerk_%d sees a customer\n", clerkID[0]);
			pthread_mutex_lock(&qStatusLock);
			
			if(queueLen[0] ==0){
				pthread_mutex_lock(&WSLock);
				
				printf("clerk_%d did not get the customer, going back to wait\n", clerkID[0]);				
				winnerSelected[que] = true;
				pthread_mutex_unlock(&WSLock);
				pthread_mutex_unlock(&qStatusLock);
				continue;
			} 
			
			queueStat[que] = clerkID[0];
			
			pthread_mutex_lock(&WSLock);
			
			winnerSelected[que] = false;
			pthread_mutex_unlock(&WSLock);
			
			
			usleep(10);
			
			printf("Economy que: clerk_%d is going to serve the next customer\n", clerkID[0]);
			
			// I am ready to serve a customer
			int y = 0;
			//printf("")
			while(true){
				if(y < 1){
					//printf("ECON: clerk_%d is sending signal. the ack is %d\n", clerkID[0], clerkRecvServe[clerkID[0]-1]);
					y++;

				}
				pthread_mutex_lock(&econServeLock);
				pthread_cond_broadcast(&econServeConvar);
				//usleep(10);
				if(clerkRecvServe[clerkID[0]-1] || queueLen[0] == 0){
					
					printf("Economy que: clerk_%d is going to serve customer_%d\n", clerkID[0], econQ[0]);
					pthread_mutex_unlock(&econServeLock);
					break;
				}
				pthread_mutex_unlock(&econServeLock);
				
			}
			
			pthread_mutex_unlock(&qStatusLock);
					
			
						
			while(true){
				k = 0;
				
				//if(k < 2) printf("ECON: clerk_%d waiting for customer reponse\n", clerkID[0]);
				//k++;	
				pthread_mutex_lock(&econFinLock);
							
				pthread_cond_wait(&econFinConvar, &econFinLock);
				
				
				if(clerkServer[clerkID[0]-1]){
					que = -1;
					pthread_mutex_lock(&clerkServerLock);
					clerkServer[clerkID[0]-1] = false;
					
					pthread_mutex_unlock(&clerkServerLock);
					pthread_mutex_unlock(&econFinLock);
					//pthread_mutex_unlock(&fromCustLock);
					// I am done serving that customer
					k=0;
					break;
				}
				
				pthread_mutex_unlock(&econFinLock);
				
			}
			pthread_mutex_lock(&clerkRecvServeLock);
			clerkRecvServe[clerkID[0]-1] = false;
			pthread_mutex_unlock(&clerkRecvServeLock);
			if (k==0)printf("clerk_%d is going back to start\n", clerkID[0]);
			
		}	
		
	}
	
	pthread_exit(NULL);
	
	return NULL;

}
