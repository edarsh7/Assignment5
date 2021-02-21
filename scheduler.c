 /**
 * See scheduler.h for function details. All are callbacks; i.e. the simulator 
 * calls you when something interesting happens.
 */
#include <stdlib.h>
#include <stdio.h>
#include "simulator.h"
#include "scheduler.h"

struct node {
    thread_t * thread;
    struct node* next;
    struct node* prev;
}node;

void append(node** head_ref, thread_t * t);

void scheduler(enum algorithm algorithm, unsigned int quantum) 
{ }

void sim_tick() { }

void sim_ready() { }

void sys_exec(thread_t *t) { }

void sys_read(thread_t *t) { }

void sys_write(thread_t *t) { }

void sys_exit(thread_t *t) { }

void io_complete(thread_t *t) { }

void io_starting(thread_t *t) { }

stats_t *stats() { 
  
}



// -------------------- list functions from geeksforgeeks -------------

void append(struct node** head_ref, thread_t * t)
{
    /* 1. allocate node */
    struct node* new_node
        = (struct node*)malloc(sizeof(struct node));
 
    struct node* last = *head_ref; /* used in step 5*/
 
    /* 3. This new node is going to be the last node, so
          make next of it as NULL*/
    new_node->next = NULL;
    new_node->thread = t;
 
    /* 4. If the Linked List is empty, then make the new
          node as head */
    if (*head_ref == NULL) {
        new_node->prev = NULL;
        *head_ref = new_node;
        return;
    }
 
    /* 5. Else traverse till the last node */
    while (last->next != NULL)
        last = last->next;
 
    /* 6. Change the next of last node */
    last->next = new_node;
 
    /* 7. Make last node as previous of new node */
    new_node->prev = last;
 
    return;
}