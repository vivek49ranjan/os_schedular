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

rb_node *runqueue = NULL;
int active_tasks = 0;

gantt_step history[MAX_HISTORY];
int history_count = 0;

process *finished[MAX_PROC];
int finished_count = 0;

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
    printf("--- Admitting Processes ---\n");

    while (fscanf(fp, "%d %d %d", &pid, &at, &bt) == 3) {
        process *p = malloc(sizeof(process));
        *p = (process){pid, at, bt, bt, 0.0, BASE_WEIGHT, 0, 0, READY};
        rb_insert(&runqueue, p);
        active_tasks++;
        printf("Admitted PID %d\n", pid);
    }
    fclose(fp);

    printf("\n--- Starting CFS Scheduler ---\n");
    cfs_schedule();
    return 0;
}
