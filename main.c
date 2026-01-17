#include <stdio.h>

/* FCFS function declaration */
void fcfs_scheduler(int n, int at[], int bt[]);

int main() {
    int n;
    int at[20], bt[20];

    printf("Enter number of processes: ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++) {
        printf("\nProcess P%d\n", i + 1);
        printf("Arrival Time: ");
        scanf("%d", &at[i]);
        printf("Burst Time: ");
        scanf("%d", &bt[i]);
    }

    fcfs_scheduler(n, at, bt);
    return 0;
}
