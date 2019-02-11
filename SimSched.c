/**
Proiect realizat de:
Butu Anda Irina
Cercel Ana Maria
Ciobanu Andrei Ovidiu
Grupa 234
**/

/**
Codul reprezinta un simulator pentru urmatorii algoritmi:
FCFS (First Come First Served)
SJF (Shortest Job First)
Priority
Round Robin
LIF (Longest I/O First)
LISC (Longest I/O & Shortest CPU First)

Pentru a rula programul trebuie sa se introduca numarul total de procese
si numarul de procese care apeleaza I/O request

Exemplu:
./simsched 10 3

Este disponibila si afisarea rezultatului intr-un fisier.
**/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX_TIME_UNIT 1000
#define MAX_PROCESS_NUM 30
#define MAX_ALGORITHM_NUM 10

#define FCFS 0
#define SJF 1
#define PRIORITY 2
#define RR 3
#define LIF 4
#define LISC 5

#define TRUE 1
#define FALSE 0

#define TIME_QUANTUM 3

//process
typedef struct myProcess* processPointer;
typedef struct myProcess {
    int pid;
    int priority;
    int arrivalTime;
    int CPUburst;
    int IOburst;
    int CPUremainingTime;
    int IOremainingTime;
    int waitingTime;
    int turnaroundTime;
    int responseTime;

}myProcess;

int Computation_start = 0;
int Computation_end = 0;
int Computation_idle = 0;

typedef struct evaluation* evalPointer;
typedef struct evaluation {
	int alg;
	int preemptive;
	int startTime;
	int endTime;
	int avg_waitingTime;
	int avg_turnaroundTime;
	int avg_responseTime;
	double CPU_util;
	int completed;
}evaluation;

evalPointer evals[MAX_ALGORITHM_NUM];
int cur_eval_num = 0;

void init_evals(){
	cur_eval_num = 0;
	int i;
	for(i=0;i<MAX_ALGORITHM_NUM;i++)
		evals[i]=NULL;
}

void clear_evals() {

	int i;
	for(i=0;i<MAX_ALGORITHM_NUM;i++){
		free(evals[i]);
		evals[i]=NULL;
	}
	cur_eval_num = 0;
}

//Job Queue
processPointer jobQueue[MAX_PROCESS_NUM];
int cur_proc_num_JQ = 0;

void init_JQ () {
	cur_proc_num_JQ = 0;
    int i;
    for (i = 0; i < MAX_PROCESS_NUM; i++)
        jobQueue[i] = NULL;
}

void sort_JQ() {
    int i, j;
    processPointer remember;
    for ( i = 1; i < cur_proc_num_JQ; i++ )
    {
      remember = jobQueue[(j=i)];
      while ( --j >= 0 && remember->pid < jobQueue[j]->pid )
          jobQueue[j+1] = jobQueue[j];
      jobQueue[j+1] = remember;
    }
}

int getProcByPid_JQ (int givenPid) {
    int result = -1;
    int i;
    for(i = 0; i < cur_proc_num_JQ; i++) {
        int temp = jobQueue[i]->pid;
        if(temp == givenPid)
            return i;
    }
    return result;
}

void insertInto_JQ (processPointer proc) {
    if(cur_proc_num_JQ<MAX_PROCESS_NUM) {
        int temp = getProcByPid_JQ(proc->pid);
        if (temp != -1) {
            printf("<ERROR> The process with pid: %d already exists in Job Queue\n", proc->pid);
            return;
        }
        jobQueue[cur_proc_num_JQ++] = proc;
    }
    else {
        puts("<ERROR> Job Queue is full");
        return;
    }
}

processPointer removeFrom_JQ (processPointer proc) {
    if(cur_proc_num_JQ>0) {
        int temp = getProcByPid_JQ(proc->pid);
        if (temp == -1) {
            printf("<ERROR> Cannot find the process with pid: %d\n", proc->pid);
            return NULL;
        } else {
            processPointer removed = jobQueue[temp];

            int i;
            for(i = temp; i < cur_proc_num_JQ - 1; i++)
                jobQueue[i] = jobQueue[i+1];
            jobQueue[cur_proc_num_JQ - 1] = NULL;

            cur_proc_num_JQ--;
            return removed;
        }

    } else {
        puts("<ERROR> Job Queue is empty");
        return NULL;
    }
}

