#include <stdio.h>
#include <stdlib.h>

#define MAX_PROC 100

typedef struct
{
    int pid;
    int arrival_time;
    int burst_time;
    int start_time;
    int completion_time;
    int waiting_time;
    int turnaround_time;
    int response_time;
} Process;

typedef struct
{
    Process heap[MAX_PROC];
    int size;
} MinHeap;

typedef struct
{
    int pid;
    int start;
    int end;
} GanttStep;

void createProcess(Process *p, int id, int arr, int brst)
{
    p->pid = id;
    p->arrival_time = arr;
    p->burst_time = brst;
    p->start_time = -1;
    p->completion_time = 0;
    p->waiting_time = 0;
    p->turnaround_time = 0;
    p->response_time = 0;
}

int compareArrival(const void *a, const void *b)
{
    const Process *p1 = (const Process *)a;
    const Process *p2 = (const Process *)b;
    if (p1->arrival_time != p2->arrival_time)
        return p1->arrival_time - p2->arrival_time;
    return p1->pid - p2->pid;
}

int higherPriority(Process a, Process b)
{
    if (a.burst_time != b.burst_time)
        return a.burst_time < b.burst_time;
    if (a.arrival_time != b.arrival_time)
        return a.arrival_time < b.arrival_time;
    return a.pid < b.pid;
}

void swap(Process *a, Process *b)
{
    Process temp = *a;
    *a = *b;
    *b = temp;
}

void heapifyUp(MinHeap *h, int idx)
{
    while (idx > 0)
    {
        int parent = (idx - 1) / 2;
        if (higherPriority(h->heap[idx], h->heap[parent]))
        {
            swap(&h->heap[idx], &h->heap[parent]);
            idx = parent;
        }
        else
            break;
    }
}

void heapifyDown(MinHeap *h, int idx)
{
    while (1)
    {
        int smallest = idx;
        int left  = 2 * idx + 1;
        int right = 2 * idx + 2;
        if (left < h->size && higherPriority(h->heap[left], h->heap[smallest]))
            smallest = left;
        if (right < h->size && higherPriority(h->heap[right], h->heap[smallest]))
            smallest = right;
        if (smallest != idx)
        {
            swap(&h->heap[idx], &h->heap[smallest]);
            idx = smallest;
        }
        else
            break;
    }
}

void pushHeap(MinHeap *h, Process p)
{
    if (h->size >= MAX_PROC)
        return;
    h->heap[h->size] = p;
    heapifyUp(h, h->size);
    h->size++;
}

Process popHeap(MinHeap *h)
{
    Process min = h->heap[0];
    h->heap[0] = h->heap[h->size - 1];
    h->size--;
    heapifyDown(h, 0);
    return min;
}

void printGanttChart(GanttStep g[], int n)
{
    int i;
    printf("\n--- Gantt Chart ---\n ");
    for (i = 0; i < n; i++)
        printf("-------");
    printf("\n|");
    for (i = 0; i < n; i++)
    {
        if (g[i].pid == -1)
            printf(" IDLE |");
        else
            printf("  P%d  |", g[i].pid);
    }
    printf("\n ");
    for (i = 0; i < n; i++)
        printf("-------");
    printf("\n%d", g[0].start);
    for (i = 0; i < n; i++)
        printf("%7d", g[i].end);
    printf("\n\n");
}

void SJF(Process input[], int n)
{
    qsort(input, n, sizeof(Process), compareArrival);
    MinHeap rq;
    rq.size = 0;
    GanttStep gantt[MAX_PROC * 2];
    int g_idx = 0;
    Process result[MAX_PROC];
    int res_idx = 0;
    int time = 0;
    int i = 0;
    float total_wt = 0, total_tat = 0, total_rt = 0;

    while (res_idx < n)
    {
        while (i < n && input[i].arrival_time <= time)
        {
            pushHeap(&rq, input[i]);
            i++;
        }

        if (rq.size == 0)
        {
            int next_time = (i < n) ? input[i].arrival_time : time;
            if (g_idx > 0 && gantt[g_idx - 1].pid == -1)
                gantt[g_idx - 1].end = next_time;
            else
            {
                gantt[g_idx].pid = -1;
                gantt[g_idx].start = time;
                gantt[g_idx].end = next_time;
                g_idx++;
            }
            time = next_time;
            continue;
        }

        Process p = popHeap(&rq);
        p.start_time = time;
        p.response_time = time - p.arrival_time;
        p.waiting_time = p.response_time;
        gantt[g_idx].pid = p.pid;
        gantt[g_idx].start = time;
        time += p.burst_time;
        p.completion_time = time;
        p.turnaround_time = p.completion_time - p.arrival_time;
        gantt[g_idx].end = time;
        g_idx++;
        total_wt  += p.waiting_time;
        total_tat += p.turnaround_time;
        total_rt  += p.response_time;
        result[res_idx++] = p;
    }

    printGanttChart(gantt, g_idx);
    printf("Average Waiting Time: %.2f\n", total_wt / n);
    printf("Average Turnaround Time: %.2f\n", total_tat / n);
    printf("Average Response Time: %.2f\n", total_rt / n);
}