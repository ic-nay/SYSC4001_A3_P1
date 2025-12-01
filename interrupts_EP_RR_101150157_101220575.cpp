#include "interrupts_101150157_101220575.hpp"

int main() {
    for (int i = 1; i <= 20; i ++){
        loadInputFile("input_files/case"+std::to_string(i)+".txt");
        ofstream("output_files/execution"+std::to_string(i)+".txt").close(); // clear file

        int time = 0;
        const int QUANTUM = 100;

        vector<PCB*> ready;

        while (true) {
            // Admit arrivals
            for (auto &p : pcb_table) {
                if (p.arrival == time && p.state == "new") {
                    if (allocateMemory(p)) {
                        logTransition(time, p.pid, i, "new", "ready");
                        p.state = "ready";
                        ready.push_back(&p);
                        printMemoryState(time, i);
                    }
                }
            }

            if (ready.empty()) {
                time++;
                if (time > 50000) break;
                continue;
            }

            // Sort by priority first, then by PID
            sort(ready.begin(), ready.end(),
                [](PCB* a, PCB* b) {
                    return (a->priority > b->priority);
                });

            PCB* running = ready.front();
            ready.erase(ready.begin());

            logTransition(time, running->pid, i, running->state, "running");
            running->state = "running";

            int runTime = min(running->remaining_cpu, QUANTUM);
            running->remaining_cpu -= runTime;
            time += runTime;

            // Finished?
            if (running->remaining_cpu <= 0) {
                logTransition(time, running->pid, i, "running", "terminated");
                running->state = "terminated";
                freeMemory(running->partition);
                printMemoryState(time, i);
                continue;
            }

            // I/O?
            if ((running->total_cpu - running->remaining_cpu) % running->io_freq == 0) {
                logTransition(time, running->pid, i, "running", "waiting");
                running->state = "waiting";
                time += running->io_dur;
                logTransition(time, running->pid, i, "waiting", "ready");
            } else {
                logTransition(time, running->pid, i, "running", "ready");
            }

            running->state = "ready";
            ready.push_back(running);
        }

        cout << "EP+RR simulation " + std::to_string(i) + " finished.\n";
    }
    return 0;
}
