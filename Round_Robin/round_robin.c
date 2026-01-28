#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct process_rr
{
    char Pid[10];
    int Arrival_Time;
    int Burst_Time;
};


struct node_rr
{
    char pid[10];
    struct node_rr *next;
};


void enqueue_rr(struct node_rr **front, struct node_rr **rear, char val[10])
{
    struct node_rr *temp = (struct node_rr*)malloc(sizeof(struct node_rr));
    strcpy(temp->pid,val);
    temp->next = NULL;

    if(*rear == NULL){
        *front = *rear = temp;
    } else {
        (*rear)->next = temp;
        *rear = temp;
    }
}


void dequeue_rr(struct node_rr **front, struct node_rr **rear)
{
    if(*front == NULL){ 
        return;
    }

    struct node_rr *temp = *front;
    *front = (*front)->next;

    if(*front == NULL)
        *rear = NULL;

    free(temp);
}


bool empty_rr(struct node_rr *front)
{
    return front == NULL;
}


void round_robin(int n, struct process_rr p[],
                 struct node_rr **front, struct node_rr **rear,
                 int rem_bt[], char order[][10], int time[],
                 int *c1, int *c2)
{
    int quantum;
    int current_time = 0;
    int completed = 0;
    bool visited[n];
    bool first_quantum = true; 

    for(int i=0;i<n;i++){
        rem_bt[i] = p[i].Burst_Time;
        visited[i] = false;
    }

    time[0] = 0;
    printf("\nQuantum sizes used: ");

    while(completed < n)
    {
        for(int i=0;i<n;i++){
            if(p[i].Arrival_Time <= current_time && rem_bt[i] > 0 && !visited[i]){
                enqueue_rr(front,rear,p[i].Pid);
                visited[i] = true;
            }
        }

        if(empty_rr(*front)){
            current_time++;
            continue;
        }

        if (first_quantum) {
            quantum = 2;
            first_quantum = false; 
        } 
        else {
            double inv_sum = 0.0;
            int cnt = 0;
            struct node_rr *temp = *front;
            while(temp){
                for(int i=0;i<n;i++){
                    if(strcmp(p[i].Pid,temp->pid)==0){
                        if(rem_bt[i] > 0){
                            inv_sum += 1.0 / rem_bt[i];
                            cnt++;
                        }
                        break;
                    }
                }
                temp = temp->next;
            }
            if(cnt > 0 && inv_sum > 0){
                quantum = (int)(cnt / inv_sum);
            }
            else{
                quantum = 2; 
            }

            if(quantum < 2){ 
                quantum = 2;
            }
        }
        printf("%d  ", quantum);

        char current_pid[10];
        strcpy(current_pid,(*front)->pid);
        dequeue_rr(front,rear);

        int j;
        for(j=0;j<n;j++){
            if(strcmp(p[j].Pid,current_pid)==0){
                break;
            }
        }

        strcpy(order[*c1],current_pid);
        (*c1)++;
        time[*c2] = current_time;
        (*c2)++;

        if(rem_bt[j] > quantum){
            current_time += quantum;
            rem_bt[j] -= quantum;
        }
        else{
            current_time += rem_bt[j];
            rem_bt[j] = 0;
            completed++;
        }

        for(int i=0;i<n;i++){
            if(p[i].Arrival_Time <= current_time && rem_bt[i] > 0 && !visited[i]){
                enqueue_rr(front,rear,p[i].Pid);
                visited[i] = true;
            }
        }

        if(rem_bt[j] > 0){
            enqueue_rr(front,rear,p[j].Pid);
        }
    }
    time[*c2] = current_time;
    (*c2)++;
    printf("\n");
}


void print_gantt_rr(int c1,int c2,char order[][10],int time[])
{
    printf("\nRound Robin Gantt Chart:\n|");
    for(int i=0;i<c1;i++){
        printf(" %s |",order[i]);
    }

    printf("\n");
    for(int i=0;i<c2;i++){
        if(time[i]<10){
            printf("%d    ",time[i]);
        }
        else{
            printf("%d   ",time[i]);
        }
    }
    printf("\n");
}


void calculate_metrics_rr(int n,struct process_rr p[],char order[][10],int time[],int c1)
{
    int CT[n],TAT[n],WT[n],RT[n];
    printf("\nPID  AT   BT   CT   TAT    WT   RT\n");

    for(int i=0;i<n;i++){
        for(int k=c1-1;k>=0;k--){
            if(strcmp(order[k],p[i].Pid)==0){
                CT[i] = time[k+1];
                TAT[i] = CT[i] - p[i].Arrival_Time;
                WT[i] = TAT[i] - p[i].Burst_Time;
                break;
            }
        }
        for(int k=0;k<c1;k++){
            if(strcmp(order[k],p[i].Pid)==0){
                RT[i] = time[k] - p[i].Arrival_Time;
                break;
            }
        }
        printf("%s   %d    %d    %d    %d    %d    %d\n",
               p[i].Pid,p[i].Arrival_Time,p[i].Burst_Time,
               CT[i],TAT[i],WT[i],RT[i]);
    }
    float s1=0,s2=0,s3=0;
    for(int i=0;i<n;i++){
        s1+=TAT[i]; s2+=WT[i]; s3+=RT[i];
    }
    printf("\nAverage TAT = %.2f\n",s1/n);
    printf("Average WT  = %.2f\n",s2/n);
    printf("Average RT  = %.2f\n",s3/n);
}
