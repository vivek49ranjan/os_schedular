#include <stdio.h>

/* Full Process Control Block (required for accessing members) */
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

/* External queue functions */
Process* dequeue(ReadyQueue *q);
int is_empty(ReadyQueue *q);

/* FCFS Scheduler with table printing */
void fcfs_schedule(ReadyQueue *rq, Process **all_processes, int n) {
    int current_time = 0;

    printf("\nGANTT CHART\n|");

    while (!is_empty(rq)) {
        Process *p = dequeue(rq);

        if (current_time < p->arrival_time)
            current_time = p->arrival_time;

        p->start_time = current_time;
        p->response_time = p->start_time - p->arrival_time;

        printf(" P%d |", p->pid);

        current_time += p->burst_time;
        p->completion_time = current_time;

        p->turnaround_time = p->completion_time - p->arrival_time;
        p->waiting_time = p->turnaround_time - p->burst_time;
    }

    printf("\n");

    /* Print table of metrics */
    printf("\nPID\tArrival\tBurst\tStart\tCompletion\tTurnaround\tWaiting\tResponse\n");
    for (int i = 0; i < n; i++) {
        Process *p = all_processes[i];
        printf("%d\t%d\t%d\t%d\t%d\t\t%d\t\t%d\t%d\n",
            p->pid,
            p->arrival_time,
            p->burst_time,
            p->start_time,
            p->completion_time,
            p->turnaround_time,
            p->waiting_time,
            p->response_time
        );
    }
}
