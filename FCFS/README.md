# FCFS CPU Scheduling with I/O Awareness (Multi-Core)

## 1. Aim
To implement and simulate the **First Come First Serve (FCFS) CPU scheduling algorithm** with **I/O handling** and **multi-core support** using the C programming language.

---

## 2. Objective
- To understand FCFS scheduling in a multi-core environment  
- To simulate process execution with I/O blocking  
- To calculate scheduling parameters such as:
  - Start Time
  - Completion Time
  - Turnaround Time
  - Waiting Time

---

## 3. Description of the Problem
In real operating systems, processes may require **I/O operations** during execution, causing them to temporarily leave the CPU.  
This project simulates an **I/O-aware FCFS scheduler** where:
- Processes are distributed across multiple CPU cores
- Each core schedules processes independently using FCFS
- Processes can move between **Ready** and **I/O** states
- Scheduling metrics are calculated after completion

---

## 4. Scheduling Algorithm Used
**First Come First Serve (FCFS)**

FCFS is a **non-preemptive scheduling algorithm** where:
- Processes are executed in the order of arrival
- Once a process gets the CPU, it runs until completion or I/O request

---

## 5. Data Structures Used

### 5.1 Process Structure (`ProcessIO`)
Each process contains CPU and I/O related information.

| Parameter | Description |
|--------|-------------|
| PID | Process ID |
| Arrival Time | Time at which process arrives |
| Burst Time | Total CPU execution time |
| Remaining Time | Remaining CPU time |
| Executed Time | CPU time already executed |
| Start Time | First CPU allocation time |
| Completion Time | Time when execution completes |
| I/O Start | Execution point where I/O begins |
| I/O Duration | Total I/O time |
| I/O Remaining | Remaining I/O time |

---

### 5.2 Queue Structure
A linked-list based queue is used to implement:
- Arrival Queue
- Ready Queue (per core)
- I/O Queue (per core)

---

## 6. Algorithm

### Step 1:
Initialize ready queues for each CPU core.

### Step 2:
Distribute processes from the arrival queue to cores using:

### Step 3:
For each core, repeat until all processes finish:
- Update I/O queue
- Move completed I/O processes back to ready queue
- Schedule process from ready queue if CPU is idle
- Execute the running process

### Step 4:
If process reaches I/O start time:
- Move it to I/O queue

### Step 5:
If process completes execution:
- Calculate CT, TAT, and WT
- Display results

---

## 7. Output Format

For each CPU core, the following table is printed:
Where:
- AT = Arrival Time
- BT = Burst Time
- ST = Start Time
- CT = Completion Time
- TAT = CT − AT
- WT = TAT − BT

---

## 8. Assumptions
- FCFS scheduling is strictly followed
- Only one I/O operation per process
- Arrival queue is pre-initialized
- Each core executes one process at a time

---

## 9. Advantages
- Simple and easy to implement
- Demonstrates real-world I/O blocking
- Useful for understanding OS scheduling concepts

---

## 10. Limitations
- Poor response time for long processes
- No priority handling
- No preemption

---

## 11. Conclusion
This project successfully demonstrates an **I/O-aware FCFS scheduling algorithm** in a **multi-core environment**.  
It helps in understanding how processes interact with CPU and I/O devices in real operating systems.

---
