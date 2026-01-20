#include <stdio.h>
#include <stdlib.h>

/* ================= PROCESS CONTROL BLOCK ================= */
typedef struct process {
    int pid;
    int arrival_time;
    int burst_time;

    /* Runtime fields */
    int remaining_time;
    int executed_time;
    int start_time;
    int completion_time;

    /* I/O (heuristic) */
    int io_start;
    int io_duration;
    int io_remaining;
    int in_io;

    struct process *next;
} Process;

/* ================= QUEUE ================= */
typedef struct {
    Process *front;
    Process *rear;
} Queue;

/* ================= QUEUE OPERATIONS ================= */
void init_queue(Queue *q) {
    q->front = q->rear = NULL;
}

int is_empty(Queue *q) {
    return q->front == NULL;
}

void enqueue(Queue *q, Process *p) {
    p->next = NULL;
    if (!q->rear) {
        q->front = q->rear = p;
    } else {
        q->rear->next = p;
        q->rear = p;
    }
}

Process *dequeue(Queue *q) {
    if (is_empty(q)) return NULL;
    Process *p = q->front;
    q->front = p->next;
    if (!q->front) q->rear = NULL;
    return p;
}

/* ========== ARRIVAL-TIME SORTED INSERTION ========== */
void insert_by_arrival(Queue *arrival_q, Process *p) {

    if (is_empty(arrival_q) ||
        p->arrival_time < arrival_q->front->arrival_time) {

        p->next = arrival_q->front;
        arrival_q->front = p;
        if (!arrival_q->rear)
            arrival_q->rear = p;
        return;
    }

    Process *cur = arrival_q->front;
    while (cur->next &&
           cur->next->arrival_time <= p->arrival_time) {
        cur = cur->next;
    }

    p->next = cur->next;
    cur->next = p;
    if (!p->next)
        arrival_q->rear = p;
}

/* ======================================================
   FCFS I/O-AWARE STREAMING SCHEDULER
   INPUT  : arrival queue (sorted)
   OUTPUT : Gantt chart + table
   ====================================================== */
void fcfs_io_aware(Queue *arrival_q) {

    static int time = 0;          /* persistent clock */
    static int header_printed = 0;

    static Queue ready_q, io_q;
    static Process *current = NULL;

    static int initialized = 0;
    if (!initialized) {
        init_queue(&ready_q);
        init_queue(&io_q);
        initialized = 1;
    }

    if (!header_printed) {
        printf("\nGANTT CHART:\n");
        header_printed = 1;
    }

    /* ---- Admit arrivals ---- */
    while (!is_empty(arrival_q) &&
           arrival_q->front->arrival_time <= time) {

        enqueue(&ready_q, dequeue(arrival_q));
    }

    /* ---- Handle I/O completion ---- */
    Process *prev = NULL;
    Process *io_p = io_q.front;

    while (io_p) {
        io_p->io_remaining--;

        if (io_p->io_remaining == 0) {
            io_p->in_io = 0;

            if (!prev)
                io_q.front = io_p->next;
            else
                prev->next = io_p->next;

            if (io_p == io_q.rear)
                io_q.rear = prev;

            Process *done = io_p;
            io_p = io_p->next;
            enqueue(&ready_q, done);
        } else {
            prev = io_p;
            io_p = io_p->next;
        }
    }

    /* ---- Select process ---- */
    if (!current && !is_empty(&ready_q)) {
        current = dequeue(&ready_q);
        if (current->executed_time == 0)
            current->start_time = time;
    }

    /* ---- Execute CPU ---- */
    if (current) {
        printf("| P%d ", current->pid);

        current->remaining_time--;
        current->executed_time++;

        /* I/O block */
        if (current->executed_time == current->io_start &&
            current->remaining_time > 0) {

            current->in_io = 1;
            current->io_remaining = current->io_duration;
            enqueue(&io_q, current);
            current = NULL;
        }
        /* Completion */
        else if (current->remaining_time == 0) {
            current->completion_time = time + 1;

            int tat = current->completion_time - current->arrival_time;
            int wt  = tat - current->burst_time;
            int rt  = current->start_time - current->arrival_time;

            printf("\nP%d  AT=%d  BT=%d  CT=%d  TAT=%d  WT=%d  RT=%d\n",
                   current->pid,
                   current->arrival_time,
                   current->burst_time,
                   current->completion_time,
                   tat, wt, rt);

            free(current);
            current = NULL;
        }
    } else {
        printf("| IDLE ");
    }

    time++;
}