void clear_JQ() {
    int i;
    for(i = 0; i < MAX_PROCESS_NUM; i++) {
        free(jobQueue[i]);
        jobQueue[i] = NULL;
    }
    cur_proc_num_JQ = 0;
}

void print_JQ() {
	printf("The total number of processes: %d\n", cur_proc_num_JQ);
	int i;
	puts("pid    priority    arrival_time    CPU burst    IO burst");
	puts("========================================================");
    for(i = 0; i < cur_proc_num_JQ; i++) {
        printf("%3d    %8d    %12d    %9d    %8d\n", jobQueue[i]->pid, jobQueue[i]->priority, jobQueue[i]->arrivalTime, jobQueue[i]->CPUburst, jobQueue[i]->IOburst);
    }
    puts("========================================================\n");
}

processPointer cloneJobQueue[MAX_PROCESS_NUM];
int cur_proc_num_clone_JQ = 0;

void clone_JQ() {

	int i;
	for (i=0; i< MAX_PROCESS_NUM; i++) { //init clone
		cloneJobQueue[i] = NULL;
	}

	for (i=0; i<cur_proc_num_JQ; i++) {

		processPointer newProcess = (processPointer)malloc(sizeof(struct myProcess));
        newProcess->pid = jobQueue[i]->pid;
        newProcess->priority = jobQueue[i]->priority;
        newProcess->arrivalTime = jobQueue[i]->arrivalTime;
        newProcess->CPUburst = jobQueue[i]->CPUburst;
        newProcess->IOburst = jobQueue[i]->IOburst;
        newProcess->CPUremainingTime = jobQueue[i]->CPUremainingTime;
        newProcess->IOremainingTime = jobQueue[i]->IOremainingTime;
        newProcess->waitingTime = jobQueue[i]->waitingTime;
        newProcess->turnaroundTime = jobQueue[i]->turnaroundTime;
        newProcess->responseTime = jobQueue[i]->responseTime;

        cloneJobQueue[i] = newProcess;
	}

	cur_proc_num_clone_JQ = cur_proc_num_JQ;
}

void loadClone_JQ() {
	clear_JQ(); //clear JQ
	int i;
	for (i=0; i<cur_proc_num_clone_JQ; i++) {

		processPointer newProcess = (processPointer)malloc(sizeof(struct myProcess));
	    newProcess->pid = cloneJobQueue[i]->pid;
	    newProcess->priority = cloneJobQueue[i]->priority;
	    newProcess->arrivalTime = cloneJobQueue[i]->arrivalTime;
	    newProcess->CPUburst = cloneJobQueue[i]->CPUburst;
	    newProcess->IOburst = cloneJobQueue[i]->IOburst;
	    newProcess->CPUremainingTime = cloneJobQueue[i]->CPUremainingTime;
	    newProcess->IOremainingTime = cloneJobQueue[i]->IOremainingTime;
	    newProcess->waitingTime = cloneJobQueue[i]->waitingTime;
	    newProcess->turnaroundTime = cloneJobQueue[i]->turnaroundTime;
	    newProcess->responseTime = cloneJobQueue[i]->responseTime;

	    jobQueue[i] = newProcess;
	}

	cur_proc_num_JQ = cur_proc_num_clone_JQ;
	//print_JQ();
}

void clearClone_JQ() {
    int i;
    for(i = 0; i < MAX_PROCESS_NUM; i++) {
        free(cloneJobQueue[i]);
        cloneJobQueue[i] = NULL;
    }
}

processPointer runningProcess = NULL;
int timeConsumed = 0;


processPointer readyQueue[MAX_PROCESS_NUM];
int cur_proc_num_RQ = 0;

void init_RQ () {
    cur_proc_num_RQ = 0;
	int i;
    for (i = 0; i < MAX_PROCESS_NUM; i++)
        readyQueue[i] = NULL;
}

