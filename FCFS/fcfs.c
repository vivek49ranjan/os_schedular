#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESSES 100

/* ---------------- PROCESS STRUCT ---------------- */
typedef struct process {
    int pid;
    int arrival_time;
    int burst_time;
    int remaining_time;

    /* I/O related */
    int io_start;        // execution point where I/O starts
    int io_duration;     // how long I/O takes
    int io_remaining;
    int in_io;

    /* Metrics */
    int start_time;
    int completion_time;
    int waiting_time;
    int turnaround_time;
    int response_time;
    int has_started;
} Process;

/* ---------------- QUEUE STRUCT ---------------- */
typedef struct node {
    Process *p;
    struct node *next;
} Node;

typedef struct {
    Node *front;
    Node *rear;
} Queue;

/* ---------------- QUEUE OPERATIONS ---------------- */
void init_queue(Queue *q) {
    q->front = q->rear = NULL;
}

int is_empty(Queue *q) {
    return q->front == NULL;
}

void enqueue(Queue *q, Process *p) {
    Node *n = (Node *)malloc(sizeof(Node));
    n->p = p;
    n->next = NULL;

    if (q->rear == NULL) {
        q->front = q->rear = n;
    } else {
        q->rear->next = n;
        q->rear = n;
    }
}

Process* dequeue(Queue *q) {
    if (is_empty(q))
        return NULL;

    Node *temp = q->front;
    Process *p = temp->p;
    q->front = temp->next;

    if (q->front == NULL)
        q->rear = NULL;

    free(temp);
    return p;
}

/* ---------------- PROCESS INITIALIZER ---------------- */
void init_process(Process *p,
                  int pid,
                  int arrival,
                  int burst,
                  int io_start,
                  int io_duration) {

    p->pid = pid;
    p->arrival_time = arrival;
    p->burst_time = burst;
    p->remaining_time = burst;

    p->io_start = io_start;
    p->io_duration = io_duration;
    p->io_remaining = io_duration;
    p->in_io = 0;

    p->start_time = -1;
    p->completion_time = 0;
    p->waiting_time = 0;
    p->turnaround_time = 0;
    p->response_time = 0;
    p->has_started = 0;
}

/* ---------------- I/O AWARE FCFS SCHEDULER ---------------- */
void fcfs_io_scheduler(Process processes[], int n) {

    Queue ready_q, io_q;
    init_queue(&ready_q);
    init_queue(&io_q);

    int time = 0;
    int completed = 0;
    Process *current = NULL;

    printf("\n--- I/O Aware FCFS Scheduling ---\n");

    while (completed < n) {

        /* Check arrivals */
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival_time == time) {
                enqueue(&ready_q, &processes[i]);
                printf("[Time %d] P%d arrived\n", time, processes[i].pid);
            }
        }

        /* Handle I/O completion */
        Node *prev = NULL;
        Node *curr = io_q.front;

        while (curr) {
            curr->p->io_remaining--;

            if (curr->p->io_remaining == 0) {
                curr->p->in_io = 0;
                enqueue(&ready_q, curr->p);
                printf("[Time %d] P%d finished I/O\n", time, curr->p->pid);

                if (prev)
                    prev->next = curr->next;
                else
                    io_q.front = curr->next;

                Node *temp = curr;
                curr = curr->next;
                free(temp);
            } else {
                prev = curr;
                curr = curr->next;
            }
        }

        /* Assign CPU */
        if (current == NULL && !is_empty(&ready_q)) {
            current = dequeue(&ready_q);

            if (!current->has_started) {
                current->start_time = time;
                current->response_time = time - current->arrival_time;
                current->has_started = 1;
            }

            printf("[Time %d] CPU -> P%d\n", time, current->pid);
        }

        /* Execute */
        if (current) {
            current->remaining_time--;

            int executed =
                current->burst_time - current->remaining_time;

            /* Trigger I/O */
            if (executed == current->io_start &&
                current->io_duration > 0) {

                current->in_io = 1;
                enqueue(&io_q, current);
                printf("[Time %d] P%d blocked for I/O\n", time, current->pid);
                current = NULL;
            }
            /* Completion */
            else if (current->remaining_time == 0) {

                current->completion_time = time + 1;
                current->turnaround_time =
                    current->completion_time - current->arrival_time;
                current->waiting_time =
                    current->turnaround_time - current->burst_time;

                printf("[Time %d] P%d completed\n",
                       time + 1, current->pid);

                completed++;
                current = NULL;
            }
        }

        time++;
    }

    /* Final Table */
    printf("\nPID AT BT CT WT TAT RT\n");
    for (int i = 0; i < n; i++) {
        printf("%d  %d  %d  %d  %d  %d  %d\n",
               processes[i].pid,
               processes[i].arrival_time,
               processes[i].burst_time,
               processes[i].completion_time,
               processes[i].waiting_time,
               processes[i].turnaround_time,
               processes[i].response_time);
    }
}
