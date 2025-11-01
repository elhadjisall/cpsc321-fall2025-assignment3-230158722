# CPSC 321 - Assignment 3 (Multi-CPU SJF Scheduling)

**Name:** El Sall  
**Student Number:** 230158722  
**Email:** esall@unbc.ca  
**GitHub Repository:** https://github.com/elhadjisall/cpsc321-fall2025-assignment3-230158722

## Build:

```bash
gcc -o assignment3 assignment3.c -pthread -Wall -Wextra
```

## Run:

```bash
./assignment3
```

## Clean:

```bash
rm -f assignment3
```

## Description

This assignment implements a multi-CPU scheduling system using the Shortest Job First (SJF) algorithm with synchronization primitives (mutex) to prevent race conditions. The program uses two worker threads to represent two CPU cores, each selecting jobs from a shared ready queue using non-preemptive SJF scheduling.

