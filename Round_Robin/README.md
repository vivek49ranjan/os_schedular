# Dynamic Quantum Round Robin Scheduling Algorithm

## 📌 Overview
This project implements an **enhanced Round Robin (RR) CPU Scheduling algorithm** in **C**, where the **time quantum is dynamically calculated** based on the **harmonic mean of the remaining burst times** of processes currently present in the ready queue.

The approach improves CPU utilization and reduces **average waiting time, turnaround time, and response time** compared to the traditional fixed-time-quantum Round Robin algorithm.

---

## 🧠 Features
- Dynamic time quantum selection
- Linked-list based ready queue
- Supports process arrival times
- Generates Gantt Chart
- Calculates:
  - Completion Time (CT)
  - Turnaround Time (TAT)
  - Waiting Time (WT)
  - Response Time (RT)
- Computes average scheduling metrics

---

## 🛠️ Tech Stack
- **Language:** C  
- **Core Concepts:**  
  - Operating Systems  
  - CPU Scheduling  
  - Round Robin Algorithm  
  - Linked Lists  

---

## 📂 Data Structures
### Process Structure
```c
struct process_rr {
    char Pid[10];
    int Arrival_Time;
    int Burst_Time;
};

