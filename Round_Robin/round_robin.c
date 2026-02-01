struct process_rr {
    char Pid[10];
    int Arrival_Time;
    int Burst_Time;
};

struct node_rr {
    char pid[10];
    struct node_rr *next;
};

typedef struct {
    int pid;
    int start;
    int end;
} Gantt_rr;

void enqueue_rr(struct node_rr **f, struct node_rr **r, char val[10]) {
    struct node_rr *temp = malloc(sizeof(struct node_rr));
    strcpy(temp->pid, val);
    temp->next = NULL;

    if (*r == NULL) *f = *r = temp;
    else {
        (*r)->next = temp;
        *r = temp;
    }
}

void dequeue_rr(struct node_rr **f, struct node_rr **r) {
    if (*f == NULL) return;
    struct node_rr *temp = *f;
    *f = (*f)->next;
    if (*f == NULL) *r = NULL;
    free(temp);
}

int harmonic_mean_quantum(struct node_rr *front, struct process_rr p[], int rem_bt[], int n) {
    double sum = 0.0;
    int count = 0;

    for (struct node_rr *t = front; t; t = t->next) {
        for (int i = 0; i < n; i++) {
            if (strcmp(p[i].Pid, t->pid) == 0 && rem_bt[i] > 0) {
                sum += 1.0 / rem_bt[i];
                count++;
                break;
            }
        }
    }

    if (count == 0) return 1;
    int q = (int)(count / sum);
    return (q < 2) ? 2 : q;
}

void round_robin(int n, struct process_rr p[]) {

    int rem_bt[MAX_PROC];
    int CT[MAX_PROC] = {0}, TAT[MAX_PROC] = {0}, WT[MAX_PROC] = {0}, RT[MAX_PROC] = {0};
    bool responded[MAX_PROC] = {false};

    for (int i = 0; i < n; i++)
        rem_bt[i] = p[i].Burst_Time;

    for (int core = 0; core < NUM_CORES; core++) {

        struct node_rr *front = NULL, *rear = NULL;
        int current_time = 0, completed = 0;

        int core_indices[MAX_PROC], core_n = 0;

        for (int i = 0; i < n; i++) {
            int pid_num;
            sscanf(p[i].Pid, "P%d", &pid_num);
            if (pid_num % NUM_CORES == core)
                core_indices[core_n++] = i;
        }
        if (core_n == 0) continue;

        Gantt_rr gantt[MAX_GANTT];
        int gcount = 0;

        printf("\n--- RR CORE %d ---\n", core);

        while (completed < core_n) {

            for (int k = 0; k < core_n; k++) {
                int i = core_indices[k];
                if (p[i].Arrival_Time <= current_time && rem_bt[i] > 0) {
                    bool exists = false;
                    for (struct node_rr *t = front; t; t = t->next)
                        if (strcmp(t->pid, p[i].Pid) == 0) exists = true;
                    if (!exists)
                        enqueue_rr(&front, &rear, p[i].Pid);
                }
            }

            if (!front) {
                current_time++;
                continue;
            }

            char current_pid[10];
            strcpy(current_pid, front->pid);
            dequeue_rr(&front, &rear);

            int j;
            for (j = 0; j < n; j++)
                if (strcmp(p[j].Pid, current_pid) == 0) break;

            if (!responded[j]) {
                RT[j] = current_time - p[j].Arrival_Time;
                responded[j] = true;
            }

            int q = harmonic_mean_quantum(front, p, rem_bt, n);
            int exec = (rem_bt[j] > q) ? q : rem_bt[j];

            gantt[gcount++] = (Gantt_rr){atoi(&current_pid[1]), current_time, current_time + exec};

            rem_bt[j] -= exec;
            current_time += exec;

            if (rem_bt[j] == 0) {
                completed++;
                CT[j] = current_time;
                TAT[j] = CT[j] - p[j].Arrival_Time;
                WT[j] = TAT[j] - p[j].Burst_Time;
            } else {
                enqueue_rr(&front, &rear, current_pid);
            }
        }

        printf("PID | AT | BT | CT | TAT | WT | RT\n");
        for (int k = 0; k < core_n; k++) {
            int i = core_indices[k];
            printf("%3s | %2d | %2d | %3d | %3d | %3d | %3d\n",
                   p[i].Pid, p[i].Arrival_Time, p[i].Burst_Time,
                   CT[i], TAT[i], WT[i], RT[i]);
        }

        printf("\nCORE %d GANTT CHART:\n|", core);
        for (int i = 0; i < gcount; i++)
            printf(" P%d |", gantt[i].pid);

        printf("\n%d", gantt[0].start);
        for (int i = 0; i < gcount; i++)
            printf("   %d", gantt[i].end);
        printf("\n");
    }
}
