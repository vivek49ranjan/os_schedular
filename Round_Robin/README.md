# Dynamic Quantum Round Robin Scheduling Algorithm

Overview

This project implements an Adaptive Round Robin CPU Scheduling Algorithm for a multicore system, where the time quantum is dynamically calculated using the harmonic mean of the remaining burst times of processes currently present in the ready queue.

Unlike traditional Round Robin scheduling with a fixed time quantum, this approach adapts to workload conditions, improving fairness and reducing waiting and turnaround times.

Key Features

Multicore Round Robin scheduling

Dynamic time quantum using harmonic mean

Linked-list based ready queue

Gantt chart generation for each core

Calculation of CT, TAT, WT, and RT

Prevention of starvation

Efficient handling of short and long processes

## Tech Stack
- Language: C  
- Core Concepts:
  - Operating Systems
  - CPU Scheduling
  - Round Robin Algorithm
  - Linked Lists
  - Harmonic Mean
  - Structures and Arrays

---
