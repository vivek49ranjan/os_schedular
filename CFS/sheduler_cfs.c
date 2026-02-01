
#define BASE_WEIGHT 1024.0

typedef enum { READY } state_t;

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
    double start;
    double end;
} Gantt;

rb_node *runqueues[NUM_CORES] = {NULL};
int active_tasks_per_core[NUM_CORES] = {0};
process *finished_cfs[MAX_PROC];
int finished_count_cfs = 0;



void print_gantt_cfs(Gantt g[], int n, int core) {
    if (n == 0) return;

    printf("\nCORE %d GANTT CHART:\n", core);

    for (int i = 0; i < n; i++)
        printf("| P%d ", g[i].pid);

    printf("|\n%.1f", g[0].start);

    for (int i = 0; i < n; i++)
        printf("   %.1f", g[i].end);

    printf("\n");
}


double calculate_slice(double weight, int nr_running) {
    if (nr_running <= 0) return SCHED_PERIOD;
    return (SCHED_PERIOD * weight) / (BASE_WEIGHT * nr_running);
}

void update_vruntime(process *p, double delta_exec) {
    p->vruntime += delta_exec * (BASE_WEIGHT / p->weight);
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


rb_node* rb_extract_min(rb_node **root) {
    rb_node *m = *root;
    while (m && m->left) m = m->left;
    if (!m) return NULL;

    rb_node *child = m->right;
    rb_node *parent = m->parent;

    if (parent) parent->left = child;
    else *root = child;

    if (child) child->parent = parent;

    m->left = m->right = m->parent = NULL;
    return m;
}


void cfs_schedule() {
    for (int core = 0; core < NUM_CORES; core++) {

        double time = 0.0;
        Gantt gantt[500];
        int gcount = 0;

        if (runqueues[core] == NULL) continue;

        printf("\n--- CFS CORE %d METRICS ---\n", core);
        printf("PID | AT | BT | CT | TAT | WT | RT\n");

        while (runqueues[core] && active_tasks_per_core[core] > 0) {

            rb_node *node = rb_extract_min(&runqueues[core]);
            process *p = node->task;

            if (p->remaining_time == (double)p->burst_time)
                p->start_time = time;

            double slice = calculate_slice(p->weight, active_tasks_per_core[core]);
            double run = (p->remaining_time < slice) ? p->remaining_time : slice;

            gantt[gcount++] = (Gantt){p->pid, time, time + run};

            time += run;
            p->remaining_time -= run;
            update_vruntime(p, run);

            if (p->remaining_time > 0.001) {
                rb_insert(&runqueues[core], p);
            } else {
                active_tasks_per_core[core]--;
                p->completion_time = time;

                double tat = p->completion_time - p->arrival_time;
                double wt = tat - p->burst_time;
                double rt = p->start_time - p->arrival_time;

                printf("%3d | %2d | %2d | %4.1f | %4.1f | %4.1f | %4.1f\n",
                       p->pid, p->arrival_time, p->burst_time,
                       p->completion_time, tat, wt, rt);

                finished_cfs[finished_count_cfs++] = p;
            }

            free(node);
        }

        print_gantt_cfs(gantt, gcount, core);
    }
}
