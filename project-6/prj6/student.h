/*
 * student.h
 * Multithreaded OS Simulation for CS 2200, Project 6 - Fall 2016
 *
 * YOU WILL NOT NEED TO MODIFY THIS FILE
 *
 */

#ifndef __STUDENT_H__
#define __STUDENT_H__

#include "os-sim.h"

/* Function declarations */
extern void idle(unsigned int cpu_id);
extern void preempt(unsigned int cpu_id);
extern void yield(unsigned int cpu_id);
extern void terminate(unsigned int cpu_id);
extern void wake_up(pcb_t *process);

void Pthread_mutex_lock(pthread_mutex_t *mutex);
void Pthread_mutex_unlock(pthread_mutex_t *mutex);
void Pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
void Pthread_cond_signal(pthread_cond_t *cond);
void Pthread_cond_broadcase(pthread_cond_t *cond);
void init_ready_queue();
void enqueue_ready(pcb_t *pcb);
pcb_t* dequeue_ready();


typedef struct {
  pcb_t *head;
  pcb_t *tail;
  int   count;
} ready_queue_t;

#endif /* __STUDENT_H__ */
