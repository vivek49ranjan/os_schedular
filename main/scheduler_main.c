#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define RED 0
#define BLACK 1
#define SCHED_PERIOD 10.0
#define BASE_WEIGHT 1024.0
#define MAX_HISTORY 5000
#define MAX_PROC 100
#define AGING_FACTOR 1

typedef enum { NEW, READY, RUNNING, FINISHED } state_t;

typedef struct process {
    int pid;
    int arrival_time;
    int burst_time;
    double remaining_time;
    double vruntime;
    double weight;
    double start_time;        
    double completion_time;  
    state_t state;
} process;

typedef struct rb_node {
    double vruntime;
    process *task;
    struct rb_node *left, *right, *parent;
    int color;
} rb_node;

typedef struct {
    int pid;
    double duration;
} gantt_step;

typedef struct ProcessIO {
    int pid;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int executed_time;
    int start_time;
    int completion_time;
    int io_start;
    int io_duration;
    int io_remaining;
    int in_io;
    struct ProcessIO *next;
} ProcessIO;

typedef struct {
    ProcessIO *front;
    ProcessIO *rear;
} Queue;

typedef struct {
    int pid;
    int arrival;
    int burst;
    int start;
    int completion;
    int waiting;
    int turnaround;
    int response;
    int age;
} ProcessSJF;

typedef struct {
    ProcessSJF data[MAX_PROC];
    int size;
} ReadyQueue;

typedef struct {
    int pid;
    int start;
    int end;
} Gantt;

rb_node *runqueue = NULL;
int active_tasks = 0;
gantt_step history[MAX_HISTORY];
int history_count = 0;
process *finished[MAX_PROC];
int finished_count = 0;

void init_queue(Queue *q) { q->front = q->rear = NULL; }
int is_empty(Queue *q) { return q->front == NULL; }

void enqueue(Queue *q, ProcessIO *p) {
    p->next = NULL;
    if (!q->rear) {
        q->front = q->rear = p;
        return;
    }
    q->rear->next = p;
    q->rear = p;
}

ProcessIO* dequeue(Queue *q) {
    if (!q->front) return NULL;
    ProcessIO *p = q->front;
    q->front = p->next;
    if (!q->front) q->rear = NULL;
    p->next = NULL;
    return p;
}

void insert_by_arrival(Queue *q, ProcessIO *p) {
    if (!q->front || p->arrival_time < q->front->arrival_time) {
        p->next = q->front;
        q->front = p;
        if (!q->rear) q->rear = p;
        return;
    }
    ProcessIO *cur = q->front;
    while (cur->next && cur->next->arrival_time <= p->arrival_time)
        cur = cur->next;
    p->next = cur->next;
    cur->next = p;
    if (!p->next) q->rear = p;
}

void fcfs_io_aware(Queue *arrival_q) {
    Queue ready_q, io_q;
    init_queue(&ready_q);
    init_queue(&io_q);
    int time = 0;
    ProcessIO *running = NULL;
    printf("\nPID | AT | BT | ST | CT | TAT | WT\n");
    printf("---------------------------------\n");
    while (!is_empty(arrival_q) || !is_empty(&ready_q) || !is_empty(&io_q) || running) {
        while (!is_empty(arrival_q) && arrival_q->front->arrival_time <= time) {
            enqueue(&ready_q, dequeue(arrival_q));
        }
        int io_count = 0;
        for (ProcessIO *p = io_q.front; p; p = p->next) io_count++;
        while (io_count--) {
            ProcessIO *p = dequeue(&io_q);
            p->io_remaining--;
            if (p->io_remaining == 0) {
                p->in_io = 0;
                enqueue(&ready_q, p);
            } else {
                enqueue(&io_q, p);
            }
        }
        if (!running && !is_empty(&ready_q)) {
            running = dequeue(&ready_q);
            if (running->start_time == -1) running->start_time = time;
        }
        if (running) {
            running->remaining_time--;
            running->executed_time++;
            if (running->executed_time == running->io_start && running->io_duration > 0) {
                running->io_remaining = running->io_duration;
                running->in_io = 1;
                enqueue(&io_q, running);
                running = NULL;
            } else if (running->remaining_time == 0) {
                running->completion_time = time + 1;
                int tat = running->completion_time - running->arrival_time;
                int wt  = tat - running->burst_time;
                printf("%3d | %2d | %2d | %2d | %2d | %3d | %3d\n",
                       running->pid, running->arrival_time, running->burst_time,
                       running->start_time, running->completion_time, tat, wt);
                free(running);
                running = NULL;
            }
        }
        time++;
    }
}

