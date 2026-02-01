<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Multi-Core CFS Scheduler Simulation</title>
</head>
<body>

<h1> Multi-Core Completely Fair Scheduler (CFS) Simulation</h1>

<p>
This project implements a simulation of the <b>Linux Completely Fair Scheduler (CFS)</b> 
for a <b>multi-core CPU system</b>. It models fair CPU time distribution using 
<b>virtual runtime (vruntime)</b> and a <b>Red-Black Tree</b> to efficiently select the next process to run.
</p>

<hr>

<h2> Features</h2>
<ul>
    <li>Simulation of Linux-style Completely Fair Scheduling</li>
    <li>Multi-core scheduling with separate runqueues per core</li>
    <li>Red-Black Tree based process ordering</li>
    <li>Virtual runtime tracking for fairness</li>
    <li>Weighted scheduling (nice-value style priority)</li>
    <li>Time-slice calculation based on task weight</li>
    <li>Detailed per-core performance metrics</li>
</ul>

<hr>

<h2> Core Scheduling Concept</h2>

<p>
Unlike traditional schedulers that rely on fixed priorities or shortest job times,
<b>CFS tries to divide CPU time as fairly as possible</b> among all runnable processes.
</p>

<p>
Each process maintains a <b>virtual runtime (vruntime)</b>, which represents how much 
processor time it has received, adjusted by its priority weight.
</p>

<p>
The scheduler always runs the process with the <b>smallest vruntime</b>.
</p>

<hr>

<h2> Important Formulas</h2>

<h3>Time Slice Calculation</h3>
<pre>
slice = (SCHED_PERIOD × weight) / (BASE_WEIGHT × number_of_running_tasks)
</pre>

<h3>Virtual Runtime Update</h3>
<pre>
vruntime += execution_time × (BASE_WEIGHT / weight)
</pre>

<p>
Higher weight → slower vruntime growth → process runs more often.
Lower weight → faster vruntime growth → process runs less often.
</p>

<hr>

<h2> Data Structures</h2>

<h3>Process Structure</h3>
<pre>
typedef struct process {
    int pid;
    int arrival_time;
    int burst_time;
    double remaining_time;
    double vruntime;
    double weight;
    double start_time;
    double completion_time;
    state_t state;
} process;
</pre>

<h3>Red-Black Tree Node</h3>
<pre>
typedef struct rb_node {
    double vruntime;
    process *task;
    struct rb_node *left, *right, *parent;
    int color;
} rb_node;
</pre>

<p>
Each core maintains its own <b>Red-Black Tree runqueue</b>, sorted by vruntime.
</p>

<hr>

<h2> Why Red-Black Tree?</h2>

<ul>
    <li>Keeps processes sorted by vruntime</li>
    <li>Allows insertion, deletion, and min extraction in <b>O(log n)</b></li>
    <li>Mimics the actual Linux CFS data structure</li>
</ul>

<hr>

<h2> Scheduling Workflow</h2>

<ol>
    <li>Each CPU core maintains an independent runqueue.</li>
    <li>The process with the <b>minimum vruntime</b> is selected.</li>
    <li>A time slice is calculated based on weight and number of tasks.</li>
    <li>The process executes for its slice (or until completion).</li>
    <li>vruntime is updated to reflect consumed CPU time.</li>
    <li>If unfinished, the process is reinserted into the Red-Black Tree.</li>
    <li>If finished, metrics are calculated and stored.</li>
</ol>

<hr>

<h2> Performance Metrics</h2>

For each completed process:

<ul>
    <li><b>Completion Time (CT)</b></li>
    <li><b>Turnaround Time (TAT)</b> = CT − Arrival Time</li>
    <li><b>Waiting Time (WT)</b> = TAT − Burst Time</li>
    <li><b>Response Time (RT)</b> = Start Time − Arrival Time</li>
</ul>

<hr>

<h2>🖥 Example Output</h2>

<pre>
--- CFS CORE 0 METRICS ---
PID | AT | BT | CT | TAT | WT | RT
  1 |  0 | 10 | 12.0 | 12.0 |  2.0 |  0.0
  2 |  1 |  6 | 14.5 | 13.5 |  7.5 |  3.0
</pre>

<hr>

<h2> Key Concepts Demonstrated</h2>
<ul>
    <li>Fair-share CPU scheduling</li>
    <li>Virtual runtime and proportional fairness</li>
    <li>Red-Black Tree balancing (rotations + recoloring)</li>
    <li>Multi-core scheduling design</li>
    <li>Performance metric analysis</li>
</ul>

<hr>

<h2> Possible Enhancements</h2>
<ul>
    <li>Load balancing between cores</li>
    <li>Support for sleeping and waking tasks</li>
    <li>Dynamic nice values</li>
    <li>Gantt chart visualization</li>
    <li>Comparison with SJF, RR, and Priority scheduling</li>
</ul>

<hr>

<h2> Author</h2>
<p>
Built as part of an Operating Systems scheduling simulation project to understand
how modern kernels manage fair CPU time distribution.
</p>

</body>
</html>