int getProcByPid_RQ (int givenPid) {
    int result = -1;
    int i;
    for(i = 0; i < cur_proc_num_RQ; i++) {
        int temp = readyQueue[i]->pid;
        if(temp == givenPid)
            return i;
    }
    return result;
}

void insertInto_RQ (processPointer proc) {
    if(cur_proc_num_RQ<MAX_PROCESS_NUM) {
        int temp = getProcByPid_RQ(proc->pid);
        if (temp != -1) {
            printf("<ERROR> The process with pid: %d already exists in Ready Queue\n", proc->pid);
            return;
        }
        readyQueue[cur_proc_num_RQ++] = proc;
    }
    else {
        puts("<ERROR> Ready Queue is full");
        return;
    }
}

processPointer removeFrom_RQ (processPointer proc) {
    if(cur_proc_num_RQ>0) {
        int temp = getProcByPid_RQ(proc->pid);
        if (temp == -1) {
            printf("<ERROR> Cannot find the process with pid: %d\n", proc->pid);
            return NULL;
        } else {
            processPointer removed = readyQueue[temp];

            int i;
            for(i = temp; i < cur_proc_num_RQ - 1; i++)
                readyQueue[i] = readyQueue[i+1];
            readyQueue[cur_proc_num_RQ - 1] = NULL;

            cur_proc_num_RQ--;
            return removed;
        }

    } else {
        puts("<ERROR> Ready Queue is empty");
        return NULL;
    }
}

void clear_RQ() {
    int i;
    for(i = 0; i < MAX_PROCESS_NUM; i++) {
        free(readyQueue[i]);
        readyQueue[i]=NULL;
    }
    cur_proc_num_RQ = 0;
}

void print_RQ() {
    puts("\nprintf_RQ()");
	int i;
    for(i = 0; i < cur_proc_num_RQ; i++) {
        printf("%d ", readyQueue[i]->pid);
    }
    printf("\nTotal number of processes: %d\n", cur_proc_num_RQ);
}

//waitingQueue
processPointer waitingQueue[MAX_PROCESS_NUM];
int cur_proc_num_WQ = 0;

void init_WQ () {
	cur_proc_num_WQ = 0;
    int i;
    for (i = 0; i < MAX_PROCESS_NUM; i++)
        waitingQueue[i] = NULL;
}

int getProcByPid_WQ (int givenPid) {
    int result = -1;
    int i;
    for(i = 0; i < cur_proc_num_WQ; i++) {
        int temp = waitingQueue[i]->pid;
        if(temp == givenPid)
            return i;
    }
    return result;
}

void insertInto_WQ (processPointer proc) {
    if(cur_proc_num_WQ<MAX_PROCESS_NUM) {
        int temp = getProcByPid_WQ(proc->pid);
        if (temp != -1) {
            printf("<ERROR> The process with pid: %d already exists in Waiting Queue\n", proc->pid);
            return;
        }
        waitingQueue[cur_proc_num_WQ++] = proc;
    }
    else {
        puts("<ERROR> Waiting Queue is full");
        return;
    }
    //print_WQ();
}

processPointer removeFrom_WQ (processPointer proc) {
    if(cur_proc_num_WQ>0) {
        int temp = getProcByPid_WQ(proc->pid);
        if (temp == -1) {
            printf("<ERROR> Cannot find the process with pid: %d\n", proc->pid);
            return NULL;
        } else {

            processPointer removed = waitingQueue[temp];
            int i;
            for(i = temp; i < cur_proc_num_WQ - 1; i++)
                waitingQueue[i] = waitingQueue[i+1];

            waitingQueue[cur_proc_num_WQ - 1] = NULL;

            cur_proc_num_WQ--;

            return removed;
        }

    } else {
        puts("<ERROR> Waiting Queue is empty");
        return NULL;
    }
}

void clear_WQ() {
    int i;
    for(i = 0; i < MAX_PROCESS_NUM; i++) {
        free(waitingQueue[i]);
        waitingQueue[i] = NULL;
    }
    cur_proc_num_WQ = 0;
}

