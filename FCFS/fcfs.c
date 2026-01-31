
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

void init_queue(Queue *q) {
    q->front = q->rear = NULL;
}

int is_empty(Queue *q) {
    return q->front == NULL;
}

void enqueue_fcfs(Queue *q, ProcessIO *p) {
    p->next = NULL;
    if (!q->rear) {
        q->front = q->rear = p;
        return;
    }
    q->rear->next = p;
    q->rear = p;
}

ProcessIO* dequeue_fcfs(Queue *q) {
    if (!q->front) return NULL;
    ProcessIO *p = q->front;
    q->front = p->next;
    if (!q->front) q->rear = NULL;
    p->next = NULL;
    return p;
}

void fcfs_io_aware(Queue *arrival_q) {
    Queue core_ready_qs[NUM_CORES];
    for (int i = 0; i < NUM_CORES; i++)
        init_queue(&core_ready_qs[i]);
    while (!is_empty(arrival_q)) {
        ProcessIO *p = dequeue_fcfs(arrival_q);
        enqueue_fcfs(&core_ready_qs[p->pid % NUM_CORES], p);
    }
    for (int core = 0; core < NUM_CORES; core++) {
        Queue io_q;
        init_queue(&io_q);
        int time = 0;
        ProcessIO *running = NULL;
        if (is_empty(&core_ready_qs[core])) continue;
        printf("\nCORE %d FCFS METRICS:\nPID | AT | BT | ST | CT | TAT | WT\n", core);
        printf("------------------------------------------\n");
        while (!is_empty(&core_ready_qs[core]) || !is_empty(&io_q) || running) {
            int io_count = 0;
            for (ProcessIO *p = io_q.front; p; p = p->next) io_count++;
            while (io_count--) {
                ProcessIO *p = dequeue_fcfs(&io_q);
                p->io_remaining--;
                if (p->io_remaining == 0) {
                    p->in_io = 0;
                    enqueue_fcfs(&core_ready_qs[core], p);
                } else enqueue_fcfs(&io_q, p);
            }
            if (!running && !is_empty(&core_ready_qs[core])) {
                running = dequeue_fcfs(&core_ready_qs[core]);
                if (running->start_time == -1) running->start_time = time;
            }
            if (running) {
                running->remaining_time--;
                running->executed_time++;
                if (running->executed_time == running->io_start && running->io_duration > 0) {
                    running->io_remaining = running->io_duration;
                    enqueue_fcfs(&io_q, running);
                    running = NULL;
                } else if (running->remaining_time == 0) {
                    running->completion_time = time + 1;
                    int tat = running->completion_time - running->arrival_time;
                    int wt = tat - running->burst_time;
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
}
