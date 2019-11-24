/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"


/**
  Stores information making up a job to be scheduled including any statistics.

  You may need to define some global variables or a struct to store your job queue elements.
*/
typedef struct _job_t
{
  // _job_t * next;
  int aTime;
  int rTime;
  int pri;
  int remTime;
  int jobNum;
  int cN;

} job_t;

typedef struct _scheduler_t
{
  // pthread_t * t;
  priqueue_t q;
  int (*comp)(const void *, const void *);
  int * c;
  int nCores;
  scheme_t s;
}s_t;

s_t sc;

int compareFCFS(const void * a, const void * b)
{
  return ( (*(job_t*)a).aTime - (*(job_t*)b).aTime );
}

int compareSJF(const void * a, const void * b)
{
  return ( (*(job_t*)a).rTime - (*(job_t*)b).rTime );
}

int comparePRI(const void * a, const void * b)
{
  return ( (*(job_t*)a).pri - (*(job_t*)b).pri );
}

int comparePSJF(const void * a, const void * b)
{
  return ( (*(job_t*)a).remTime - (*(job_t*)b).remTime );
}

/**
  Initalizes the scheduler.

  Assumptions:
    - You may assume this will be the first scheduler function called.
    - You may assume this function will be called once once.
    - You may assume that cores is a positive, non-zero number.
    - You may assume that scheme is a valid scheduling scheme.

  @param cores the number of cores that is available by the scheduler. These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
  @param scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
*/
void scheduler_start_up(int cores, scheme_t scheme)
{
  sc.c = malloc(cores*sizeof(int));
  sc.nCores = cores;
  sc.s = scheme;
  for(int i =0; i<cores; i++){
    sc.c[i] = -1;
  }
  switch (scheme){
    case FCFS:
              sc.comp = &compareFCFS;
              break;
    case SJF:
            sc.comp = &compareSJF;
            break;
    case PRI:
            sc.comp = &comparePRI;
            break;
    case PSJF:
            sc.comp = &comparePSJF;
            break;
    default:
          printf("default\n");
          break;
  }
 // sc.t = malloc(cores * sizeof(pthread_t));
 // for(int i =0; i<cores; i++){
 //   sc.t[i] = '\0';
 // }
 priqueue_init(&sc.q, sc.comp);
}


/**
  Called when a new job arrives.

  If multiple cores are idle, the job should be assigned to the core with the
  lowest id.
  If the job arriving should be scheduled to run during the next
  time cycle, return the zero-based index of the core the job should be
  scheduled on. If another job is already running on the core specified,
  this will preempt the currently running job.
  Assumption:
    - You may assume that every job wil have a unique arrival time.

  @param job_number a globally unique identification number of the job arriving.
  @param time the current time of the simulator.
  @param running_time the total number of time units this job will run before it will be finished.
  @param priority the priority of the job. (The lower the value, the higher the priority.)
  @return index of core job should be scheduled on
  @return -1 if no scheduling changes should be made.

 */
int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
  job_t * j = malloc(sizeof(job_t));
  j->aTime = time;
  j->jobNum = job_number;
  j->rTime = j->remTime = running_time;
  j->pri = priority;
  priqueue_offer(&sc.q, (void *)j);
  for(int i = 0; i<sc.nCores; i++){
    if(sc.c[i] == -1){
      sc.c[i] = ((job_t *)priqueue_poll(&sc.q))->jobNum;
      j->cN = i;
      return i;
    }
  }
  if((job_t *)priqueue_peek(&sc.q) == j && sc.s == PSJF){
    // int tJ = (job_t *)priqueue_peek(&sc.q)->jobNum;

    // priqueue_at(&sc.q,1)
  }

	return -1;
}


/**
  Called when a job has completed execution.

  The core_id, job_number and time parameters are provided for convenience. You may be able to calculate the values with your own data structure.
  If any job should be scheduled to run on the core free'd up by the
  finished job, return the job_number of the job that should be scheduled to
  run on core core_id.

  @param core_id the zero-based index of the core where the job was located.
  @param job_number a globally unique identification number of the job.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled to run on core core_id
  @return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time)
{
  if(priqueue_size(&sc.q) == 0){
    sc.c[core_id] = -1;
    return -1;
  }
  else{
    // sc.c[core_id] = -1;
    // priqueue_remove_at(&sc.q,0);
    // if(priqueue_size(&sc.q) == 1)
      // return -1;
    // else
      return ((job_t *)priqueue_poll(&sc.q))->jobNum;
  }
}


/**
  When the scheme is set to RR, called when the quantum timer has expired
  on a core.

  If any job should be scheduled to run on the core free'd up by
  the quantum expiration, return the job_number of the job that should be
  scheduled to run on core core_id.

  @param core_id the zero-based index of the core where the quantum has expired.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled on core cord_id
  @return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time)
{
	return -1;
}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
	return 0.0;
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
	return 0.0;
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
	return 0.0;
}


/**
  Free any memory associated with your scheduler.

  Assumption:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{

}


/**
  This function may print out any debugging information you choose. This
  function will be called by the simulator after every call the simulator
  makes to your scheduler.
  In our provided output, we have implemented this function to list the jobs in the order they are to be scheduled. Furthermore, we have also listed the current state of the job (either running on a given core or idle). For example, if we have a non-preemptive algorithm and job(id=4) has began running, job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority, the output in our sample output will be:

    2(-1) 4(0) 1(-1)

  This function is not required and will not be graded. You may leave it
  blank if you do not find it useful.
 */
void scheduler_show_queue()
{
  for (int i = 0; i < priqueue_size(&sc.q); i++)
		printf("val: %d ", ((job_t *)priqueue_at(&sc.q, i))->jobNum );
	printf("\n");
}
