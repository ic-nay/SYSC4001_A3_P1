#ifndef INTERRUPTS_101150157_101220575_HPP
#define INTERRUPTS_101150157_101220575_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

// ---------------- MEMORY PARTITION STRUCT ----------------
struct Partition {
    unsigned int number;
    unsigned int size;
    string code; // "free", "init", or process name
};

// ---------------- PCB STRUCT ----------------
struct PCB {
    int pid;
    int mem_required;
    int arrival;
    int total_cpu;
    int remaining_cpu;
    int io_freq;
    int io_dur;

    int partition; // -1 = not loaded
    string state;  // new, ready, running, waiting, terminated
    int priority;  // EP scheduler
};

// ---------------- GLOBAL TABLES ----------------
extern vector<PCB> pcb_table;
extern Partition mem_table[6];

// ---------------- FUNCTION DECLARATIONS ----------------
void loadInputFile(string filename);
bool allocateMemory(PCB &p);
void freeMemory(int partition);
void logTransition(int time, int pid, int iteration, string oldState, string newState);
void printMemoryState(int time, int iteration);

// ---------------- HELPER IMPLEMENTATIONS ----------------

vector<PCB> pcb_table;

Partition mem_table[6] = {
    {1, 40, "free"},
    {2, 25, "free"},
    {3, 15, "free"},
    {4, 10, "free"},
    {5, 8,  "free"},
    {6, 2,  "free"}
};

void loadInputFile(string filename) {
    ifstream f(filename);
    if (!f) {
        cerr << "Error opening input file.\n";
        exit(1);
    }

    pcb_table.clear();
    int pid, mem, arr, cpu, freq, dur;

    while (f >> pid >> mem >> arr >> cpu >> freq >> dur) {
        PCB p;
        p.pid = pid;
        p.mem_required = mem;
        p.arrival = arr;
        p.total_cpu = cpu;
        p.remaining_cpu = cpu;
        p.io_freq = freq;
        p.io_dur = dur;
        p.partition = -1;
        p.state = "new";
        p.priority = rand() % 5; // Random priority for EP schedulers
        pcb_table.push_back(p);
    }
}

bool allocateMemory(PCB &p) {
    for (int i = 0; i < 6; i++) {
        if (mem_table[i].code == "free" && mem_table[i].size >= p.mem_required) {
            mem_table[i].code = "P" + to_string(p.pid);
            p.partition = mem_table[i].number;
            return true;
        }
    }
    return false;
}

void freeMemory(int partition) {
    for (int i = 0; i < 6; i++) {
        if (mem_table[i].number == partition) {
            mem_table[i].code = "free";
        }
    }
}

void logTransition(int time, int pid, int iteration, string oldState, string newState) {
    ofstream out("output_files/execution"+std::to_string(iteration)+".txt", ios::app);
    out << time << "  " << pid << "  " << oldState << "  " << newState << "\n";
}

void printMemoryState(int time, int iteration) {
    ofstream out("output_files/memory"+std::to_string(iteration)+".txt", ios::app);
    int used = 0, free = 0;

    out << "TIME " << time << " ms\n";

    for (int i = 0; i < 6; i++) {
        if (mem_table[i].code == "free")
            free += mem_table[i].size;
        else
            used += mem_table[i].size;

        out << "Partition " << mem_table[i].number
            << " (" << mem_table[i].size << "MB): "
            << mem_table[i].code << "\n";
    }

    out << "Total Used: " << used << " MB\n";
    out << "Total Free: " << free << " MB\n\n";
}

#endif
