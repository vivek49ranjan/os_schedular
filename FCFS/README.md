# FCFS I/O-Aware CPU Scheduling Simulator

## Objective

The objective of this project is to **mimic real operating system CPU scheduling behavior** and to **understand the working of the First Come First Serve (FCFS) scheduling algorithm** with basic **I/O awareness**. The implementation helps visualize how processes move between **arrival**, **ready**, **CPU**, and **I/O queues** and how scheduling metrics are calculated.

---

## Description

This project implements an **FCFS (First Come First Serve) scheduler** using the C programming language. Unlike a simple FCFS implementation, this version is **I/O-aware**, meaning that a running process can get blocked due to an I/O operation and later rejoin the ready queue after I/O completion.

The scheduler is designed to be **time-driven (streaming)**, where the scheduling function is called repeatedly to simulate each unit of CPU time, similar to how a real operating system clock works.

---

## Key Features

* FCFS (non-preemptive) CPU scheduling
* Arrival-time sorted process admission
* Separate **Ready Queue** and **I/O Queue**
* I/O blocking and I/O completion handling
* Gantt chart visualization in terminal
* Calculation of scheduling metrics:

  * Completion Time (CT)
  * Turnaround Time (TAT)
  * Waiting Time (WT)
  * Response Time (RT)

---

## Scheduling Behavior

* Processes are selected in **first-come-first-serve order** based on arrival time.
* Once a process gets the CPU, it executes **until completion or I/O blocking**.
* If a process requests I/O, it is moved to the I/O queue.
* After I/O completion, the process returns to the ready queue.
* If no process is ready, the CPU remains **IDLE**.

---

## Data Structures Used

* **Process Control Block (PCB)** implemented using a `struct`
* **Queues** implemented using singly linked lists
* Arrival queue is maintained using **sorted insertion by arrival time**

---

## Static Variables Usage

The FCFS scheduler uses `static` variables for:

* Global simulation clock
* Ready and I/O queues
* Currently running process

This allows the scheduler to **preserve state across multiple function calls**, accurately simulating an operating system scheduler that advances with time.

---

## Metrics Calculation

* **Response Time (RT)** = Start Time − Arrival Time
* **Turnaround Time (TAT)** = Completion Time − Arrival Time
* **Waiting Time (WT)** = Turnaround Time − Burst Time

---

## How to Run

1. Initialize processes and insert them into the arrival queue using `insert_by_arrival()`.
2. Call `fcfs_io_aware()` repeatedly inside a loop until all processes complete.
3. Observe the Gantt chart and scheduling metrics printed on the terminal.

---

## Learning Outcomes

* Clear understanding of FCFS scheduling
* Practical exposure to OS concepts like queues, I/O blocking, and CPU idle time
* Experience in designing time-driven simulations
* Improved understanding of scheduling performance metrics

---

## Conclusion

This FCFS I/O-aware scheduler provides a realistic and educational simulation of CPU scheduling in operating systems. It bridges the gap between theoretical concepts and practical implementation, making it a valuable learning tool for OS and systems programming courses.