void print_WQ() {
    puts("\nprintf_WQ()");
	int i;

    for(i = 0; i < cur_proc_num_WQ; i++) {
        printf("%d ", waitingQueue[i]->pid);
    }
    printf("\nThe total number of processes: %d\n", cur_proc_num_WQ);
}

//terminatedQueue
processPointer terminated[MAX_PROCESS_NUM];
int cur_proc_num_T = 0;

void init_T () {
	cur_proc_num_T = 0;
    int i;
    for (i = 0; i < MAX_PROCESS_NUM; i++)
        terminated[i] = NULL;
}

void clear_T() {
    int i;
    for(i = 0; i < MAX_PROCESS_NUM; i++) {
        free(terminated[i]);
        terminated[i] = NULL;
    }
    cur_proc_num_T = 0;
}

void insertInto_T (processPointer proc) {
    if(cur_proc_num_T<MAX_PROCESS_NUM) {
        terminated[cur_proc_num_T++] = proc;
    }
    else {
        puts("<ERROR> Cannot terminate the process");
        return;
    }
}

void print_T() {
    puts("\nprintf_T()");

	int i;
    for(i = 0; i < cur_proc_num_T; i++) {
        printf("%d ", terminated[i]->pid);
    }
    printf("\nThe total number of processes: %d\n", cur_proc_num_T);
}

processPointer createProcess(int pid, int priority, int arrivalTime, int CPUburst, int IOburst) {


    if (arrivalTime > MAX_TIME_UNIT || arrivalTime < 0) {
        printf("<ERROR> arrivalTime should be in [0..MAX_TIME_UNIT]\n");
        printf("<USAGE> createProcess(int pid, int priority, int arrivalTime, int CPUburst, int IOburst)\n");
        return NULL;
    }

    if (CPUburst <= 0 || IOburst < 0) {
        printf("<ERROR> CPUburst and should be larger than 0 and IOburst cannot be a negative number.\n");
        printf("<USAGE> createProcess(int pid, int priority, int arrivalTime, int CPUburst, int IOburst)\n");
        return NULL;
    }
        processPointer newProcess = (processPointer)malloc(sizeof(struct myProcess));
        newProcess->pid = pid;
        newProcess->priority = priority;
        newProcess->arrivalTime = arrivalTime;
        newProcess->CPUburst = CPUburst;
        newProcess->IOburst = IOburst;
        newProcess->CPUremainingTime = CPUburst;
        newProcess->IOremainingTime = IOburst;
        newProcess->waitingTime = 0;
        newProcess->turnaroundTime = 0;
        newProcess->responseTime = -1;

        //job queue
        insertInto_JQ(newProcess);

    //debug
    //printf("%d %d %d %d created\n",newProcess.pid ,newProcess.priority, newProcess.arrivalTime, newProcess.CPUburst);
    return newProcess;
}

processPointer FCFS_alg() {

        processPointer earliestProc = readyQueue[0];

        if (earliestProc != NULL){

            if(runningProcess != NULL) {
                /*
				if(runningProcess->arrivalTime > earliestProc->arrivalTime)
					puts("<ERROR> Invalid access.");
	            */
				return runningProcess;
        	} else {
				return removeFrom_RQ(earliestProc);
			}

        } else {
            return runningProcess;
        }
}

processPointer SJF_alg(int preemptive) {

	processPointer shortestJob = readyQueue[0];

	if(shortestJob != NULL) {
		int i;
        for(i = 0; i < cur_proc_num_RQ; i++) {
            if (readyQueue[i]->CPUremainingTime <= shortestJob->CPUremainingTime) {

                if(readyQueue[i]->CPUremainingTime == shortestJob->CPUremainingTime) {
                    if (readyQueue[i]->arrivalTime < shortestJob->arrivalTime) shortestJob = readyQueue[i];
                } else {
                    shortestJob = readyQueue[i];
                }
            }
        }

		if(runningProcess != NULL) {
				if(preemptive){

					if(runningProcess->CPUremainingTime >= shortestJob->CPUremainingTime) {
						if(runningProcess->CPUremainingTime == shortestJob->CPUremainingTime) {
		                    if (runningProcess->arrivalTime < shortestJob->arrivalTime){
								return runningProcess;
							} else if(runningProcess->arrivalTime == shortestJob->arrivalTime)
								return runningProcess;
						}
						puts("preemption is detected.");
						insertInto_RQ(runningProcess);
						return removeFrom_RQ(shortestJob);
					}

					return runningProcess;
				}

				return runningProcess;
        	} else {
				return removeFrom_RQ(shortestJob);
			}

	}else {
		return runningProcess;
	}
}