void left_rotate(rb_node **root, rb_node *x) {
    rb_node *y = x->right;
    if (!y) return;
    x->right = y->left;
    if (y->left) y->left->parent = x;
    y->parent = x->parent;
    if (!x->parent) *root = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    y->left = x;
    x->parent = y;
}

void right_rotate(rb_node **root, rb_node *y) {
    rb_node *x = y->left;
    if (!x) return;
    y->left = x->right;
    if (x->right) x->right->parent = y;
    x->parent = y->parent;
    if (!y->parent) *root = x;
    else if (y == y->parent->left) y->parent->left = x;
    else y->parent->right = x;
    x->right = y;
    y->parent = x;
}

void fix_insert(rb_node **root, rb_node *z) {
    while (z != *root && z->parent && z->parent->color == RED) {
        rb_node *gp = z->parent->parent;
        if (!gp) break;
        if (z->parent == gp->left) {
            rb_node *u = gp->right;
            if (u && u->color == RED) {
                z->parent->color = BLACK;
                u->color = BLACK;
                gp->color = RED;
                z = gp;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    left_rotate(root, z);
                }
                z->parent->color = BLACK;
                gp->color = RED;
                right_rotate(root, gp);
            }
        } else {
            rb_node *u = gp->left;
            if (u && u->color == RED) {
                z->parent->color = BLACK;
                u->color = BLACK;
                gp->color = RED;
                z = gp;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    right_rotate(root, z);
                }
                z->parent->color = BLACK;
                gp->color = RED;
                left_rotate(root, gp);
            }
        }
    }
    if (*root) (*root)->color = BLACK;
}

void rb_insert(rb_node **root, process *p) {
    rb_node *z = malloc(sizeof(rb_node));
    z->vruntime = p->vruntime;
    z->task = p;
    z->left = z->right = z->parent = NULL;
    z->color = RED;
    rb_node *y = NULL;
    rb_node *x = *root;
    while (x) {
        y = x;
        if (z->vruntime < x->vruntime) x = x->left;
        else x = x->right;
    }
    z->parent = y;
    if (!y) *root = z;
    else if (z->vruntime < y->vruntime) y->left = z;
    else y->right = z;
    fix_insert(root, z);
}

rb_node* pick_min(rb_node *root) {
    while (root && root->left) root = root->left;
    return root;
}

rb_node* rb_extract_min(rb_node **root) {
    rb_node *m = pick_min(*root);
    if (!m) return NULL;
    rb_node *child = m->right;
    rb_node *parent = m->parent;
    if (parent) parent->left = child;
    else *root = child;
    if (child) child->parent = parent;
    m->left = m->right = m->parent = NULL;
    return m;
}

double calc_slice(double weight, int nr) {
    return (nr > 0) ? (SCHED_PERIOD * weight) / (BASE_WEIGHT * nr) : SCHED_PERIOD;
}

void update_vruntime(process *p, double runtime) {
    p->vruntime += runtime * (BASE_WEIGHT / p->weight);
}

void print_gantt_cfs() {
    int max = history_count < 40 ? history_count : 40;
    printf("\n--- CFS GANTT CHART (First %d slices) ---\n\n", max);
    for (int i = 0; i < max; i++) printf("+------");
    printf("+\n");
    for (int i = 0; i < max; i++) printf("| P%-3d", history[i].pid);
    printf("|\n");
    for (int i = 0; i < max; i++) printf("+------");
    printf("+\n");
    double t = 0.0;
    printf("%-6.1f", t);
    for (int i = 0; i < max; i++) {
        t += history[i].duration;
        printf("%-6.1f", t);
    }
    printf("\n");
}

