# Completely Fair Scheduler (CFS) Simulation

This project is a C implementation of the Completely Fair Scheduler (CFS), the default process scheduler used in the Linux kernel. It utilizes a Red-Black Tree to manage tasks and ensure a fair distribution of CPU time.

---

## Technical Overview

The CFS algorithm does not use traditional time slices. Instead, it assigns a proportion of the CPU based on the weight of each task. It maintains balance by tracking the **virtual runtime** ($vruntime$) of each process.

### Key Components

* **vruntime**: A measure of the CPU time a task has received. The scheduler always chooses the task with the smallest $vruntime$.
* **Red-Black Tree**: A self-balancing binary search tree used to store active tasks. This ensures $O(\log N)$ time complexity for insertions and deletions.
* **Weight-based Allocation**: Tasks are assigned weights based on their priority. Higher weights result in $vruntime$ increasing more slowly.



---

## Core Formulas

The scheduler uses the following mathematical logic to determine execution:

1.  **Time Slice Calculation**:
    $$\text{slice} = \frac{\text{SCHED\_PERIOD} \times \text{weight}}{\text{Total active weight}}$$

2.  **Virtual Runtime Update**:
    $$\Delta vruntime = \text{actual\_time} \times \frac{\text{BASE\_WEIGHT}}{\text{process\_weight}}$$

---

## Logic Flow

1.  **Extraction**: The process with the minimum $vruntime$ is extracted from the Red-Black Tree.
2.  **Execution**: The process runs for its calculated time slice.
3.  **Update**: The $vruntime$ is updated based on the actual duration it ran.
4.  **Re-insertion**: If the process has remaining burst time, it is re-inserted into the tree. The tree rebalances itself via rotations.



---

## Execution Results

Upon completion, the simulation provides:

### 1. Gantt Chart
A visual representation of how processes occupied the CPU over time.

### 2. Performance Metrics
A detailed table containing:
* **TAT (Turnaround Time)**: Completion Time - Arrival Time
* **WT (Waiting Time)**: Turnaround Time - Burst Time
* **RT (Response Time)**: First Start Time - Arrival Time

---

## Compilation and Usage

Compile the source code using `gcc`:

```bash
gcc cfs_scheduler.c -o cfs_sim
./cfs_sim
