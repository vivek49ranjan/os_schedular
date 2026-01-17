#define BASE_WEIGHT 1.0
#define ALPHA 0.5    
#define BETA  1.0     

double calculate_weight(int arrival_time, int burst_time, int max_arrival)
{
    if (burst_time <= 0)
        burst_time = 1;  
    double arrival_component = ALPHA * (max_arrival - arrival_time);
    double burst_component   = BETA * (1.0 / burst_time);

    return BASE_WEIGHT + arrival_component + burst_component;
}