processPointer PRIORITY_alg(int preemptive) {

	processPointer importantJob = readyQueue[0];

	if(importantJob != NULL) {
		int i;
        for(i = 0; i < cur_proc_num_RQ; i++) {
            if (readyQueue[i]->priority <= importantJob->priority) {

                if(readyQueue[i]->priority == importantJob->priority) {
                    if (readyQueue[i]->arrivalTime < importantJob->arrivalTime)
						importantJob = readyQueue[i];
                } else {
                    importantJob = readyQueue[i];
                }
            }
        }

		if(runningProcess != NULL) {
				if(preemptive){

					if(runningProcess->priority >= importantJob->priority) {
						if(runningProcess->priority == importantJob->priority) {
		                    if (runningProcess->arrivalTime < importantJob->arrivalTime){
								return runningProcess;
							} else if(runningProcess->arrivalTime == importantJob->arrivalTime) {
								return runningProcess;

							}
						}
						puts("preemption is detected.");
						insertInto_RQ(runningProcess);
						return removeFrom_RQ(importantJob);
					}

					return runningProcess;
				}

				return runningProcess;
        	} else {
				return removeFrom_RQ(importantJob);
			}

	}else {
		return runningProcess;
	}
}

processPointer RR_alg(int time_quantum){

	processPointer earliestProc = readyQueue[0];

        if (earliestProc != NULL){

            if(runningProcess != NULL) {
				//return runningProcess;

				if(timeConsumed >= TIME_QUANTUM){
					insertInto_RQ(runningProcess);
					return removeFrom_RQ(earliestProc);
				} else {
					return runningProcess;
				}

        	} else {
				return removeFrom_RQ(earliestProc);
			}

        } else {
            return runningProcess;
        }
}

processPointer LIF_alg(int preemptive) {

	processPointer longestJob = readyQueue[0];
	if(longestJob != NULL) {
		int i;
        for(i = 0; i < cur_proc_num_RQ; i++) {
            if (readyQueue[i]->IOremainingTime >= longestJob->IOremainingTime) {

                if(readyQueue[i]->IOremainingTime == longestJob->IOremainingTime) {
                    if (readyQueue[i]->arrivalTime < longestJob->arrivalTime) longestJob = readyQueue[i];
                } else {
                    longestJob = readyQueue[i];
                }
            }
        }

		if(runningProcess != NULL) {
				if(preemptive){

					if(runningProcess->IOremainingTime <= longestJob->IOremainingTime) {
						if(runningProcess->IOremainingTime == longestJob->IOremainingTime) {
		                    if (runningProcess->arrivalTime < longestJob->arrivalTime){
								return runningProcess;
							} else if(runningProcess->arrivalTime == longestJob->arrivalTime) {
								return runningProcess;
							}
						}
						puts("preemption is detected.");
						insertInto_RQ(runningProcess);
						return removeFrom_RQ(longestJob);
					}

					return runningProcess;
				}

				return runningProcess;
        	} else {
				return removeFrom_RQ(longestJob);
			}

	}else {
		return runningProcess;
	}
}

