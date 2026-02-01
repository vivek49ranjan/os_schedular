#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_PROC 100
#define NUM_CORES 4
#define MAX_HISTORY 5000
#define BASE_WEIGHT 1024.0
#define SCHED_PERIOD 10.0
#define AGING_FACTOR 1
#define RED 0
#define BLACK 1


#include "../RR/rr_scheduler"
#include "../FCFS/fcfs_scheduler"
#include "../CFS/cfs_scheduler.c"
#include "../SJF/sjf_scheduler"

int main() {
    FILE *fp = fopen("processes.txt", "r");
    if (!fp) return 1;
    int pid;
    int at;
    int bt;
    int n = 0;
    Queue fcfs_arrival_q;
    init_queue(&fcfs_arrival_q);
    ProcessSJF sjf_procs[MAX_PROC];
    struct process_rr rr_procs[MAX_PROC];

    while (fscanf(fp, "%d %d %d", &pid, &at, &bt) == 3 && n < MAX_PROC) {
        int core_idx = pid % NUM_CORES;
        process *p_node = malloc(sizeof(process));
        *p_node = (process){pid, at, bt, (double)bt, 0.0, BASE_WEIGHT, 0, 0, READY};
        rb_insert(&runqueues[core_idx], p_node);
        active_tasks_per_core[core_idx]++;
        ProcessIO *fp_proc = malloc(sizeof(ProcessIO));
        *fp_proc = (ProcessIO){pid, at, bt, bt, 0, -1, 0, bt / 2, 2, 0, 0, NULL};
        enqueue_fcfs(&fcfs_arrival_q, fp_proc);
        sjf_procs[n] = (ProcessSJF){pid, at, bt, -1, 0, 0, 0, 0};
        sprintf(rr_procs[n].Pid, "P%d", pid);
        rr_procs[n].Arrival_Time = at;
        rr_procs[n].Burst_Time = bt;
        n++;
    }
    fclose(fp);

    printf("\n--- Starting Round Robin (4 Cores) ---");
    round_robin(n, rr_procs);
    printf("\n--- Starting FCFS IO-Aware (4 Cores) ---");
    fcfs_io_aware(&fcfs_arrival_q);
    printf("\n--- Starting CFS (4 Cores) ---");
    cfs_schedule();
    printf("\n--- Starting SJF (4 Cores) ---");
    scheduleSJF(sjf_procs, n);

    return 0;
}
