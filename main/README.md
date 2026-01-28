
# CPU Scheduling Algorithm Comparison Suite

<p>
  This project provides a comprehensive simulation of four major CPU scheduling algorithms. 
  It reads process data from a local file and evaluates how each algorithm manages 
  resources, fairness, and execution flow.
</p>

---

## 📋 Implemented Algorithms

### 1. Round Robin (RR)
<p>
  A preemptive scheduling algorithm where each process is assigned a fixed time slot 
  in a cyclic order. It is designed to provide high interactivity.
</p>

### 2. IO-Aware FCFS
<p>
  A <b>First-Come, First-Served</b> model that simulates real-world I/O blocking. 
  When a process hits an I/O trigger, it yields the CPU and returns later, 
  allowing other processes to utilize the idle cycles.
</p>



### 3. Completely Fair Scheduler (CFS)
<p>
  The modern Linux kernel scheduler. It uses a <b>Red-Black Tree</b> to track 
  virtual runtime (<i>vruntime</i>), ensuring that the process with the 
  least execution time is always serviced next.
</p>



### 4. Shortest Job First (SJF)
<p>
  A policy that selects the process with the smallest burst time to execute next, 
  aiming to minimize the average waiting time across all tasks.
</p>

---

## ⚙️ How it Works

<p>
  <b>Input Handling:</b><br>
  The program opens <code>processes.txt</code> and parses data into three distinct 
  formats simultaneously:
</p>

<ul>
  <li><b>Red-Black Tree nodes</b> for the CFS scheduler.</li>
  <li><b>Linked Lists/Queues</b> for the IO-Aware FCFS.</li>
  <li><b>Arrays/Structures</b> for SJF and Round Robin.</li>
</ul>

<p>
  <b>Metrics Calculation:</b><br>
  After each execution, the simulation calculates the following for every PID:
</p>

<pre>
Turnaround Time (TAT) = Completion Time - Arrival Time
Waiting Time (WT)     = Turnaround Time - Burst Time
Response Time (RT)    = First Start Time - Arrival Time
</pre>

---

## 🚀 Usage Instructions

<p>
  1. Create a file named <code>processes.txt</code> in the following format:<br>
  &nbsp;&nbsp;&nbsp;<code>[PID] [Arrival_Time] [Burst_Time]</code>
</p>

<p>
  2. Compile the suite:<br>
  &nbsp;&nbsp;&nbsp;<code>gcc -o scheduler_sim main.c</code>
</p>

<p>
  3. Run the simulation:<br>
  &nbsp;&nbsp;&nbsp;<code>./scheduler_sim</code>
</p>

---

<p align="center">
  <i>Developed to demonstrate the trade-offs between scheduling fairness and throughput.</i>
</p>