processPointer LISC_alg(int preemptive) { //Longest IO burst, Shortest CPU burst Algorithm
	processPointer longestJob = readyQueue[0]; //search longest IO burst
	if(longestJob != NULL) {
		int i;
        for(i = 0; i < cur_proc_num_RQ; i++) {
            if (readyQueue[i]->IOremainingTime >= longestJob->IOremainingTime) {

                if(readyQueue[i]->IOremainingTime == longestJob->IOremainingTime) {

					if (readyQueue[i]->CPUremainingTime <= longestJob->CPUremainingTime) {

		                if(readyQueue[i]->CPUremainingTime == longestJob->CPUremainingTime) {
		                    if (readyQueue[i]->arrivalTime < longestJob->arrivalTime) longestJob = readyQueue[i];
		                } else {
		                    longestJob = readyQueue[i];
		                }
		            }

                } else {
                    longestJob = readyQueue[i];
                }
            }
        }

		if(runningProcess != NULL) {
				if(preemptive){

					if(runningProcess->IOremainingTime <= longestJob->IOremainingTime) {
						if(runningProcess->IOremainingTime == longestJob->IOremainingTime) {

							if(runningProcess->CPUremainingTime <= longestJob->CPUremainingTime) {
								if(runningProcess->CPUremainingTime == longestJob->CPUremainingTime){
									if (runningProcess->arrivalTime < longestJob->arrivalTime){
										return runningProcess;
									} else if(runningProcess->arrivalTime == longestJob->arrivalTime) {
										return runningProcess;
									}
								} else {
									return runningProcess;
								}
							}
						}
						puts("preemption is detected.");
						insertInto_RQ(runningProcess);
						return removeFrom_RQ(longestJob);
					}

					return runningProcess;
				}
				return runningProcess;
        	} else {
				return removeFrom_RQ(longestJob);
			}

	}else {
		return runningProcess;
	}
}

processPointer schedule(int alg, int preemptive, int time_quantum) {
	processPointer selectedProcess = NULL;

    switch(alg) {
        case FCFS:
            selectedProcess = FCFS_alg();
            break;
        case SJF:
        	selectedProcess = SJF_alg(preemptive);
        	break;
        case RR:
        	selectedProcess = RR_alg(time_quantum);
        	break;
        case PRIORITY:
        	selectedProcess = PRIORITY_alg(preemptive);
        	break;
        case LIF:
        	selectedProcess = LIF_alg(preemptive);
        	break;
        case LISC:
        	selectedProcess = LISC_alg(preemptive);
        	break;
        default:
        return NULL;
    }

    return selectedProcess;
}

