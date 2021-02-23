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
    int arrival;
    int completion;
    int turnaround;
    int ready_q;
    int io_wait;
    int io_start;
    int waittime;
    int done;
}node;

//global variables to hold important info
int count=0;
unsigned int q_value;
enum algorithm algo_number;

//global thread variable to hold the running thread
thread_t * running_thread = NULL;
thread_t * io_thread = NULL;
thread_t * td_off_cpu = NULL;

//global head variable to hold ready queue
struct node *head = NULL;
struct node *thread_list = NULL;

//borrowed list functions ::: CREDITS GIVEN IN COMMENTS BEFORE IMPLEMENTATION
void sortedInsert(struct node** head_ref, thread_t *t);
void append(struct node** head_ref, thread_t * t);
void pop(struct node** head_ref);

void turnaround(thread_t *td);

// ROUND ROBIN SET OF FUNCTIONS
void rr_sysready();
void rr_sysexec(thread_t *t);
void rr_sys_rd_wr(thread_t *t);
void rr_sysexit(thread_t *t);
void rr_iocomplete(thread_t *t);
void rr_iostarting(thread_t *t);

//NON-PREEMPTIVE PRIORITY SET OF FUNCTIONS
void np_prio_sysready();
void np_prio_sysexec(thread_t *t);
void np_prio_sys_rd_wr(thread_t *t);
void np_prio_sysexit(thread_t *t);
void np_prio_iocomplete(thread_t *t);
void np_prio_iostarting(thread_t *t);

//PREEMPTIVE PRIORITY SET OF FUNCTIONS
void prmtv_prio_sysready();
void prmtv_prio_sysexec(thread_t *t);
void prmtv_prio_sys_rd_wr(thread_t *t);
void prmtv_prio_sysexit(thread_t *t);
void prmtv_prio_iocomplete(thread_t *t);
void prmtv_prio_iostarting(thread_t *t);


void scheduler(enum algorithm algorithm, unsigned int quantum) 
{
  q_value = quantum;
  algo_number = algorithm;
}

void sim_tick() { }

void sim_ready() 
{
  if(algo_number == ROUND_ROBIN)
  {
    rr_sysready();
  }
  else if(algo_number == NON_PREEMPTIVE_PRIORITY)
  {
    np_prio_sysready();
  }
  else if(algo_number == PREEMPTIVE_PRIORITY)
  {
    prmtv_prio_sysready();
  }
}

void sys_exec(thread_t *t) 
{
  count++;
  if(algo_number == ROUND_ROBIN)
  {
    rr_sysexec(t);
  }
  else if(algo_number == NON_PREEMPTIVE_PRIORITY)
  {
    np_prio_sysexec(t);
  }
  else if(algo_number == PREEMPTIVE_PRIORITY)
  {
    prmtv_prio_sysexec(t);
  }
}

void sys_read(thread_t *t) 
{ 
  if(algo_number == ROUND_ROBIN)
  {
    rr_sys_rd_wr(t);
  }
  else if(algo_number == NON_PREEMPTIVE_PRIORITY)
  {
    np_prio_sys_rd_wr(t);
  }
  else if(algo_number == PREEMPTIVE_PRIORITY)
  {
    prmtv_prio_sys_rd_wr(t);
  }
}

void sys_write(thread_t *t) 
{
  if(algo_number == ROUND_ROBIN)
  {
    rr_sys_rd_wr(t);
  }
  else if(algo_number == NON_PREEMPTIVE_PRIORITY)
  {
    np_prio_sys_rd_wr(t);
  }
  else if(algo_number == PREEMPTIVE_PRIORITY)
  {
    prmtv_prio_sys_rd_wr(t);
  }
}

void sys_exit(thread_t *t) 
{ 
  if(algo_number == ROUND_ROBIN)
  {
    rr_sysexit(t);
  }
  else if(algo_number == NON_PREEMPTIVE_PRIORITY)
  {
    np_prio_sysexit(t);
  }
  else if(algo_number == PREEMPTIVE_PRIORITY)
  {
    prmtv_prio_sysexit(t);
  }
}

void io_complete(thread_t *t) 
{ 
  if(algo_number == ROUND_ROBIN)
  {
    rr_iocomplete(t);
  }
  else if(algo_number == NON_PREEMPTIVE_PRIORITY)
  {
    np_prio_iocomplete(t);
  }
  else if(algo_number == PREEMPTIVE_PRIORITY)
  {
    prmtv_prio_iocomplete(t);
  }
}