void print_metrics_cfs() {
    double avg_wt = 0, avg_tat = 0, avg_rt = 0;
    printf("\n             CFS PROCESS METRICS                 \n");
    printf("PID | AT | BT | CT  | TAT | WT  | RT\n");
    for (int i = 0; i < finished_count; i++) {
        process *p = finished[i];
        double tat = p->completion_time - p->arrival_time;
        double wt  = tat - p->burst_time;
        double rt  = p->start_time - p->arrival_time;
        avg_tat += tat; avg_wt += wt; avg_rt += rt;
        printf("%3d | %2d | %2d | %4.1f | %4.1f | %4.1f | %4.1f\n",
               p->pid, p->arrival_time, p->burst_time, p->completion_time, tat, wt, rt);
    }
    printf("-----------------------------------------------\n");
    printf("AVG |    |    |      | %4.2f | %4.2f | %4.2f\n",
           avg_tat / finished_count, avg_wt / finished_count, avg_rt / finished_count);
}

void cfs_schedule() {
    double time = 0.0;
    while (runqueue && active_tasks > 0) {
        rb_node *node = rb_extract_min(&runqueue);
        process *p = node->task;
        if (p->remaining_time == p->burst_time) p->start_time = time;
        double slice = calc_slice(p->weight, active_tasks);
        double run = (p->remaining_time < slice) ? p->remaining_time : slice;
        history[history_count++] = (gantt_step){p->pid, run};
        time += run;
        p->remaining_time -= run;
        update_vruntime(p, run);
        if (p->remaining_time > 0.001) {
            rb_insert(&runqueue, p);
        } else {
            active_tasks--;
            p->completion_time = time;
            finished[finished_count++] = p;
        }
        free(node);
    }
    print_gantt_cfs();
    print_metrics_cfs();
}

void initProcess(ProcessSJF *p, int id, int at, int bt) {
    p->pid = id; p->arrival = at; p->burst = bt; p->start = -1;
    p->completion = 0; p->waiting = 0; p->turnaround = 0; p->response = 0; p->age = 0;
}

int sortByArrival(const void *a, const void *b) {
    ProcessSJF *p1 = (ProcessSJF *)a;
    ProcessSJF *p2 = (ProcessSJF *)b;
    if (p1->arrival != p2->arrival) return p1->arrival - p2->arrival;
    return p1->pid - p2->pid;
}

int hasHigherPriority(ProcessSJF a, ProcessSJF b) {
    int effectiveA = a.burst - a.age * AGING_FACTOR;
    int effectiveB = b.burst - b.age * AGING_FACTOR;
    if (effectiveA != effectiveB) return effectiveA < effectiveB;
    if (a.arrival != b.arrival) return a.arrival < b.arrival;
    return a.pid < b.pid;
}

void swap(ProcessSJF *a, ProcessSJF *b) { ProcessSJF temp = *a; *a = *b; *b = temp; }

void heapifyUp(ReadyQueue *rq, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (hasHigherPriority(rq->data[index], rq->data[parent])) {
            swap(&rq->data[index], &rq->data[parent]);
            index = parent;
        } else break;
    }
}

void heapifyDown(ReadyQueue *rq, int index) {
    while (1) {
        int best = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        if (left < rq->size && hasHigherPriority(rq->data[left], rq->data[best])) best = left;
        if (right < rq->size && hasHigherPriority(rq->data[right], rq->data[best])) best = right;
        if (best != index) {
            swap(&rq->data[index], &rq->data[best]);
            index = best;
        } else break;
    }
}

void pushProcess(ReadyQueue *rq, ProcessSJF p) {
    rq->data[rq->size] = p;
    heapifyUp(rq, rq->size);
    rq->size++;
}

ProcessSJF popProcess(ReadyQueue *rq) {
    ProcessSJF top = rq->data[0];
    rq->data[0] = rq->data[rq->size - 1];
    rq->size--;
    heapifyDown(rq, 0);
    return top;
}

void increaseAge(ReadyQueue *rq) {
    for (int i = 0; i < rq->size; i++) rq->data[i].age++;
}

void printGanttSJF(Gantt chart[], int count) {
    printf("\nSJF Gantt Chart\n ");
    for (int i = 0; i < count; i++) printf("-------");
    printf("\n|");
    for (int i = 0; i < count; i++) {
        if (chart[i].pid == -1) printf(" IDLE |");
        else printf("  P%d  |", chart[i].pid);
    }
    printf("\n ");
    for (int i = 0; i < count; i++) printf("-------");
    printf("\n%d", chart[0].start);
    for (int i = 0; i < count; i++) printf("%7d", chart[i].end);
    printf("\n\n");
}

