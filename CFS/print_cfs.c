void print_metrics() {
    double avg_wt = 0, avg_tat = 0, avg_rt = 0;

    printf("\n             PROCESS METRICS                 \n");
    printf("PID | AT | BT | CT  | TAT | WT  | RT\n");
    printf("                                               \n");

    for (int i = 0; i < finished_count; i++) {
        process *p = finished[i];

        double tat = p->completion_time - p->arrival_time;
        double wt  = tat - p->burst_time;
        double rt  = p->start_time - p->arrival_time;

        avg_tat += tat;
        avg_wt  += wt;
        avg_rt  += rt;

        printf("%3d | %2d | %2d | %4.1f | %4.1f | %4.1f | %4.1f\n",
               p->pid, p->arrival_time, p->burst_time,
               p->completion_time, tat, wt, rt);
    }

    int n = finished_count;
    printf("-----------------------------------------------\n");
    printf("AVG |    |    |      | %4.2f | %4.2f | %4.2f\n",
           avg_tat / n, avg_wt / n, avg_rt / n);
}

void print_gantt() {
    int max = history_count < 40 ? history_count : 40;

    printf("\n--- GANTT CHART (First %d slices) ---\n\n", max);

    for (int i = 0; i < max; i++) printf("+------");
    printf("+\n");

    for (int i = 0; i < max; i++)
        printf("| P%-3d", history[i].pid);
    printf("|\n");

    for (int i = 0; i < max; i++) printf("+------");
    printf("+\n");

    double t = 0.0;
    printf("%-6.1f", t);
    for (int i = 0; i < max; i++) {
        t += history[i].duration;
        printf("%-6.1f", t);
    }
    printf("\n");
}
