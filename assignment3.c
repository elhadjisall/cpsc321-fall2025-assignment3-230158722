#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

// Static arrays as specified in the assignment
static const int N = 5;
static const char* names[] = {"P1","P2","P3","P4","P5"}; // the process identifiers
static const int arrival[] = { 0, 1, 2, 3, 4 }; // the arrival time of each process (in time units).
static const int burst[] = {10, 5, 8, 6, 3 }; // the burst time (execution time in time units) of each process.

// Process structure to hold process information
typedef struct {
    char name[10];
    int arrival_time;
    int burst_time;
    int waiting_time;
    int turnaround_time;
    int cpu_id;
    int completed;
} Process;

// Global variables
Process processes[N];
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
int current_time = 0;
int completed_processes = 0;

// Function prototypes
void* cpu_worker(void* arg);
Process* find_shortest_job();
void execute_process(Process* process, int cpu_id);

int main() {
    printf("CPSC 321 Assignment 3 - Multi-CPU SJF Scheduling\n");
    printf("Initializing %d processes...\n", N);
    
    // Initialize process structures
    for (int i = 0; i < N; i++) {
        strcpy(processes[i].name, names[i]);
        processes[i].arrival_time = arrival[i];
        processes[i].burst_time = burst[i];
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].cpu_id = -1;
        processes[i].completed = 0;
    }
    
    printf("Process initialization complete.\n");
    return 0;
}
