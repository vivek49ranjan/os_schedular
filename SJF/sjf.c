#define AGING_FACTOR 1

typedef struct { int pid, arrival, burst, start, completion, age, waiting, turnaround; } ProcessSJF;
typedef struct { ProcessSJF data[MAX_PROC]; int size; } ReadyQueue;
typedef struct { int pid, start, end; } Gantt_sjf;

static void print_gantt_sjf(Gantt_sjf g[], int n, int core) {
    printf("\nCORE %d GANTT CHART:\n", core);
    for (int i = 0; i < n; i++) printf("| P%d ", g[i].pid);
    printf("|\n%d", g[0].start);
    for (int i = 0; i < n; i++) printf("   %d", g[i].end);
    printf("\n");
}

int higher(ProcessSJF a, ProcessSJF b) {
    int ea = a.burst - a.age * AGING_FACTOR;
    int eb = b.burst - b.age * AGING_FACTOR;
    return ea == eb ? a.arrival < b.arrival : ea < eb;
}

void push(ReadyQueue *rq, ProcessSJF p) {
    rq->data[rq->size] = p;
    int i = rq->size++;
    while (i && higher(rq->data[i], rq->data[(i-1)/2])) {
        ProcessSJF t = rq->data[i];
        rq->data[i] = rq->data[(i-1)/2];
        rq->data[(i-1)/2] = t;
        i = (i-1)/2;
    }
}

ProcessSJF pop(ReadyQueue *rq) {
    ProcessSJF t = rq->data[0];
    rq->data[0] = rq->data[--rq->size];
    int i = 0;
    while (1) {
        int l = 2*i+1, r = 2*i+2, s = i;
        if (l < rq->size && higher(rq->data[l], rq->data[s])) s = l;
        if (r < rq->size && higher(rq->data[r], rq->data[s])) s = r;
        if (s == i) break;
        ProcessSJF tmp = rq->data[i];
        rq->data[i] = rq->data[s];
        rq->data[s] = tmp;
        i = s;
    }
    return t;
}

void scheduleSJF(ProcessSJF p[], int n) {
    for (int core = 0; core < NUM_CORES; core++) {
        ReadyQueue rq = {.size = 0};
        int time = 0, done = 0, total = 0;
        Gantt_sjf gantt[500]; int gcount = 0;

        for (int i = 0; i < n; i++)
            if (p[i].pid % NUM_CORES == core) { p[i].start = -1; p[i].age = 0; total++; }

        if (!total) continue;

        printf("\n--- SJF CORE %d ---\n", core);
        printf("PID | AT | BT | ST | CT | TAT | WT\n");

        while (done < total) {
            for (int i = 0; i < n; i++)
                if (p[i].pid % NUM_CORES == core && p[i].arrival <= time && p[i].start == -1) {
                    p[i].start = -2;
                    push(&rq, p[i]);
                }

            if (!rq.size) { time++; continue; }

            for (int i = 0; i < rq.size; i++) rq.data[i].age++;

            ProcessSJF cur = pop(&rq);
            cur.start = time;
            gantt[gcount++] = (Gantt_sjf){cur.pid, time, time + cur.burst};
            time += cur.burst;

            cur.completion = time;
            cur.turnaround = cur.completion - cur.arrival;
            cur.waiting = cur.turnaround - cur.burst;

            printf("%3d | %2d | %2d | %2d | %2d | %3d | %3d\n",
                   cur.pid, cur.arrival, cur.burst,
                   cur.start, cur.completion, cur.turnaround, cur.waiting);
            done++;
        }
        print_gantt_sjf(gantt, gcount, core);
    }
}
