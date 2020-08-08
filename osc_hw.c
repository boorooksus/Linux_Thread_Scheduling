#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>

sem_t mutexq; //semaphore for queues
sem_t mutexp; //semaphore for threads

void *runner(void *arg); //thread that print process id
void *q1Shed(void *args); //scheduler for 1st queue
void *q2Shed(void *args); //scheduler for 2nd queue
void *q3Shed(void *args); //scheduler for 3rd queue
void *qSched(void *args); //scheduler for queues


int main(){
	//array that saves informations of processes
	int arr[101][4] = {0, };
	
	printf("Input data: \n");
	
	//get informatinos of processes
	int a, b, c, d;
	int i = 0;
	while(scanf("%d %d %d %d", &a, &b, &c, &d)){
		if(a == 0 && b == 0 && c == 0 && d == 0)
			break;
		arr[i][0] = a;
		arr[i][1] = b;
		arr[i][2] = c;
		arr[i][3] = d;
		i++;
	}
	
	printf("Output: \n");
	//thread identifier for queue scheduling
	pthread_t pqs; 
	//cteate thread for queue scheduling
	pthread_create(&pqs, NULL, qSched, arr);
	//wait for the thread to exit
	pthread_join(pqs, NULL);
	
	return 0;
}

//thread that prints process id
//arg : process id
void *runner(void *arg){
	//wait
	sem_wait(&mutexp);

	//get process id
	int id = *(int *)arg;

	//print process id once
	printf("%d ", id);
	//halt for 1ms
	usleep(1000);

	//signal
	sem_post(&mutexp);
}

//SJF Scheduler(scheduler for 1st queue)
//args : informations of processes in 1st queue
void *q1Shed(void *args){
	//wait
	sem_wait(&mutexq);
	
	//get informations of processes in 1st queue
	int (*q1)[101][4] = args;

	//number of processes in 1st queue
	int cnt = 0; 
	//array that saves ids and runtimes of each process
	int id_rt[101][2]; 
	
	//save ids and runtimes of each process
	//	and get number of processes in 1st queue
	for(int i = 0; (*q1)[i][0] != 0; i++){
		id_rt[i][0] = (*q1)[i][1];
		id_rt[i][1] = (*q1)[i][3];
		cnt++;
	}
	
	//bubble sort
	//sort id_rt array by runtime
	for (int i = 0; i < cnt; i++){
		for (int j = 0; j < cnt - (i + 1); j++){
			if(id_rt[j][1] > id_rt[j + 1][1]){
				int temp[2] = {id_rt[j + 1][0], id_rt[j + 1][1]};
				id_rt[j + 1][0] = id_rt[j][0];
				id_rt[j + 1][1] = id_rt[j][1];
				id_rt[j][0] = temp[0];
				id_rt[j][1] = temp[1];
			}
		}
	}
	
	//semaphore for threads
	sem_init(&mutexp, 0, 1);
	
	//create threads that print id for each processes
	for(int i = 0; i < cnt; i++){
		for (int j = 0; j < id_rt[i][1]; j++){
			pthread_t t1;

			int pid = id_rt[i][0];
			int *p = &pid;

			pthread_create(&t1, NULL, runner, p);
			pthread_join(t1, NULL);
		}
		printf("\n");
	}
	
	
	sem_destroy(&mutexp);

	//signal
	sem_post(&mutexq);
}

//Priority Scheduler(scheduler for 2nd queue)
//args : informations of processes in 2nd queue
void *q2Shed(void *args){
	//wait
	sem_wait(&mutexq);

	//get informations of processes in 2nd queue
	int (*q2)[101][4] = args;

	//maximum priority of processes in queue
	int priorMax = 0;

	//get maximum priority of processes in queue
	for(int i = 0; (*q2)[i][0] != 0; i++){
		if (priorMax < (*q2)[i][2]){
			priorMax = (*q2)[i][2];
		}
	}
	
	//semaphore for runner threads
	sem_init(&mutexp, 0, 1);

	//create threads that print id for each processes
	for(int i = 1; i <= priorMax; i++){
		for(int j = 0; (*q2)[j][0] != 0; j++){
			if ((*q2)[j][2] == i){
				for(int k = 0; k < (*q2)[j][3]; k++){
					pthread_t t2;
				

					int pid = (*q2)[j][1];
					int *p = &pid;
	
					pthread_create(&t2, NULL, runner, p);
					pthread_join(t2, NULL);
				
				}
				printf("\n");
			}
		}
	}

	sem_destroy(&mutexp);
	
	//signal
	sem_post(&mutexq);
}