void simulate(int amount, int alg, int preemptive, int time_quantum) {

	processPointer tempProcess = NULL;
	int jobNum = cur_proc_num_JQ;
	int i;
	for(i = 0; i < cur_proc_num_JQ; i++) {
		if(jobQueue[i]->arrivalTime == amount) {
			tempProcess = removeFrom_JQ(jobQueue[i--]);
			insertInto_RQ(tempProcess);
		}
	}
	processPointer prevProcess = runningProcess;
	runningProcess = schedule(alg, preemptive, time_quantum);

	printf("%d: ",amount);
	if(prevProcess != runningProcess) {
		timeConsumed = 0;

		if(runningProcess->responseTime == -1) {
			runningProcess->responseTime = amount - runningProcess->arrivalTime;
		}
	}

    for(i = 0; i < cur_proc_num_RQ; i++) {

        if(readyQueue[i]) {
        	readyQueue[i]->waitingTime++;
        	readyQueue[i]->turnaroundTime++;
    	}
    }

    for(i = 0; i < cur_proc_num_WQ; i++) {
		if(waitingQueue[i]) {
			waitingQueue[i]->turnaroundTime++;
			waitingQueue[i]->IOremainingTime--;

			if(waitingQueue[i]->IOremainingTime <= 0 ) {
				printf("(pid: %d) -> IO complete, ", waitingQueue[i]->pid);
				insertInto_RQ(removeFrom_WQ(waitingQueue[i--]));

			}
		}
	}

    if(runningProcess != NULL) {
        runningProcess->CPUremainingTime --;
        runningProcess->turnaroundTime ++;
        timeConsumed ++;
        printf("(pid: %d) -> running ",runningProcess->pid);

        if(runningProcess->CPUremainingTime <= 0) {
			insertInto_T(runningProcess);
			runningProcess = NULL;
			printf("-> terminated");
		} else {
			if(runningProcess->IOremainingTime > 0) {
				insertInto_WQ(runningProcess);
				runningProcess = NULL;
				printf("-> IO request");
			}
		}

        printf("\n");
    } else {
    	printf("idle\n");
    	Computation_idle++;
	}
}
void analyize(int alg, int preemptive) {

	int wait_sum = 0;
	int turnaround_sum = 0;
	int response_sum = 0;
	int i;
	processPointer p=NULL;
	puts  ("===========================================================");
	for(i=0;i<cur_proc_num_T;i++){
		p = terminated[i];
		printf("(pid: %d)\n",p->pid);
		printf("waiting time = %d, ",p->waitingTime);
		printf("turnaround time = %d, ",p->turnaroundTime);
		printf("response time = %d\n",p->responseTime);

		puts  ("===========================================================");
		wait_sum += p->waitingTime;
		turnaround_sum += p->turnaroundTime;
		response_sum += p->responseTime;
	}
	printf("start time: %d / end time: %d / CPU utilization : %.2lf%% \n",Computation_start, Computation_end,
	 (double)(Computation_end - Computation_idle)/(Computation_end - Computation_start)*100);

	if(cur_proc_num_T != 0) {
		printf("Average waiting time: %d\n",wait_sum/cur_proc_num_T);
		printf("Average turnaround time: %d\n",turnaround_sum/cur_proc_num_T);
		printf("Average response time: %d\n",response_sum/cur_proc_num_T);
	}
		printf("Completed: %d\n",cur_proc_num_T);

	if(cur_proc_num_T != 0) {
		evalPointer newEval = (evalPointer)malloc(sizeof(struct evaluation));
		newEval->alg = alg;
		newEval->preemptive = preemptive;

		newEval->startTime = Computation_start;
		newEval->endTime = Computation_end;
		newEval->avg_waitingTime = wait_sum/cur_proc_num_T;
		newEval->avg_turnaroundTime = turnaround_sum/cur_proc_num_T;
		newEval->avg_responseTime = response_sum/cur_proc_num_T;
		newEval->CPU_util = (double)(Computation_end - Computation_idle)/(Computation_end - Computation_start)*100;
		newEval->completed = cur_proc_num_T;
		evals[cur_eval_num++] = newEval;
	}
	puts  ("===========================================================");

}
void startSimulation(int alg, int preemptive, int time_quantum, int count) {
	loadClone_JQ();

	switch(alg) {
        case FCFS:
            puts("<FCFS Algorithm>");
            break;
        case SJF:
        	if(preemptive) printf("<Preemptive ");
        	else printf("<Non-preemptive ");
        	puts("SJF Algorithm>");
        	break;
        case RR:
        	printf("<Round Robin Algorithm (time quantum: %d)>\n",time_quantum);
        	break;
        case PRIORITY:
        	if(preemptive) printf("<Preemptive ");
        	else printf("<Non-preemptive ");
        	puts("Priority Algorithm>");
        	break;
        case LIF:
        	if(preemptive) printf("<Preemptive ");
        	else printf("<Non-preemptive ");
        	puts("LIF Algorithm>");
        	break;
        case LISC:
        	if(preemptive) printf("<Preemptive ");
        	else printf("<Non-preemptive ");
        	puts("LISC Algorithm>");
        	break;
        default:
        return;
    }

	int initial_proc_num = cur_proc_num_JQ;

	int i;
	if(cur_proc_num_JQ <= 0) {
		puts("<ERROR> Simulation failed. Process doesn't exist in the job queue");
		return;
	}

	int minArriv = jobQueue[0]->arrivalTime;
	for(i=0;i<cur_proc_num_JQ;i++) {
		if(minArriv > jobQueue[i]->arrivalTime)
			minArriv = jobQueue[i]->arrivalTime;
	}
	Computation_start = minArriv;
	Computation_idle = 0;
	for(i=0;i<count;i++) {
		simulate(i,alg, preemptive, TIME_QUANTUM);
		if(cur_proc_num_T == initial_proc_num) {
			i++;
			break;
		}
	}
	Computation_end = i-1;

	analyize(alg, preemptive);
	clear_JQ();
    clear_RQ();
    clear_T();
    clear_WQ();
    free(runningProcess);
    runningProcess = NULL;
    timeConsumed = 0;
    Computation_start = 0;
	Computation_end = 0;
	Computation_idle = 0;
}

