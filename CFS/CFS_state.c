typedef enum {
    NEW,
    READY,
    RUNNING,
    FINISHED
} state_t;
void update_state(process *p, int current_time)
{
    if (p->remaining_time == 0) {
        p->state = FINISHED;
    }
    else if (current_time < p->arrival_time) {
        p->state = NEW;
    }
    else {
        p->state = READY;
    }
}
