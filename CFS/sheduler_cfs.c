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


rb_node* pick_min(rb_node *root) {
    while (root && root->left) root = root->left;
    return root;
}

double calc_slice(double weight, int nr) {
    return (nr > 0) ? (SCHED_PERIOD * weight) / (BASE_WEIGHT * nr) : SCHED_PERIOD;
}

void update_vruntime(process *p, double runtime) {
    p->vruntime += runtime * (BASE_WEIGHT / p->weight);
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