void evaluate() {

	puts ("\n                       <Evaluation>                    \n");
	int i;
	for(i=0;i<cur_eval_num;i++) {

		puts ("===========================================================");

		int alg = evals[i]->alg;
		int preemptive = evals[i]->preemptive;

		switch (evals[i]->alg) {

		case FCFS:
            puts("<FCFS Algorithm>");
            break;
        case SJF:
        	if(preemptive) printf("<Preemptive ");
        	else printf("<Non-preemptive ");
        	puts("SJF Algorithm>");
        	break;
        case RR:
        	puts("<Round Robin Algorithm>");
        	break;
        case PRIORITY:
        	if(preemptive) printf("<Preemptive ");
        	else printf("<Non-preemptive ");
        	puts("Priority Algorithm>");
        	break;
        case LIF:
        	if(preemptive) printf("<Preemptive ");
        	else printf("<Non-preemptive ");
        	puts("LIF Algorithm>");
        	break;
        case LISC:
        	if(preemptive) printf("<Preemptive ");
        	else printf("<Non-preemptive ");
        	puts("LISC Algorithm>");
        	break;
        default:
        return;
		}
		puts ("-----------------------------------------------------------");
		printf("start time: %d / end time: %d / CPU utilization : %.2lf%% \n",evals[i]->startTime,evals[i]->endTime,evals[i]->CPU_util);
		printf("Average waiting time: %d\n",evals[i]->avg_waitingTime);
		printf("Average turnaround time: %d\n",evals[i]->avg_turnaroundTime);
		printf("Average response time: %d\n",evals[i]->avg_responseTime);
		printf("Completed: %d\n",evals[i]->completed);
	}

	puts  ("===========================================================");
}


void createProcesses(int total_num, int io_num) {
	if(io_num > total_num) {
		puts("<ERROR> The number of IO event cannot be higher than the number of processes");
		exit(-1);
	}

	srand(time(NULL));

	int i;
	for(i=0;i<total_num; i++) {
		createProcess(i+1, rand() % total_num + 1, rand() % (total_num + 10), rand() % 16 + 5, 0);
	}

	for(i=0;i<io_num;i++) {

		int randomIndex = rand() % total_num ;
		if(jobQueue[randomIndex]->IOburst ==0) {

			int randomIOburst = rand() % 10 + 1;
			jobQueue[randomIndex]->IOburst = randomIOburst;
			jobQueue[randomIndex]->IOremainingTime = randomIOburst;

		} else {
			i--;
		}

	}
	sort_JQ();
	clone_JQ(); //backup this JQ
	print_JQ();
}

void main(int argc, char **argv) {
    init_RQ();
    init_JQ();
    init_T();
    init_WQ();
    init_evals();

    int totalProcessNum = atoi(argv[1]);
    int totalIOProcessNum = atoi(argv[2]);
    createProcesses(totalProcessNum,totalIOProcessNum);
    int i;
    int amount = 120;
 	startSimulation(FCFS,FALSE,TIME_QUANTUM, amount);

    startSimulation(SJF,FALSE,TIME_QUANTUM, amount);
    startSimulation(SJF,TRUE,TIME_QUANTUM, amount);
	startSimulation(PRIORITY,FALSE,TIME_QUANTUM, amount);
	startSimulation(PRIORITY,TRUE,TIME_QUANTUM, amount);
	startSimulation(RR,TRUE,TIME_QUANTUM, amount);
	startSimulation(LIF,FALSE, TIME_QUANTUM, amount);
	startSimulation(LIF,TRUE, TIME_QUANTUM, amount);
	startSimulation(LISC,FALSE, TIME_QUANTUM, amount);
	startSimulation(LISC,TRUE, TIME_QUANTUM, amount);
	evaluate();

	clear_JQ();
    clear_RQ();
    clear_T();
    clear_WQ();
    clearClone_JQ();

	clear_evals();
}
