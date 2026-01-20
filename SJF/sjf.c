#include <stdio.h>
#include <stdlib.h>

#define MAX_PROC 100
#define AGING_FACTOR 1

typedef struct
{
    int pid;
    int arrival;
    int burst;
    int start;
    int completion;
    int waiting;
    int turnaround;
    int response;
    int age;
} Process;

typedef struct
{
    Process data[MAX_PROC];
    int size;
} ReadyQueue;

typedef struct
{
    int pid;
    int start;
    int end;
} Gantt;

void initProcess(Process *p, int id, int at, int bt)
{
    p->pid = id;
    p->arrival = at;
    p->burst = bt;
    p->start = -1;
    p->completion = 0;
    p->waiting = 0;
    p->turnaround = 0;
    p->response = 0;
    p->age = 0;
}

int sortByArrival(const void *a, const void *b)
{
    Process *p1 = (Process *)a;
    Process *p2 = (Process *)b;

    if (p1->arrival != p2->arrival)
        return p1->arrival - p2->arrival;

    return p1->pid - p2->pid;
}

int hasHigherPriority(Process a, Process b)
{
    int effectiveA = a.burst - a.age * AGING_FACTOR;
    int effectiveB = b.burst - b.age * AGING_FACTOR;

    if (effectiveA != effectiveB)
        return effectiveA < effectiveB;

    if (a.arrival != b.arrival)
        return a.arrival < b.arrival;

    return a.pid < b.pid;
}

void swap(Process *a, Process *b)
{
    Process temp = *a;
    *a = *b;
    *b = temp;
}

void heapifyUp(ReadyQueue *rq, int index)
{
    while (index > 0)
    {
        int parent = (index - 1) / 2;

        if (hasHigherPriority(rq->data[index], rq->data[parent]))
        {
            swap(&rq->data[index], &rq->data[parent]);
            index = parent;
        }
        else
            break;
    }
}

void heapifyDown(ReadyQueue *rq, int index)
{
    while (1)
    {
        int best = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < rq->size && hasHigherPriority(rq->data[left], rq->data[best]))
            best = left;

        if (right < rq->size && hasHigherPriority(rq->data[right], rq->data[best]))
            best = right;

        if (best != index)
        {
            swap(&rq->data[index], &rq->data[best]);
            index = best;
        }
        else
            break;
    }
}

void pushProcess(ReadyQueue *rq, Process p)
{
    rq->data[rq->size] = p;
    heapifyUp(rq, rq->size);
    rq->size++;
}

Process popProcess(ReadyQueue *rq)
{
    Process top = rq->data[0];
    rq->data[0] = rq->data[rq->size - 1];
    rq->size--;
    heapifyDown(rq, 0);
    return top;
}

void increaseAge(ReadyQueue *rq)
{
    for (int i = 0; i < rq->size; i++)
        rq->data[i].age++;
}

void printGantt(Gantt chart[], int count)
{
    printf("\nGantt Chart\n ");

    for (int i = 0; i < count; i++)
        printf("-------");

    printf("\n|");

    for (int i = 0; i < count; i++)
    {
        if (chart[i].pid == -1)
            printf(" IDLE |");
        else
            printf("  P%d  |", chart[i].pid);
    }

    printf("\n ");

    for (int i = 0; i < count; i++)
        printf("-------");

    printf("\n%d", chart[0].start);

    for (int i = 0; i < count; i++)
        printf("%7d", chart[i].end);

    printf("\n\n");
}

void scheduleSJF(Process processes[], int n)
{
    qsort(processes, n, sizeof(Process), sortByArrival);

    ReadyQueue rq;
    rq.size = 0;

    Gantt chart[MAX_PROC * 2];
    int chartSize = 0;

    int time = 0, index = 0, done = 0;
    float avgWait = 0, avgTurn = 0, avgResp = 0;

    while (done < n)
    {
        while (index < n && processes[index].arrival <= time)
            pushProcess(&rq, processes[index++]);

        if (rq.size == 0)
        {
            int nextTime = processes[index].arrival;
            chart[chartSize++] = (Gantt){-1, time, nextTime};
            time = nextTime;
            continue;
        }

        increaseAge(&rq);
        Process current = popProcess(&rq);

        current.start = time;
        current.response = time - current.arrival;
        current.waiting = current.response;

        chart[chartSize++] = (Gantt){current.pid, time, time + current.burst};

        time += current.burst;
        current.completion = time;
        current.turnaround = current.completion - current.arrival;

        avgWait += current.waiting;
        avgTurn += current.turnaround;
        avgResp += current.response;

        done++;
    }

    printGantt(chart, chartSize);

    printf("Average Waiting Time    : %.2f\n", avgWait / n);
    printf("Average Turnaround Time : %.2f\n", avgTurn / n);
    printf("Average Response Time   : %.2f\n", avgResp / n);
}
