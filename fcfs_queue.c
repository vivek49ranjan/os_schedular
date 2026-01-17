#include <stdio.h>
#include <stdlib.h>

/* Process Control Block */
struct process {
    int pid;
    int arrival_time;
    int burst_time;

    int start_time;
    int completion_time;
    int waiting_time;
    int turnaround_time;
    int response_time;

    struct process *next;
};

typedef struct process Process;

/* Ready Queue */
typedef struct {
    Process *front;
    Process *rear;
} ReadyQueue;

/* Initialize queue */
void init_queue(ReadyQueue *q) {
    q->front = q->rear = NULL;
}

/* Enqueue process */
void enqueue(ReadyQueue *q, Process *p) {
    p->next = NULL;

    if (q->rear == NULL) {
        q->front = q->rear = p;
        return;
    }

    q->rear->next = p;
    q->rear = p;
}

/* Dequeue process */
Process* dequeue(ReadyQueue *q) {
    if (q->front == NULL)
        return NULL;

    Process *temp = q->front;
    q->front = q->front->next;

    if (q->front == NULL)
        q->rear = NULL;

    return temp;
}

/* Check empty */
int is_empty(ReadyQueue *q) {
    return q->front == NULL;
}
