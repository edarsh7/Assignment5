/**
 * See scheduler.h for function details. All are callbacks; i.e. the simulator 
 * calls you when something interesting happens.
 */
#include <stdlib.h>

#include "simulator.h"
#include "scheduler.h"

void scheduler(enum algorithm algorithm, unsigned int quantum) { }

void sim_tick() { }

void sim_ready() { }

void sys_exec(thread_t *t) { }

void sys_read(thread_t *t) { }

void sys_write(thread_t *t) { }

void sys_exit(thread_t *t) { }

void io_complete(thread_t *t) { }

void io_starting(thread_t *t) { }

stats_t *stats() { 
  int i = 0; // Remove - only here to fail code warnings check
  return NULL; 
}
