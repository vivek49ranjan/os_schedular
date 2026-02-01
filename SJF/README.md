<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
</head>
<body>

<h2>Multi-Core Shortest Job First (SJF) CPU Scheduling Simulator</h2>

<h3>Objective</h3>
<p>
The objective of this project is to understand the working of the
<strong>Shortest Job First (SJF)</strong> CPU scheduling algorithm in a
<strong>multi-core environment</strong> and analyze its behavior using
standard scheduling metrics.
</p>

<h3>Description</h3>
<p>
This project implements a <strong>non-preemptive SJF scheduler</strong> in C
for a system with multiple CPU cores.
Each core schedules its assigned processes independently using the
Shortest Job First policy.
</p>
<p>
An <strong>aging mechanism</strong> is implemented to dynamically increase
the priority of processes waiting in the ready queue, thereby reducing
the possibility of starvation.
</p>

<h3>Key Features</h3>
<ul>
    <li>Non-preemptive Shortest Job First scheduling</li>
    <li>Multi-core CPU scheduling simulation</li>
    <li>Static process-to-core assignment using process ID</li>
    <li>Heap-based ready queue for efficient job selection</li>
    <li>Aging support to prevent starvation</li>
    <li>CPU idle time handling</li>
    <li>Per-core scheduling metrics output</li>
</ul>

<h3>Scheduling Behavior</h3>
<ul>
    <li>Each CPU core maintains its own ready queue</li>
    <li>Processes are assigned to cores based on <code>PID % NUM_CORES</code></li>
    <li>Processes enter the ready queue when their arrival time is reached</li>
    <li>Waiting processes gradually increase their priority through aging</li>
    <li>The shortest effective job is selected from the ready queue</li>
    <li>Once a process starts execution, it runs until completion</li>
    <li>The CPU core remains <strong>IDLE</strong> if no process is ready</li>
</ul>

<h3>Data Structures Used</h3>
<ul>
    <li>Process Control Block (PCB) implemented using <code>struct</code></li>
    <li>Separate ready queue per core implemented as a <strong>min-heap</strong></li>
</ul>

<h3>Metrics Calculation</h3>
<ul>
    <li><strong>Turnaround Time (TAT)</strong> = Completion Time − Arrival Time</li>
    <li><strong>Waiting Time (WT)</strong> = Turnaround Time − Burst Time</li>
</ul>

<h3>How to Run</h3>
<ol>
    <li>Initialize processes in the <code>ProcessSJF</code> array</li>
    <li>Ensure <code>NUM_CORES</code> is defined</li>
    <li>Call <code>scheduleSJF(processes, n)</code></li>
    <li>Observe per-core scheduling results printed on the terminal</li>
</ol>

<h3>Notes</h3>
<p>
Aging is applied by incrementing the age of all processes waiting in the
ready queue before each scheduling decision.
This ensures that long-waiting processes gradually gain higher priority
and starvation is reduced.
</p>

<h3>Conclusion</h3>
<p>
This project provides a clear and practical implementation of
non-preemptive Shortest Job First scheduling in a multi-core system.
With aging support and per-core scheduling metrics, it serves as an
effective learning tool for operating system concepts, laboratory
experiments, and interview preparation.
</p>

</body>
</html>
