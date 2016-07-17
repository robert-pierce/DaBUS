/*
 * student.c
 * Multithreaded OS Simulation for CS 2200, Project 6
 * Fall 2015
 *
 * This file contains the CPU scheduler for the simulation.
 * Name: Robert Pierce
 * GTID: 901554418
 */

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"
#include "os-sim.h"

/* Thread Wrappers */
void Pthread_mutex_lock(pthread_mutex_t *mutex) {
  int rc = pthread_mutex_lock(mutex);
  assert(rc == 0);
}

void Pthread_mutex_unlock(pthread_mutex_t *mutex) {
  int rc = pthread_mutex_unlock(mutex);
  assert(rc == 0);
}

void Pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
  int rc = pthread_cond_wait(cond, mutex);
  assert(rc == 0);
}

void Pthread_cond_signal(pthread_cond_t *cond) {
  int rc = pthread_cond_signal(cond);
  assert(rc == 0);
}

void Pthread_cond_broadcast(pthread_cond_t *cond) {
  int rc = pthread_cond_broadcast(cond);
  assert(rc == 0);
}


/*
 * current[] is an array of pointers to the currently running processes.
 * There is one array element corresponding to each CPU in the simulation.
 *
 * current[] should be updated by schedule() each time a process is scheduled
 * on a CPU.  Since the current[] array is accessed by multiple threads, you
 * will need to use a mutex to protect it.  current_mutex has been provided
 * for your use.
 */
static pcb_t **current;
static pthread_mutex_t current_mutex;



/* declare a ready queue and a mutex for the queue*/ 
ready_queue_t *ready_queue;
pthread_mutex_t  ready_queue_mutex;


/* declare idle condition variable*/
pthread_cond_t idle_cond = PTHREAD_COND_INITIALIZER;

/* declare scheduling algorithm flags*/
int round_robin_flag;
int static_priority_flag;
    
/* declare time slice variable*/
int time_slice;



/* Init the ready queue */
void init_ready_queue() {
  ready_queue = (ready_queue_t*) malloc (sizeof(ready_queue_t));
  ready_queue->count = 0;
}

/* ready_queue operations */

/*
 * Places a pcb in the ready queue
 * @param pcb A pointer to the pcb being added to the ready queue
 */
void enqueue_ready(pcb_t *pcb) {
  Pthread_mutex_lock(&ready_queue_mutex);    // lock it down
  
  if( (ready_queue->count) == 0) {           // check if queue is empty
    ready_queue->head = pcb;                 // if empty then point head and tail to new block
    ready_queue->tail = pcb;
    Pthread_cond_broadcast(&idle_cond);         // signal that the ready queue is not empty
  } else {              
    ready_queue->tail->next = pcb;           // if not empty add new block to end of queue
    ready_queue->tail = pcb;                 // point tail to this new block
  }

  ready_queue->count++;                      // increment the ready queue count

  Pthread_mutex_unlock(&ready_queue_mutex);  // unlock
} 

/*
 * Removes a block from the ready_queue
 * @return a pointer to the removed pcb
 */
pcb_t* dequeue_ready() {
  pcb_t* temp;
  Pthread_mutex_lock(&ready_queue_mutex);   // lock it down
  
  if( (ready_queue->count) == 0) {          // check if the queue is empty
    Pthread_mutex_unlock(&ready_queue_mutex); // unlock
    return NULL;                            // if empty then return NULL, nothing to dequeue
  } else if ( (ready_queue->count) == 1) {  // check if there is only 1 block in ready queue
    temp = ready_queue->head;
    ready_queue->head = NULL;               // if only one block then set head and tail to NULL
    ready_queue->tail = NULL;
  } else {                                  // more than one block in queue
    temp = ready_queue->head;         
    ready_queue->head = temp->next;         // remove the head and set next pcb to head
  } 
  
  ready_queue->count--;                     // decrement ready_queue count
 
  Pthread_mutex_unlock(&ready_queue_mutex); // release the lock
  return temp;                              // return the block
}


/*
 * schedule() is your CPU scheduler.  It should perform the following tasks:
 *
 *   1. Select and remove a runnable process from your ready queue which 
 *	you will have to implement with a linked list or something of the sort.
 *
 *   2. Set the process state to RUNNING
 *
 *   3. Call context_switch(), to tell the simulator which process to execute
 *      next on the CPU.  If no process is runnable, call context_switch()
 *      with a pointer to NULL to select the idle process.
 *	The current array (see above) is how you access the currently running
 *	process indexed by the cpu id. See above for full description.
 *	context_switch() is prototyped in os-sim.h. Look there for more information 
 *	about it and its parameters.
 */
static void schedule(unsigned int cpu_id)
{
  pcb_t *pcb;                            
  pcb = dequeue_ready();                    // dequeue from the ready queue
  
  if(pcb != NULL) {                        // set the process' state to running, if it is not NULL
    pcb->state = PROCESS_RUNNING;
  }

  Pthread_mutex_lock(&current_mutex);       // lock down the current[]
  current[cpu_id] = pcb;
  Pthread_mutex_unlock(&current_mutex);     // unlock

  context_switch(cpu_id, pcb, time_slice);  // invoke the scheduler 
}


