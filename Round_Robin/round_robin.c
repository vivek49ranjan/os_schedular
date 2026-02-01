struct process_rr {
    char Pid[10];
    int Arrival_Time;
    int Burst_Time;
};

struct node_rr {
    char pid[10];
    struct node_rr *next;
};

void enqueue_rr(struct node_rr **f, struct node_rr **r, char val[10]) {
    struct node_rr *t = malloc(sizeof(struct node_rr));
    strcpy(t->pid, val);
    t->next = NULL;
    if (!*r) {
        *f = *r = t;
    } else {
        (*r)->next = t;
        *r = t;
    }
}

void dequeue_rr(struct node_rr **f, struct node_rr **r) {
    if (!*f) return;
    struct node_rr *t = *f;
    *f = (*f)->next;
    if (!*f) *r = NULL;
    free(t);
}

void round_robin(int n, struct process_rr p[]) {
    int rem_bt[MAX_PROC];
    int CT[MAX_PROC];
    int TAT[MAX_PROC];
    int WT[MAX_PROC];
    int RT[MAX_PROC];
    bool responded[MAX_PROC];
    for (int i = 0; i < n; i++) responded[i] = false;

    for (int core = 0; core < NUM_CORES; core++) {
        struct node_rr *cf = NULL;
        struct node_rr *cr = NULL;
        int current_time = 0;
        int completed = 0;
        int core_n = 0;
        int core_p_indices[MAX_PROC];

        for (int i = 0; i < n; i++) {
            int p_id;
            sscanf(p[i].Pid, "P%d", &p_id);
            if (p_id % NUM_CORES == core) {
                core_p_indices[core_n] = i;
                core_n++;
                rem_bt[i] = p[i].Burst_Time;
            }
        }
        if (core_n == 0) continue;

        printf("\n--- RR CORE %d EXECUTION ---\n", core);
        while (completed < core_n) {
            for (int k = 0; k < core_n; k++) {
                int i = core_p_indices[k];
                if (p[i].Arrival_Time <= current_time && rem_bt[i] > 0) {
                    bool exist = false;
                    for (struct node_rr *t = cf; t; t = t->next)
                        if (strcmp(t->pid, p[i].Pid) == 0) exist = true;
                    if (!exist) enqueue_rr(&cf, &cr, p[i].Pid);
                }
            }
            if (!cf) {
                current_time++;
                continue;
            }
            char cur[10];
            strcpy(cur, cf->pid);
            dequeue_rr(&cf, &cr);
            int j;
            for (j = 0; j < n; j++)
                if (strcmp(p[j].Pid, cur) == 0) break;

            if (!responded[j]) {
                RT[j] = current_time - p[j].Arrival_Time;
                responded[j] = true;
            }

            int q = 4;
            int exec = (rem_bt[j] > q) ? q : rem_bt[j];
            rem_bt[j] -= exec;
            current_time += exec;
            printf("  Time %d: %s runs for %d\n", current_time - exec, cur, exec);

            if (rem_bt[j] == 0) {
                completed++;
                CT[j] = current_time;
                TAT[j] = CT[j] - p[j].Arrival_Time;
                WT[j] = TAT[j] - p[j].Burst_Time;
            } else {
                enqueue_rr(&cf, &cr, cur);
            }
        }
        printf("\nCORE %d RR METRICS:\nPID | AT | BT | CT | TAT | WT | RT\n", core);
        for (int k = 0; k < core_n; k++) {
            int i = core_p_indices[k];
            printf("%3s | %2d | %2d | %2d | %3d | %3d | %3d\n",
                   p[i].Pid, p[i].Arrival_Time, p[i].Burst_Time, CT[i], TAT[i], WT[i], RT[i]);
        }
    }
}
