#define SCHED_PERIOD 10.0
#define BASE_WEIGHT_VAL 1.0
#define ALPHA 0.5    
#define BETA  1.0     

double calculate_weight(int arrival, int burst, int max_arrival) {
    double burst_val = (burst <= 0) ? 1.0 : (double)burst;
    double arrival_comp = ALPHA * (max_arrival - arrival);
    double burst_comp = BETA * (1.0 / burst_val);
    return BASE_WEIGHT_VAL + arrival_comp + burst_comp;
}

double calc_slice(double weight, int nr_running) {
    if (nr_running <= 0) return SCHED_PERIOD;
    return (SCHED_PERIOD * weight) / (BASE_WEIGHT_VAL * nr_running);
}

void update_vruntime(process *p, double runtime) {
    p->vruntime += runtime * (BASE_WEIGHT_VAL / p->weight);
}
