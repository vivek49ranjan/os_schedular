
#define AGING_FACTOR 1

typedef struct {
    int pid;
    int arrival;
    int burst;
    int start;
    int completion;
    int age;
    int waiting;
    int turnaround;
} ProcessSJF;

typedef struct {
    ProcessSJF data[MAX_PROC];
    int size;
} ReadyQueueSJF;

int hasHigherPriority(ProcessSJF a, ProcessSJF b) {
    int effA = a.burst - a.age * AGING_FACTOR;
    int effB = b.burst - b.age * AGING_FACTOR;
    if (effA != effB) return effA < effB;
    return a.arrival < b.arrival;
}

void pushSJF(ReadyQueueSJF *rq, ProcessSJF p) {
    rq->data[rq->size] = p;
    int i = rq->size++;
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (hasHigherPriority(rq->data[i], rq->data[parent])) {
            ProcessSJF tmp = rq->data[i];
            rq->data[i] = rq->data[parent];
            rq->data[parent] = tmp;
            i = parent;
        } else break;
    }
}

ProcessSJF popSJF(ReadyQueueSJF *rq) {
    ProcessSJF top = rq->data[0];
    rq->data[0] = rq->data[--rq->size];
    int i = 0;
    while (1) {
        int best = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        if (left < rq->size && hasHigherPriority(rq->data[left], rq->data[best])) best = left;
        if (right < rq->size && hasHigherPriority(rq->data[right], rq->data[best])) best = right;
        if (best != i) {
            ProcessSJF tmp = rq->data[i];
            rq->data[i] = rq->data[best];
            rq->data[best] = tmp;
            i = best;
        } else break;
    }
    return top;
}

void scheduleSJF(ProcessSJF processes[], int n) {
    for (int core = 0; core < NUM_CORES; core++) {
        ReadyQueueSJF rq;
        rq.size = 0;
        int time = 0;
        int done = 0;
        int core_total = 0;
        ProcessSJF core_finished[MAX_PROC];
        int core_f_count = 0;

        for (int i = 0; i < n; i++)
            if (processes[i].pid % NUM_CORES == core) core_total++;
        if (core_total == 0) continue;

        printf("\n--- SJF CORE %d METRICS ---\nPID | AT | BT | ST | CT | TAT | WT\n", core);
        while (done < core_total) {
            for (int i = 0; i < n; i++) {
                if (processes[i].pid % NUM_CORES == core && processes[i].arrival <= time && processes[i].start == -1) {
                    processes[i].start = -2;
                    pushSJF(&rq, processes[i]);
                }
            }
            if (rq.size == 0) {
                time++;
                continue;
            }
            ProcessSJF current = popSJF(&rq);
            current.start = time;
            time += current.burst;
            current.completion = time;
            current.turnaround = current.completion - current.arrival;
            current.waiting = current.turnaround - current.burst;
            printf("%3d | %2d | %2d | %2d | %2d | %3d | %3d\n",
                   current.pid, current.arrival, current.burst, current.start, current.completion, current.turnaround, current.waiting);
            done++;
        }
    }
}
