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
void cfs_schedule(process proc[], int n)
{
    double time = 0.0;
    int alive = n;
    for (int i = 0; i < n; i++) {
        if (proc[i].state == READY)
            rb_insert(&root, &proc[i]);
    }

    while (alive > 0 && root) {

        rb_node *node = pick_next_process(root);
        process *p = node->task;

        rb_delete(&root, node);

        p->state = RUNNING;

        int nr_running = alive;
        double slice = calc_slice(p->weight, nr_running);

        double run = (p->exec_time < slice) ? p->exec_time : slice;

        printf("[t=%.2f] PID %d runs for %.2f (vruntime=%.2f)\n",
               time, p->pid, run, p->vruntime);

        time += run;

        p->exec_time -= run;
        update_vruntime(p, run);

        if (p->exec_time <= 0) {
            p->state = DONE;
            alive--;
            printf("PID %d finished at t=%.2f\n", p->pid, time);
        } else {
            p->state = READY;
            rb_insert(&root, p);
        }
    }
}
