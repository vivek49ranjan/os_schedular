#include <stdio.h>
#include <stdlib.h>

/* ================= PROCESS CONTROL BLOCK ================= */
typedef struct process {
    int pid;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int executed_time;
    int start_time;
    int completion_time;

    int io_start;        /* when I/O starts */
    int io_duration;     /* fixed I/O time */
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

/* ================= SORT BY ARRIVAL ================= */
void sort_by_arrival(Process *p, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (p[i].arrival_time > p[j].arrival_time) {
                Process temp = p[i];
                p[i] = p[j];
                p[j] = temp;
            }
        }
    }
}


/*=============== FCFS I/O-AWARE SCHEDULER ================*/

void fcfs_io_aware(
    int pid[],
    int arrival_time[],
    int burst_time[],
    int n
) {
    Process *ptable = (Process *)malloc(sizeof(Process) * n);

    for (int i = 0; i < n; i++) {
        ptable[i].pid = pid[i];
        ptable[i].arrival_time = arrival_time[i];
        ptable[i].burst_time = burst_time[i];

        ptable[i].remaining_time = burst_time[i];
        ptable[i].executed_time = 0;
        ptable[i].start_time = -1;
        ptable[i].completion_time = 0;

        ptable[i].io_start = burst_time[i] / 2;
        ptable[i].io_duration = 2;
        ptable[i].io_remaining = 0;
        ptable[i].in_io = 0;

        ptable[i].next = NULL;
    }

    sort_by_arrival(ptable, n);

    Queue ready_q, io_q;
    init_queue(&ready_q);
    init_queue(&io_q);

    int time = 0;
    int completed = 0;
    int next_arrival = 0;
    Process *current = NULL;

    printf("\nGANTT CHART:\n");

    while (completed < n) {

        /* ---- Handle arrivals ---- */
        while (next_arrival < n &&
               ptable[next_arrival].arrival_time == time) {
            enqueue(&ready_q, &ptable[next_arrival]);
            next_arrival++;
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

        /* ---- CPU selection ---- */
        if (!current && !is_empty(&ready_q)) {
            current = dequeue(&ready_q);
            if (current->start_time == -1)
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
                completed++;
                current = NULL;
            }
        } else {
            printf("| IDLE ");
        }

        time++;
    }

    printf("|\n");

    /* ================= OUTPUT TABLE ================= */
    printf("\nPID\tAT\tBT\tCT\tTAT\tWT\tRT\n");

    for (int i = 0; i < n; i++) {
        int tat = ptable[i].completion_time - ptable[i].arrival_time;
        int wt  = tat - ptable[i].burst_time;
        int rt  = ptable[i].start_time - ptable[i].arrival_time;

        printf("P%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
               ptable[i].pid,
               ptable[i].arrival_time,
               ptable[i].burst_time,
               ptable[i].completion_time,
               tat,
               wt,
               rt);
    }

    free(ptable);
}
