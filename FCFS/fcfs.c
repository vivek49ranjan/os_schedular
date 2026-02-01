typedef struct ProcessIO {
    int pid, arrival_time, burst_time;
    int remaining_time, executed_time;
    int start_time, completion_time;
    int io_start, io_duration, io_remaining;
    int in_io;
    struct ProcessIO *next;
} ProcessIO;

typedef struct {
    ProcessIO *front, *rear;
} Queue;

typedef struct {
    int pid, start, end;
} Gantt_fcfs;

void init_queue(Queue *q) { q->front = q->rear = NULL; }
int is_empty(Queue *q) { return q->front == NULL; }

void enqueue_fcfs(Queue *q, ProcessIO *p) {
    p->next = NULL;
    if (!q->rear) q->front = q->rear = p;
    else { q->rear->next = p; q->rear = p; }
}

ProcessIO* dequeue_fcfs(Queue *q) {
    if (!q->front) return NULL;
    ProcessIO *p = q->front;
    q->front = p->next;
    if (!q->front) q->rear = NULL;
    return p;
}

static void print_gantt_fcfs(Gantt_fcfs g[], int n, int core) {
    printf("\nCORE %d GANTT CHART:\n", core);
    for (int i = 0; i < n; i++) printf("| P%d ", g[i].pid);
    printf("|\n%d", g[0].start);
    for (int i = 0; i < n; i++) printf("   %d", g[i].end);
    printf("\n");
}

void fcfs_io_aware(Queue *arrival_q) {
    Queue ready[NUM_CORES], io_q;
    for (int i = 0; i < NUM_CORES; i++) init_queue(&ready[i]);
    init_queue(&io_q);

    while (!is_empty(arrival_q)) {
        ProcessIO *p = dequeue_fcfs(arrival_q);
        enqueue_fcfs(&ready[p->pid % NUM_CORES], p);
    }

    for (int core = 0; core < NUM_CORES; core++) {
        int time = 0;
        ProcessIO *running = NULL;
        Gantt_fcfs gantt[500]; int gcount = 0;

        printf("\n--- FCFS CORE %d ---\n", core);
        printf("PID | AT | BT | ST | CT | TAT | WT\n");

        while (!is_empty(&ready[core]) || !is_empty(&io_q) || running) {
            int io_n = 0;
            for (ProcessIO *p = io_q.front; p; p = p->next) io_n++;
            while (io_n--) {
                ProcessIO *p = dequeue_fcfs(&io_q);
                if (--p->io_remaining == 0) enqueue_fcfs(&ready[core], p);
                else enqueue_fcfs(&io_q, p);
            }

            if (!running && !is_empty(&ready[core])) {
                running = dequeue_fcfs(&ready[core]);
                if (running->start_time == -1) running->start_time = time;
                gantt[gcount].pid = running->pid;
                gantt[gcount].start = time;
            }

            if (running) {
                running->remaining_time--;
                running->executed_time++;

                if (running->executed_time == running->io_start && running->io_duration) {
                    running->io_remaining = running->io_duration;
                    enqueue_fcfs(&io_q, running);
                    gantt[gcount++].end = time + 1;
                    running = NULL;
                } else if (running->remaining_time == 0) {
                    running->completion_time = time + 1;
                    int tat = running->completion_time - running->arrival_time;
                    int wt = tat - running->burst_time;
                    printf("%3d | %2d | %2d | %2d | %2d | %3d | %3d\n",
                           running->pid, running->arrival_time, running->burst_time,
                           running->start_time, running->completion_time, tat, wt);
                    gantt[gcount++].end = time + 1;
                    free(running);
                    running = NULL;
                }
            }
            time++;
        }
        if (gcount) print_gantt_fcfs(gantt, gcount, core);
    }
}
