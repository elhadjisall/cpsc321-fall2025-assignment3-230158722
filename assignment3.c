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
int current_time = 0; // global reference time (simple simulation)
int completed_processes = 0;
int cpu_time[2] = {0, 0}; // per-CPU simulated clocks

// Function prototypes
void* cpu_worker(void* arg);
Process* find_shortest_job(int now);
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

    // Launch two CPU worker threads
    pthread_t threads[2];
    int cpu_ids[2] = {0, 1};
    pthread_create(&threads[0], NULL, cpu_worker, &cpu_ids[0]);
    pthread_create(&threads[1], NULL, cpu_worker, &cpu_ids[1]);

    // Wait for both CPUs to finish
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    // Print per-process results and averages
    double total_wait = 0.0, total_turn = 0.0;
    for (int i = 0; i < N; i++) {
        printf("Process: %s Arrival: %d Burst: %d CPU: %d Waiting Time: %d Turnaround Time: %d\n",
               processes[i].name,
               processes[i].arrival_time,
               processes[i].burst_time,
               processes[i].cpu_id,
               processes[i].waiting_time,
               processes[i].turnaround_time);
        total_wait += processes[i].waiting_time;
        total_turn += processes[i].turnaround_time;
    }
    printf("Average waiting time = %.2f\n", total_wait / N);
    printf("Average turnaround time = %.2f\n", total_turn / N);
    return 0;
}

// Pick the shortest job among those that have arrived (arrival <= now) and not completed
Process* find_shortest_job(int now) {
    Process* best = NULL;
    for (int i = 0; i < N; i++) {
        if (processes[i].completed == 0 && processes[i].arrival_time <= now) {
            if (best == NULL || processes[i].burst_time < best->burst_time) {
                best = &processes[i];
            }
        }
    }
    return best;
}

// Simulate execution: compute waiting/turnaround using per-CPU time and sleep to emulate work
void execute_process(Process* process, int cpu_id) {
    int start_time = cpu_time[cpu_id] < process->arrival_time ? process->arrival_time : cpu_time[cpu_id];
    process->waiting_time = start_time - process->arrival_time;
    process->cpu_id = cpu_id;

    // Simulate execution time; scale to milliseconds for quick runs
    usleep(process->burst_time * 1000);

    int finish_time = start_time + process->burst_time;
    process->turnaround_time = finish_time - process->arrival_time;

    // Advance this CPU's clock
    cpu_time[cpu_id] = finish_time;
}

void* cpu_worker(void* arg) {
    int cpu_id = *(int*)arg;
    while (1) {
        pthread_mutex_lock(&queue_mutex);

        if (completed_processes >= N) {
            pthread_mutex_unlock(&queue_mutex);
            break;
        }

        // Decide current reference time: earliest of both CPUs' clocks
        int ref_now = cpu_time[0] < cpu_time[1] ? cpu_time[0] : cpu_time[1];
        if (ref_now < cpu_time[cpu_id]) ref_now = cpu_time[cpu_id];

        Process* p = find_shortest_job(ref_now);
        if (p == NULL) {
            // Nothing eligible yet; release lock and wait a bit
            pthread_mutex_unlock(&queue_mutex);
            usleep(1000);
            continue;
        }

        // Mark as running to avoid double-pick
        p->completed = 2; // 2 = running
        pthread_mutex_unlock(&queue_mutex);

        execute_process(p, cpu_id);

        pthread_mutex_lock(&queue_mutex);
        p->completed = 1; // 1 = done
        completed_processes++;
        // Update a coarse global time reference (optional)
        if (cpu_time[cpu_id] > current_time) current_time = cpu_time[cpu_id];
        pthread_mutex_unlock(&queue_mutex);
    }
    return NULL;
}
