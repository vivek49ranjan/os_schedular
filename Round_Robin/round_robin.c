#include <stdio.h>
#include <stdbool.h>
#include <string.h>

struct process
{
    char Pid[10];
    int Arrival_Time;
    int Burst_Time;
};


void enqueue(char queue[][10], int *front, int *rear, char value[10], int n)
{
    if(((*rear + 1) % n) == *front)
        return;
    else if(*front == -1){
        *front = 0;
        *rear = 0;
        strcpy(queue[*rear], value);
    }
    else{
        *rear = (*rear + 1) % n;
        strcpy(queue[*rear], value);
    }
}


void dequeue(char queue[][10], int *front, int *rear, int n)
{
    if(*front == -1)
        return;
    else if(*front == *rear){
        *front = -1;
        *rear = -1;
    }
    else{
        *front = (*front + 1) % n;
    }
}



bool empty(char queue[][10], int *front, int *rear, int n)
{
    if(*front == -1)
        return 1;
    return 0;
}



bool present_in_queue(char pid[10], char array[][10],int n)
{
    for(int j=0;j<n;j++){
        if(strcmp(array[j], pid) == 0){
            return true;
        }
    }
    return false;
}



void read_input(int n,char Pid_list[][10],int Arrival_time_list[],int Burst_time_list[])
{
    int a,b;
    char c[10];

    for(int i=0;i<n;i++){
        scanf("%s",c);
        scanf("%d",&a);
        scanf("%d",&b);
        strcpy(Pid_list[i],c);
        Arrival_time_list[i]=a;
        Burst_time_list[i]=b;
    }
}



void fill_struct(int n,struct process p[],char Pid_list[][10],int Arrival_time_list[],int Burst_time_list[])
{
    for(int i=0;i<n;i++){
        strcpy(p[i].Pid,Pid_list[i]);
        p[i].Arrival_Time=Arrival_time_list[i];
        p[i].Burst_Time=Burst_time_list[i];
    }
}



void round_robin(int n,struct process p[],char queue[][10],int *front,int *rear,int remaining_burst_time[],char process_order[][10],int time[],int *count1,int *count2)
{
    int current_time=0;
    int quantum_size=2;
    int completed=0;

    time[0]=0;

    for(int i=0;i<n;i++)
        remaining_burst_time[i]=p[i].Burst_Time;

    for(int i=0;i<n;i++)
        if(p[i].Arrival_Time==0)
            enqueue(queue,front,rear,p[i].Pid,n);

    while(completed<n)
    {
        if(empty(queue,front,rear,n)){
            current_time++;

            for(int i=0;i<n;i++){
                if(p[i].Arrival_Time<=current_time && remaining_burst_time[i]>0 && !present_in_queue(p[i].Pid,queue,n)){
                    enqueue(queue,front,rear,p[i].Pid,n);
                }
            }
            continue;
        }

        char current_pid[10];
        strcpy(current_pid,queue[*front]);
        dequeue(queue,front,rear,n);

        int j;
        for(j=0;j<n;j++)
            if(strcmp(p[j].Pid,current_pid)==0)
                break;

        strcpy(process_order[*count1],p[j].Pid);
        (*count1)++;
        time[*count2]=current_time;
        (*count2)++;

        if(remaining_burst_time[j]>quantum_size){
            current_time+=quantum_size;
            remaining_burst_time[j]-=quantum_size;
        }
        else{
            current_time+=remaining_burst_time[j];
            remaining_burst_time[j]=0;
            completed++;
        }

        for(int i=0;i<n;i++){
            if(p[i].Arrival_Time<=current_time &&remaining_burst_time[i]>0 && !present_in_queue(p[i].Pid,queue,n)){
                enqueue(queue,front,rear,p[i].Pid,n);
            }
        }

        if(remaining_burst_time[j]>0)
            enqueue(queue,front,rear,p[j].Pid,n);
    }

    time[*count2]=current_time;
    (*count2)++;
}


void print_gantt(int count1,int count2,char process_order[][10],int time[])
{
    printf("\nGantt Chart:\n");
    for(int i=0;i<count1;i++)
        printf("  %s",process_order[i]);
    printf("\n");

    for(int i=0;i<count2;i++){
        if(time[i]<10){
            printf("%d   ",time[i]);
        }
        else{
            printf("%d  ",time[i]);
        }
    }
    printf("\n");
}

void calculate_metrics(int n,struct process p[],char process_order[][10],int time[],int count1)
{
    int CT[n];
    int TAT[n];
    int WT[n];
    int RT[n];
    int FET[n];

    printf("\n      TAT    WT    RT   AT   BT\n");

    for(int i=0;i<n;i++){

        CT[i] = -1;
        for(int k=count1-1;k>=0;k--){
            if(strcmp(process_order[k], p[i].Pid)==0){
                CT[i] = time[k+1];
                TAT[i] = CT[i]-p[i].Arrival_Time;
                WT[i] = TAT[i]- p[i].Burst_Time;
                break;
            }
        }

        FET[i]= -1;
        for(int k =0; k<count1; k++){
            if(strcmp(process_order[k], p[i].Pid)==0){
                FET[i] = time[k];
                RT[i] = FET[i]-p[i].Arrival_Time;
                break;
            }
        }

        printf("%s-:  %d      %d    %d    %d    %d\n",
               p[i].Pid,TAT[i],WT[i],RT[i],
               p[i].Arrival_Time,p[i].Burst_Time);
    }

    int sum_TAT=0,sum_RT=0,sum_WT=0;

    for(int i=0; i<n; i++){
        sum_TAT += TAT[i];
        sum_WT += WT[i];
        sum_RT += RT[i];
    }

    printf("\nAverage TAT-: %f\n",(float)sum_TAT/n);
    printf("Average WT-: %f\n",(float)sum_WT/n);
    printf("Average RT-: %f\n",(float)sum_RT/n);
}



int main()
{
    int n;
    scanf("%d",&n);

    char queue[n][10];
    char pop_array[n][10];
    int front=-1,rear=-1;

    char Pid_list[n][10];
    int Arrival_time_list[n];
    int Burst_time_list[n];

    read_input(n,Pid_list,Arrival_time_list,Burst_time_list);

    struct process p[n];
    fill_struct(n,p,Pid_list,Arrival_time_list,Burst_time_list);

    int remaining_burst_time[n];
    char process_order[100][10];
    int time[100];
    int count1=0,count2=0;

    round_robin(n,p,queue,&front,&rear,remaining_burst_time,process_order,time,&count1,&count2);

    print_gantt(count1,count2,process_order,time);

    calculate_metrics(n,p,process_order,time,count1);

    return 0;
}