void scheduleSJF(ProcessSJF processes[], int n) {
    qsort(processes, n, sizeof(ProcessSJF), sortByArrival);
    ReadyQueue rq; rq.size = 0;
    Gantt chart[MAX_PROC * 2];
    int chartSize = 0, time = 0, index = 0, done = 0;
    float avgWait = 0, avgTurn = 0, avgResp = 0;
    while (done < n) {
        while (index < n && processes[index].arrival <= time)
            pushProcess(&rq, processes[index++]);
        if (rq.size == 0) {
            int nextTime = processes[index].arrival;
            chart[chartSize++] = (Gantt){-1, time, nextTime};
            time = nextTime;
            continue;
        }
        increaseAge(&rq);
        ProcessSJF current = popProcess(&rq);
        current.start = time;
        current.response = time - current.arrival;
        current.waiting = current.response;
        chart[chartSize++] = (Gantt){current.pid, time, time + current.burst};
        time += current.burst;
        current.completion = time;
        current.turnaround = current.completion - current.arrival;
        avgWait += current.waiting; avgTurn += current.turnaround; avgResp += current.response;
        done++;
    }
    printGanttSJF(chart, chartSize);
    printf("Average Waiting Time    : %.2f\n", avgWait / n);
    printf("Average Turnaround Time : %.2f\n", avgTurn / n);
    printf("Average Response Time   : %.2f\n", avgResp / n);
}


}

int main() {
    FILE *fp = fopen("processes.txt", "r");
    if (!fp) { perror("File open failed"); return 1; }
    
    int pid, at, bt;
    Queue fcfs_arrival_q;
    init_queue(&fcfs_arrival_q);
    
    ProcessSJF sjf_procs[MAX_PROC];
    int sjf_count = 0;

    char pid_rr_temp[MAX_PROC][10];
    int at_rr_temp[MAX_PROC];
    int bt_rr_temp[MAX_PROC];
    int n = 0;

    while (fscanf(fp, "%d %d %d", &pid, &at, &bt) == 3 && n < MAX_PROC) {
        process *p_node = malloc(sizeof(process));
        *p_node = (process){pid, at, bt, bt, 0.0, BASE_WEIGHT, 0, 0, READY};
        rb_insert(&runqueue, p_node);
        active_tasks++;

        ProcessIO *fp_proc = malloc(sizeof(ProcessIO));
        fp_proc->pid = pid; fp_proc->arrival_time = at; fp_proc->burst_time = bt;
        fp_proc->remaining_time = bt; fp_proc->executed_time = 0; fp_proc->start_time = -1;
        fp_proc->completion_time = 0; fp_proc->io_start = bt / 2; fp_proc->io_duration = 2;
        fp_proc->io_remaining = 0; fp_proc->in_io = 0; fp_proc->next = NULL;
        insert_by_arrival(&fcfs_arrival_q, fp_proc);

        initProcess(&sjf_procs[sjf_count++], pid, at, bt);

        sprintf(pid_rr_temp[n], "P%d", pid);
        at_rr_temp[n] = at;
        bt_rr_temp[n] = bt;
        n++;
    }
    fclose(fp);

    struct process_rr p_rr[n];
    for(int i=0; i<n; i++){
        strcpy(p_rr[i].Pid, pid_rr_temp[i]);
        p_rr[i].Arrival_Time = at_rr_temp[i];
        p_rr[i].Burst_Time = bt_rr_temp[i];
    }

    struct node *front_rr=NULL, *rear_rr=NULL;
    int rem_bt[n];
    char order[MAX_HISTORY][10];
    int time_arr[MAX_HISTORY];
    int c1=0, c2=0;

    printf("\n--- Starting Round Robin Scheduler ---");
    round_robin(n, p_rr, &front_rr, &rear_rr, rem_bt, order, time_arr, &c1, &c2);
    print_gantt_rr(c1, c2, order, time_arr);
    calculate_metrics_rr(n, p_rr, order, time_arr, c1);

    printf("\n--- Starting FCFS Scheduler ---\n");
    fcfs_io_aware(&fcfs_arrival_q);

    printf("\n--- Starting CFS Scheduler ---\n");
    cfs_schedule();

    printf("\n--- Starting SJF Scheduler ---\n");
    scheduleSJF(sjf_procs, sjf_count);

    return 0;
}
