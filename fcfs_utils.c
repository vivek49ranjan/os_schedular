#include <stdio.h>

void print_table(int n, int at[], int bt[], int ct[], int wt[], int tat[], int rt[]) {
    float avg_wt = 0, avg_tat = 0, avg_rt = 0;

    printf("\nPROCESS TABLE\n");
    printf("PID\tAT\tBT\tCT\tWT\tTAT\tRT\n");

    for (int i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
               i + 1, at[i], bt[i], ct[i], wt[i], tat[i], rt[i]);

        avg_wt += wt[i];
        avg_tat += tat[i];
        avg_rt += rt[i];
    }

    printf("\nAverage Waiting Time     = %.2f", avg_wt / n);
    printf("\nAverage Turnaround Time  = %.2f", avg_tat / n);
    printf("\nAverage Response Time    = %.2f\n", avg_rt / n);
}