//Round-Robin Scheduler(scheduler for 3rd queue)
//args : informations of processes in 3rd queue
void *q3Shed(void *args){
	//wait
	sem_wait(&mutexq);
	
	//get informations of processes in 3rd queue
	int (*q3)[101][4] = args;
	
	//array that saves progress of each processes
	int progress[101] = {0, };
	//number of processes in 3rd queue
	int pCnt = 0;
	//number of processes finished
	int finished = 0;

	//get number of processes in 3rd queue
	for(int i = 0; (*q3)[i][0] != 0; i++){
		pCnt++;
	}
	
	//semaphore for runner threads
	sem_init(&mutexp, 0, 1);

	//create threads that print id for each processes
	while(finished < pCnt){
		for(int i = 0; i < pCnt; i++){
			if ((*q3)[i][3] > progress[i]){
				pthread_t t3;

				int pid = (*q3)[i][1];
				int *p = &pid;

				pthread_create(&t3, NULL, runner, p);
				pthread_join(t3, NULL);

				progress[i]++;
				if(progress[i] == (*q3)[i][3]){
					finished++;
				}
				printf("\n");
			}
		}
	}
	
	sem_destroy(&mutexp);

	//signal
	sem_post(&mutexq);
}

//scheduler for queues
//args : informations of processes
void *qSched(void *args){
	//get informations of processes
	int (*arr)[101][4] = args;

	int q1[101][4] = {0, }; //1st queue
	int q2[101][4] = {0. }; //2nd queue
	int q3[101][4] = {0, }; //3rd queue
	
	int qCnt[4] = {0, 0, 0, 0}; //count each queues

	//classify processes according to queue class number
	int i = 0;
	while(1){
		//get class no., process id, priority, runtime in line
		int temp[4];
		for(int j = 0; j < 4; j++){
			temp[j] = (*arr)[i][j];
		}
		//break when temp gets 0
		if(temp[0] == 0 && temp[1] == 0 && temp[2] == 0 && temp[3] == 0)
			break;
		
		//categorized as 1st queue
		if(temp[0] == 1){
			int idx = qCnt[1]++;
			for(int j = 0; j < 4; j++){
				q1[idx][j] = temp[j];
			}
		}
		//categorized as 2nd queue
		else if(temp[0] == 2){
			int idx = qCnt[2]++;
			for(int j = 0; j < 4; j++){
				q2[idx][j] = temp[j];
			}
		}
		//categorized as 3rd queue
		else if(temp[0] == 3){
			int idx = qCnt[3]++;
			for(int j = 0; j < 4; j++){
				q3[idx][j] = temp[j];

			}
		}
		
		i++;
	}
		

	//semaphore for queues
	sem_init(&mutexq, 0, 1);
	
	//thread identifier for scheduler of 1st, 2nd, 3rd queue
	pthread_t ps1, ps2, ps3;
	
	//create thread for scheduler of 1st queue
	pthread_create(&ps1, NULL, q1Shed, q1);
	//wait for the thread to exit
	pthread_join(ps1, NULL);
	
	//create thread for scheduler of 2nd queue
	pthread_create(&ps2, NULL, q2Shed, q2);
	//wait for the thread to exit
	pthread_join(ps2, NULL);
	
	//create thread for scheduler of 3rd queue
	pthread_create(&ps3, NULL, q3Shed, q3);
	//wait for the thread to exit
	pthread_join(ps3, NULL);

	sem_destroy(&mutexq);

}


