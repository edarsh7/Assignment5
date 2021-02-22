 /**
 * See scheduler.h for function details. All are callbacks; i.e. the simulator 
 * calls you when something interesting happens.
 */
#include <stdlib.h>
#include <stdio.h>
#include "simulator.h"
#include "scheduler.h"

typedef struct node {
    thread_t * thread;
    struct node* next;
    int quantum_ct;
}node;

unsigned int q_value;

//global thread variable to hold the running thread
thread_t * running_thread = NULL;

//global head variable to hold ready queue
struct node *head = NULL;
struct node *thread_list = NULL;

void append(struct node** head_ref, thread_t * t);
void pop(struct node** head_ref);

void scheduler(enum algorithm algorithm, unsigned int quantum) 
{
  q_value = quantum;
}

void sim_tick() { }

void sim_ready() 
{
  if(running_thread != NULL && head != NULL)
  { 
    if(head->quantum_ct == 0)
    {
      thread_t *temp = head->thread;
      pop(&head);
      append(&head, temp);
      sim_dispatch(head->thread);
      running_thread = head->thread;
    }
    head->quantum_ct--;
  }
}

void sys_exec(thread_t *t) 
{ 
  append(&head, t);
  append(&thread_list, t);
  

  if(head != NULL)
  {
    sim_dispatch(head->thread);
    running_thread  = head->thread;
  }
}

void sys_read(thread_t *t) 
{ 
  pop(&head);
  if(head != NULL)
  {
    sim_dispatch(head->thread);
    running_thread = head->thread;
  }
}

void sys_write(thread_t *t) 
{
  pop(&head);
  if(head != NULL)
  {
    sim_dispatch(head->thread);
    running_thread = head->thread;
  }
}

void sys_exit(thread_t *t) 
{ 
  pop(&head);
  if(head != NULL)
  {
    sim_dispatch(head->thread);
    running_thread = head->thread;
  }
}

void io_complete(thread_t *t) 
{ 
  append(&head, t);
  if(head != NULL)
  {
    sim_dispatch(head->thread);
    running_thread = head->thread;
  }
}

void io_starting(thread_t *t) { }

stats_t *stats() { }




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
  new_node->quantum_ct = q_value;
  /* 4. If the Linked List is empty, then make the new
        node as head */
  if (*head_ref == NULL) {
      *head_ref = new_node;
      return;
  }

  /* 5. Else traverse till the last node */
  while (last->next != NULL)
      last = last->next;

  /* 6. Change the next of last node */
  last->next = new_node;

  return;
}

void pop(struct node** head_ref)
{
  struct node *t;

  if(*head_ref == NULL)
  {
    return;
  }

  
  t = (*head_ref)->next;
  (*head_ref) = NULL;
  (*head_ref) = t;
  

}