/*
 * idle() is your idle process.  It is called by the simulator when the idle
 * process is scheduled.
 *
 * This function should block until a process is added to your ready queue.
 * It should then call schedule() to select the process to run on the CPU.
 */
extern void idle(unsigned int cpu_id)
{
  Pthread_mutex_lock(&ready_queue_mutex);   // lock down using the ready queue mutex
  
  while( (ready_queue->count)  == 0) {
    Pthread_cond_wait(&idle_cond, &ready_queue_mutex);
  }
  
  Pthread_mutex_unlock(&ready_queue_mutex);  // unlock
  schedule(cpu_id);
   

  /*
     * REMOVE THE LINE BELOW AFTER IMPLEMENTING IDLE()
     *
     * idle() must block when the ready queue is empty, or else the CPU threads
     * will spin in a loop.  Until a ready queue is implemented, we'll put the
     * thread to sleep to keep it from consuming 100% of the CPU time.  Once
     * you implement a proper idle() function using a condition variable,
     * remove the call to mt_safe_usleep() below.
     */
  //***************REMOVED**************************//
 
}


/*
 * preempt() is the handler called by the simulator when a process is
 * preempted due to its timeslice expiring.
 *
 * This function should place the currently running process back in the
 * ready queue, and call schedule() to select a new runnable process.
 */
extern void preempt(unsigned int cpu_id)
{
    /* FIX ME */
}


/*
 * yield() is the handler called by the simulator when a process yields the
 * CPU to perform an I/O request.
 *
 * It should mark the process as WAITING, then call schedule() to select
 * a new process for the CPU.
 */
extern void yield(unsigned int cpu_id)
{
  pcb_t *pcb;

  Pthread_mutex_lock(&current_mutex);      // lock it down
  
  // get the currently running process on the cpu in quesition
  pcb = current[cpu_id];
  pcb->state = PROCESS_WAITING;           // update the process state to waiting
 
  Pthread_mutex_unlock(&current_mutex);   // unlock

  schedule(cpu_id);                       // invoke the scheduler  
}


/*
 * terminate() is the handler called by the simulator when a process completes.
 * It should mark the process as terminated, then call schedule() to select
 * a new process for the CPU.
 */
extern void terminate(unsigned int cpu_id)
{
  pcb_t *pcb;
  
  Pthread_mutex_lock(&current_mutex);          // lock it down

  // get the currently running process in the CPU in question
  pcb = current[cpu_id];                        
  pcb->state = PROCESS_TERMINATED;             // update the process state to terminated

  Pthread_mutex_unlock(&current_mutex);        // unlock

  schedule(cpu_id);                            // invoke the scheduler
}


/*
 * wake_up() is the handler called by the simulator when a process's I/O
 * request completes.  It should perform the following tasks:
 *
 *   1. Mark the process as READY, and insert it into the ready queue.
 *
 *   2. If the scheduling algorithm is static priority, wake_up() may need
 *      to preempt the CPU with the lowest priority process to allow it to
 *      execute the process which just woke up.  However, if any CPU is
 *      currently running idle, or all of the CPUs are running processes
 *      with a higher priority than the one which just woke up, wake_up()
 *      should not preempt any CPUs.
 *	To preempt a process, use force_preempt(). Look in os-sim.h for 
 * 	its prototype and the parameters it takes in.
 */
extern void wake_up(pcb_t *process)
{
  process->state = PROCESS_READY;
  enqueue_ready(process);
}


/*
 * main() simply parses command line arguments, then calls start_simulator().
 * You will need to modify it to support the -r and -p command-line parameters.
 */
int main(int argc, char *argv[])
{
  int cpu_count, i;
    round_robin_flag = 0;
    static_priority_flag = 0;
    time_slice = -1;
    
    /* Parse command-line arguments */
    if (argc < 2 || argc > 5)
    {
        fprintf(stderr, "CS 2200 Project 6 -- Multithreaded OS Simulator\n"
            "Usage: ./os-sim <# CPUs> [ -r <time slice> | -p ]\n"
            "    Default : FIFO Scheduler\n"
            "         -r : Round-Robin Scheduler\n"
            "         -p : Static Priority Scheduler\n\n");
        return -1;
    }
    cpu_count = atoi(argv[1]);

    /* FIX ME - Add support for -r and -p parameters*/
    for(i = 0; i < argc; i++) {
      if(strcmp(argv[i], "-r") == 0) {
        round_robin_flag = 1;
        time_slice = atoi(argv[i + 1]);
      }
      
      if(strcmp(argv[i], "-p") == 0) {
        static_priority_flag = 1;
      }
    }

    /* Allocate the current[] array and its mutex */
    current = malloc(sizeof(pcb_t*) * cpu_count);
    assert(current != NULL);
    pthread_mutex_init(&current_mutex, NULL);

    /* Init the ready queue and its mutex*/
     init_ready_queue();
     pthread_mutex_init(&ready_queue_mutex, NULL);

    /* Start the simulator in the library */
    start_simulator(cpu_count);

    return 0;
}


