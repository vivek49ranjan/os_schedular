#include <stdio.h>

/* Utility function declaration */
void print_table(int n, int at[], int bt[], int ct[], int wt[], int tat[], int rt[]);

void fcfs_scheduler(int n, int at[], int bt[]) {
    int ct[20], wt[20], tat[20], rt[20];
    int time = 0;

    printf("\nGANTT CHART\n|");

    for (int i = 0; i < n; i++) {
        if (time < at[i])
            time = at[i];

        printf(" P%d |", i + 1);

        rt[i] = time - at[i];
        time += bt[i];
        ct[i] = time;

        tat[i] = ct[i] - at[i];
        wt[i] = tat[i] - bt[i];
    }

    printf("\n");
    print_table(n, at, bt, ct, wt, tat, rt);
}
