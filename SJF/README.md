<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    
</head>
<body>

<h2>Shortest Job First (SJF) CPU Scheduling Simulator</h2>

<h3>Objective</h3>
<p>
The objective of this project is to understand the working of the
<strong>Shortest Job First (SJF)</strong> CPU scheduling algorithm and analyze
its performance using scheduling metrics and a Gantt chart.
</p>

<h3>Description</h3>
<p>
This project implements a <strong>non-preemptive SJF scheduler</strong> in C.
Processes are selected based on the shortest burst time.
An <strong>aging mechanism</strong> is included to reduce starvation of longer processes.
</p>
<p>
The scheduler simulates CPU execution over time and records the execution
order for visualization.
</p>

<h3>Key Features</h3>
<ul>
    <li>Non-preemptive Shortest Job First scheduling</li>
    <li>Aging support to avoid starvation</li>
    <li>Heap-based ready queue for efficient process selection</li>
    <li>CPU idle time handling</li>
    <li>Gantt chart visualization</li>
    <li>Calculation of CT, TAT, WT, and RT</li>
</ul>

<h3>Scheduling Behavior</h3>
<ul>
    <li>Processes are admitted based on arrival time</li>
    <li>The shortest job is selected from the ready queue</li>
    <li>Once a process starts execution, it runs until completion</li>
    <li>Aging increases the priority of waiting processes</li>
    <li>The CPU remains <strong>IDLE</strong> if no process is ready</li>
</ul>

<h3>Data Structures Used</h3>
<ul>
    <li>Process Control Block (PCB) implemented using <code>struct</code></li>
    <li>Ready queue implemented as a <strong>min-heap</strong></li>
    <li>Gantt chart stored using a struct array</li>
</ul>

<h3>Metrics Calculation</h3>
<ul>
    <li><strong>Response Time (RT)</strong> = Start Time − Arrival Time</li>
    <li><strong>Turnaround Time (TAT)</strong> = Completion Time − Arrival Time</li>
    <li><strong>Waiting Time (WT)</strong> = Turnaround Time − Burst Time</li>
</ul>

<h3>How to Run</h3>
<ol>
    <li>Initialize processes in the <code>Process</code> array</li>
    <li>Call <code>scheduleSJF(processes, n)</code></li>
    <li>Observe the Gantt chart and average metrics printed on the terminal</li>
</ol>

<h3>Conclusion</h3>
<p>
This SJF scheduler provides a clear and practical implementation of
non-preemptive scheduling with aging, making it useful for learning
operating system scheduling concepts and interview preparation.
</p>

</body>
</html>

