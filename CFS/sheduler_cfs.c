rb_node* pick_next_process(rb_node *root)
{
    if (!root) return NULL;

    while (root->left)
        root = root->left;

    return root;   
}
#define SCHED_PERIOD 10.0
#define BASE_WEIGHT 1024.0

double calc_slice(int weight, int nr_running)
{
    return (SCHED_PERIOD * weight) / (BASE_WEIGHT * nr_running);
}
void update_vruntime(process *p, double runtime)
{
    p->vruntime += runtime * (BASE_WEIGHT / p->weight);
}
rb_node *runqueue = NULL;
int active_tasks = 0;

void cfs_schedule_dynamic() {
    double current_time = 0.0;

    while (runqueue != NULL) {
        rb_node *node = pick_next_process(runqueue);
        process *p = node->task;
        rb_delete(&runqueue, node); 
        p->state = RUNNING;

        double slice = calc_slice(p->weight, active_tasks);
        double run = (p->remaining_time < slice) ? p->remaining_time : slice;

        printf("[t=%.2f] PID %d runs for %.2f (vruntime=%.2f)\n", 
               current_time, p->pid, run, p->vruntime);

        current_time += run;
        p->remaining_time -= run;
        update_vruntime(p, run);

        if (p->remaining_time <= 0) {
            p->state = FINISHED;
            active_tasks--;
            printf("PID %d finished at t=%.2f\n", p->pid, current_time);
        } else {
            p->state = READY;
            rb_insert(&runqueue, p);
        }
    }
}
