# CPU Scheduling Comparison Suite

<p>
  This project implements and compares four distinct CPU scheduling algorithms in C. 
  By reading a single process list, the simulation provides a side-by-side 
  performance analysis of various operating system scheduling strategies.
</p>

<hr>

## Implemented Algorithms

<h3>1. Round Robin (RR)</h3>
<p>
  A preemptive algorithm that allocates a fixed time quantum to each process. 
  It ensures that all processes receive a share of the CPU in a circular fashion, 
  reducing the risk of starvation.
</p>

<h3>2. IO-Aware First-Come, First-Served (FCFS)</h3>
<p>
  A non-preemptive model that processes tasks in the order they arrive. This 
  specific implementation includes IO-aware logic, simulating how a process 
  relinquishes the CPU when performing I/O operations and re-enters the ready 
  queue upon completion.
</p>



<h3>3. Completely Fair Scheduler (CFS)</h3>
<p>
  The modern Linux kernel scheduler. It utilizes a Red-Black Tree to maintain 
  the timeline of process execution. The scheduler always selects the task with 
  the smallest virtual runtime (vruntime) to ensure maximum fairness.
</p>



<h3>4. Shortest Job First (SJF)</h3>
<p>
  A policy that selects the process with the smallest burst time for the next 
  execution cycle. This approach is mathematically optimal for minimizing 
  average waiting time.
</p>

<hr>

## Technical Flow

<p>
  <b>1. Data Parsing:</b><br>
  The system reads <code>processes.txt</code>. For every entry, it creates:
</p>
<ul>
  <li>A Red-Black Tree node for <b>CFS</b>.</li>
  <li>A queue-based <code>ProcessIO</code> structure for <b>FCFS</b>.</li>
  <li>An array-based structure for <b>SJF</b> and <b>RR</b>.</li>
</ul>

<p>
  <b>2. Execution:</b><br>
  The <code>main</code> function triggers each scheduler sequentially. Each 
  algorithm maintains its own internal state, allowing for a fair comparison of 
  the same input data across different logic models.
</p>

<p>
  <b>3. Metrics Output:</b><br>
  Every scheduler outputs a Gantt chart and a statistics table including:
</p>
<pre>
  Turnaround Time (TAT) = Completion Time - Arrival Time
  Waiting Time (WT)     = Turnaround Time - Burst Time
  Response Time (RT)    = First Execution Time - Arrival Time
</pre>

<hr>

## How to Run

<p>
  1. Prepare a <code>processes.txt</code> file in the root directory:<br>
  &nbsp;&nbsp;&nbsp;&nbsp;Format: <code>[PID] [Arrival_Time] [Burst_Time]</code>
</p>

<p>
  2. Compile the source code:<br>
  &nbsp;&nbsp;&nbsp;&nbsp;<code>gcc -o scheduler_sim main.c</code>
</p>

<p>
  3. Run the executable:<br>
  &nbsp;&nbsp;&nbsp;&nbsp;<code>./scheduler_sim</code>
</p>

<hr>

<p align="center">
  <i>Simulation designed for educational analysis of Operating System scheduling behaviors.</i>
</p>