void io_starting(thread_t *t)
{
  if(algo_number == ROUND_ROBIN)
  {
    rr_iostarting(t);
  }
  else if(algo_number == NON_PREEMPTIVE_PRIORITY)
  {
    np_prio_iostarting(t);
  }
  else if(algo_number == PREEMPTIVE_PRIORITY)
  {
    prmtv_prio_iostarting(t);
  }
}

stats_t *stats()
{ 
  int thread_count = count;
  stats_t *stats = malloc(sizeof(stats_t));
  stats->tstats = malloc(sizeof(stats_t) * thread_count);


  struct node *temp = thread_list;

  while(temp != NULL)
  {
    turnaround(temp->thread);
    temp = temp->next;
  }

  temp = thread_list;
  int x = 0;
  int y = 0;
  while(temp != NULL)
  {
    stats->tstats[temp->thread->tid - 1].tid = temp->thread->tid;
    stats->tstats[temp->thread->tid - 1].turnaround_time = temp->turnaround;
    stats->tstats[temp->thread->tid - 1].waiting_time = temp->waittime; 
    x = x + temp->turnaround;
    y = y + temp->waittime;
    temp=temp->next;
  }
  stats->thread_count = count;
  stats->turnaround_time = x/count;
  stats->waiting_time = y/count;

  return stats;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*= = = = = = = = = = = = = = = = = ROUND ROBIN FUNCTIONS = = = = = = = = = = = = = = = = =*/

void rr_sysready()
{
  if(running_thread != NULL && head != NULL)
  { 
    if(head->quantum_ct == 0)
    {
      struct node *temp_x;
      temp_x = thread_list;
      while(temp_x->thread->tid != head->thread->tid)
      {
        temp_x = temp_x->next;
      }
      temp_x->ready_q = 1;

      thread_t *temp = head->thread;
      pop(&head);
      append(&head, temp);
      sim_dispatch(head->thread);
      head->ready_q = 0;
      running_thread = head->thread;
    }
    head->quantum_ct--;
  }

  struct node *temp_n;
  temp_n = thread_list;
  while(temp_n != NULL)
  {
    if(temp_n->thread != running_thread && temp_n->done == 0)
    {
      if(temp_n->ready_q == 1)
      {
        temp_n->waittime++;
      }
    }
    temp_n = temp_n->next;
  }
}

void rr_sysexec(thread_t *t)
{

  append(&head, t);
  append(&thread_list, t);

  struct node *temp;
  temp = thread_list;
  while(temp->thread->tid != t->tid)
  {
    temp = temp->next;
  }
  temp->arrival = sim_time();
  temp->waittime = 0;
  temp->ready_q = 1;
  temp->done = 0;
  
  if(head != NULL)
  {
    sim_dispatch(head->thread);
    running_thread = head->thread;
  }

  if(running_thread == temp->thread)
  {
    temp->ready_q = 0;
  }
}

void rr_sys_rd_wr(thread_t *t)
{
  
  struct node *temp;
  temp = thread_list;
  while(temp->thread->tid != t->tid)
  {
    temp = temp->next;
  }
  temp->ready_q = 0;
  temp->io_wait = sim_time();

  pop(&head);
  if(head != NULL)
  {
    sim_dispatch(head->thread);
    running_thread = head->thread;
  }
}

void rr_sysexit(thread_t *t)
{
  
  struct node *temp;
  temp = thread_list;
  while(temp->thread->tid != t->tid)
  {
    temp = temp->next;
  }
  temp->completion = sim_time();
  temp->ready_q = 0;
  temp->done = 1;

  pop(&head);
  if(head != NULL)
  {
    sim_dispatch(head->thread);
    running_thread = head->thread;
  }
  if(running_thread == temp->thread)
  {
    temp->ready_q = 0;
  }
}

void rr_iocomplete(thread_t *t)
{
  
  struct node *temp;
  temp = thread_list;
  while(temp->thread->tid != t->tid)
  {
    temp = temp->next;
  }
  temp->ready_q = 1;

  append(&head, t);
  if(head != NULL)
  {
    sim_dispatch(head->thread);
    running_thread = head->thread;
  }
  if(running_thread == temp->thread)
  {
    temp->ready_q = 0;
  }
  io_thread = NULL;
}

void rr_iostarting(thread_t *t)
{
  struct node *temp;
  temp = thread_list;
  while(temp->thread->tid != t->tid)
  {
    temp = temp->next;
  }
  temp->ready_q = 0;
  temp->io_start = sim_time();

  temp->waittime = temp->waittime + (temp->io_start - temp->io_wait - 1);
  temp->io_wait = 0;
  temp->io_start = 0;

  if(head != NULL)
  {
    sim_dispatch(head->thread);
    running_thread = head->thread;
  }
  if(running_thread == temp->thread)
  {
    temp->ready_q = 0;
  }
}
/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*= = = = = = = = = = = = = = = = = NP_PRIO FUNCTIONS = = = = = = = = = = = = = = = = =*/

void np_prio_sysready()
{

  struct node *temp;

  if(running_thread == NULL && head != NULL)
  {
    running_thread = head->thread;
    sim_dispatch(running_thread);
    pop(&head);
    
    temp = thread_list;
    while(temp->thread != running_thread)
    {
      temp = temp->next;
    }
    temp->ready_q = 0;
  }

  temp = thread_list;
  while(temp != NULL)
  {
    if(temp->ready_q == 1 && temp->thread != running_thread && temp->done != 1)
    {
      temp->waittime++;
    }
    temp = temp->next;
  }
  
}

void np_prio_sysexec(thread_t *t)
{  
  append(&thread_list, t);
  sortedInsert(&head, t);

  struct node *temp;
  temp = thread_list;
  while(temp->thread->tid != t->tid)
  {
    temp = temp->next;
  }
  temp->arrival = sim_time();
  temp->ready_q = 1;
}

void np_prio_sys_rd_wr(thread_t *t)
{
  running_thread = NULL;
  struct node *temp;
  temp = thread_list;
  while(temp->thread->tid != t->tid)
  {
    temp = temp->next;
  }
  temp->ready_q = 0;
  temp->io_wait = sim_time();
}

void np_prio_sysexit(thread_t *t)
{
  running_thread = NULL;

  struct node *temp;
  temp = thread_list;
  while(temp->thread->tid != t->tid)
  {
    temp = temp->next;
  }
  temp->completion = sim_time();
  temp->ready_q = 0;

}

void np_prio_iocomplete(thread_t *t)
{
  sortedInsert(&head, t);
  struct node *temp;
  temp = thread_list;
  while(temp->thread->tid != t->tid)
  {
    temp = temp->next;
  }
  temp->ready_q = 1;
}

void np_prio_iostarting(thread_t *t)
{
  struct node *temp;
  temp = thread_list;
  while(temp->thread->tid != t->tid)
  {
    temp = temp->next;
  }
  temp->ready_q = 0;
  temp->io_start = sim_time();

  temp->waittime = temp->waittime + (temp->io_start - temp->io_wait - 1);
}

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*= = = = = = = = = = = = = = = = = NP_PRIO FUNCTIONS = = = = = = = = = = = = = = = = =*/
void prmtv_prio_sysready()
{

  if(head != NULL)
  {
    sim_dispatch(head->thread);
    running_thread = head->thread;
  }
}

void prmtv_prio_sysexec(thread_t *t)
{
  append(&thread_list, t);
  sortedInsert(&head, t);
}

void prmtv_prio_sys_rd_wr(thread_t *t)
{
  running_thread = NULL;
  if(head->thread == t)
    pop(&head);
}

void prmtv_prio_sysexit(thread_t *t)
{
  running_thread = NULL;
  if(head->thread == t)
    pop(&head);
}

void prmtv_prio_iocomplete(thread_t *t)
{
  sortedInsert(&head, t);
}

void prmtv_prio_iostarting(thread_t *t)
{

}


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

void turnaround(thread_t *td)
{
  struct node *temp;
  temp = thread_list;

  while(temp->thread->tid != td->tid)
  {
    temp = temp->next;
  }

  temp->turnaround = temp->completion - temp->arrival + 1;
}

//borrowed from https://www.geeksforgeeks.org/given-a-linked-list-which-is-sorted-how-will-you-insert-in-sorted-way/
//then altered for use in this assignment
void sortedInsert(struct node** head_ref, thread_t *t) 
{ 
    struct node* new_node = (struct node*)malloc(sizeof(struct node));
    new_node->next = NULL;
    new_node->thread = t;

    struct node *temp;

    if (*head_ref == NULL || (*head_ref)->thread->priority > new_node->thread->priority) 
    { 
      new_node->next = (*head_ref); 
      (*head_ref) = new_node; 
    } 
    else
    { 
      temp = (*head_ref); 
      while (temp->next != NULL && temp->thread->priority <= new_node->thread->priority) 
      { 
        temp = temp->next; 
      } 
      new_node->next = temp->next; 
      temp->next = new_node; 
    } 
} 

//borrowed from https://www.geeksforgeeks.org/linked-list-set-2-inserting-a-node/ 
//then altered for use in this assignment 
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
  new_node->waittime = 0;
  new_node->turnaround = 0;

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

//borrowed from https://www.geeksforgeeks.org/implement-a-stack-using-singly-linked-list/
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



