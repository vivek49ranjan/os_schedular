#include <stdio.h>
#include <stdlib.h>

#define RED 0
#define BLACK 1
#define SCHED_PERIOD 10.0
#define BASE_WEIGHT 1024.0
#define MAX_HISTORY 5000
#define MAX_PROC 100

typedef enum {
    NEW,
    READY,
    RUNNING,
    FINISHED
} state_t;

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

typedef struct process_fcfs {
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

    struct process_fcfs *next;
} Process;

typedef struct {
    Process *front;
    Process *rear;
} Queue;

rb_node *runqueue = NULL;
int active_tasks = 0;

gantt_step history[MAX_HISTORY];
int history_count = 0;

process *finished[MAX_PROC];
int finished_count = 0;

Queue fcfs_arrival_q;

void init_queue(Queue *q) {
    q->front = q->rear = NULL;
}

void enqueue(Queue *q, Process *p) {
    p->next = NULL;
    if (!q->rear) {
        q->front = q->rear = p;
        return;
    }
    q->rear->next = p;
    q->rear = p;
}

Process* dequeue(Queue *q) {
    if (!q->front) return NULL;
    Process *p = q->front;
    q->front = p->next;
    if (!q->front) q->rear = NULL;
    p->next = NULL;
    return p;
}

void insert_by_arrival(Queue *q, Process *p) {
    if (!q->front || p->arrival_time < q->front->arrival_time) {
        p->next = q->front;
        q->front = p;
        if (!q->rear) q->rear = p;
        return;
    }

    Process *cur = q->front;
    while (cur->next && cur->next->arrival_time <= p->arrival_time)
        cur = cur->next;

    p->next = cur->next;
    cur->next = p;
    if (!p->next) q->rear = p;
}

void fcfs_io_aware(Queue *arrival_q) {
    Queue ready_q = {0};
    Queue io_q = {0};

    int clock = 0;

    printf("\nPID | AT | BT | ST | CT | TAT | WT\n");
    printf("---------------------------------\n");

    while (arrival_q->front || ready_q.front || io_q.front) {
        while (arrival_q->front && arrival_q->front->arrival_time <= clock)
            enqueue(&ready_q, dequeue(arrival_q));

        Process *prev = NULL, *cur = io_q.front;
        while (cur) {
            cur->io_remaining--;
            if (cur->io_remaining <= 0) {
                Process *done = cur;
                if (prev) prev->next = cur->next;
                else io_q.front = cur->next;
                cur = cur->next;
                done->in_io = 0;
                enqueue(&ready_q, done);
            } else {
                prev = cur;
                cur = cur->next;
            }
        }

        if (!ready_q.front) {
            clock++;
            continue;
        }

        Process *p = dequeue(&ready_q);

        if (p->start_time == -1)
            p->start_time = clock;

        p->remaining_time--;
        clock++;

        if (!p->in_io && p->remaining_time == p->burst_time - p->io_start) {
            p->in_io = 1;
            p->io_remaining = p->io_duration;
            enqueue(&io_q, p);
            continue;
        }

        if (p->remaining_time == 0) {
            p->completion_time = clock;
            int tat = p->completion_time - p->arrival_time;
            int wt = tat - p->burst_time;

            printf("%3d | %2d | %2d | %2d | %2d | %3d | %2d\n",
                   p->pid, p->arrival_time, p->burst_time,
                   p->start_time, p->completion_time, tat, wt);
            free(p);
        } else {
            enqueue(&ready_q, p);
        }
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

void print_gantt() {
    int max = history_count < 40 ? history_count : 40;

    printf("\n--- GANTT CHART (First %d slices) ---\n\n", max);

    for (int i = 0; i < max; i++) printf("+------");
    printf("+\n");

    for (int i = 0; i < max; i++)
        printf("| P%-3d", history[i].pid);
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

void print_metrics() {
    double avg_wt = 0, avg_tat = 0, avg_rt = 0;

    printf("\n             PROCESS METRICS                 \n");
    printf("PID | AT | BT | CT  | TAT | WT  | RT\n");
    printf("                                               \n");

    for (int i = 0; i < finished_count; i++) {
        process *p = finished[i];

        double tat = p->completion_time - p->arrival_time;
        double wt  = tat - p->burst_time;
        double rt  = p->start_time - p->arrival_time;

        avg_tat += tat;
        avg_wt  += wt;
        avg_rt  += rt;

        printf("%3d | %2d | %2d | %4.1f | %4.1f | %4.1f | %4.1f\n",
               p->pid, p->arrival_time, p->burst_time,
               p->completion_time, tat, wt, rt);
    }

    int n = finished_count;
    printf("-----------------------------------------------\n");
    printf("AVG |    |    |      | %4.2f | %4.2f | %4.2f\n",
           avg_tat / n, avg_wt / n, avg_rt / n);
}

void cfs_schedule() {
    double time = 0.0;

    while (runqueue && active_tasks > 0) {
        rb_node *node = rb_extract_min(&runqueue);
        process *p = node->task;

        if (p->remaining_time == p->burst_time)
            p->start_time = time;

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

    print_gantt();
    print_metrics();
}

int main() {
    FILE *fp = fopen("processes.txt", "r");
    if (!fp) {
        perror("File open failed");
        return 1;
    }

    int pid, at, bt;

    init_queue(&fcfs_arrival_q);

    printf("--- Admitting Processes ---\n");

    while (fscanf(fp, "%d %d %d", &pid, &at, &bt) == 3) {
        process *p = malloc(sizeof(process));
        *p = (process){pid, at, bt, bt, 0.0, BASE_WEIGHT, 0, 0, READY};
        rb_insert(&runqueue, p);
        active_tasks++;
        printf("Admitted PID %d\n", pid);

        Process *fp_proc = malloc(sizeof(Process));
        fp_proc->pid = pid;
        fp_proc->arrival_time = at;
        fp_proc->burst_time = bt;
        fp_proc->remaining_time = bt;
        fp_proc->executed_time = 0;
        fp_proc->start_time = -1;
        fp_proc->completion_time = 0;
        fp_proc->io_start = bt / 2;
        fp_proc->io_duration = 2;
        fp_proc->io_remaining = 0;
        fp_proc->in_io = 0;
        fp_proc->next = NULL;

        insert_by_arrival(&fcfs_arrival_q, fp_proc);

        printf("Admitted PID %d\n", pid);
    }
    fclose(fp);

    printf("\n--- Starting FCFS Scheduler ---\n");
    fcfs_io_aware(&fcfs_arrival_q);

    printf("\n--- Starting CFS Scheduler ---\n");
    cfs_schedule();
    return 0;
}
