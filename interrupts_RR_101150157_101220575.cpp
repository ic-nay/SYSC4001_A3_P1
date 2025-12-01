#include "interrupts_101150157_101220575.hpp"

int main() {
    for (int i = 1; i <= 20; i ++){
        loadInputFile("input_files/case"+std::to_string(i)+".txt");
        ofstream("output_files/execution"+std::to_string(i)+".txt").close();

        int time = 0;
        queue<PCB*> ready;

        const int QUANTUM = 100;

        while (true) {
            // Admit arrivals
            for (auto &p : pcb_table) {
                if (p.arrival == time && p.state == "new") {
                    if (allocateMemory(p)) {
                        logTransition(time, p.pid, i,"new", "ready");
                        p.state = "ready";
                        ready.push(&p);
                        printMemoryState(time, i);
                    }
                }
            }

            if (ready.empty()) {
                time++;
                if (time > 100000) break;
                continue;
            }

            PCB *running = ready.front();
            ready.pop();

            logTransition(time, running->pid, i, running->state, "running");
            running->state = "running";

            int runTime = min(QUANTUM, running->remaining_cpu);
            running->remaining_cpu -= runTime;
            time += runTime;

            // If finished
            if (running->remaining_cpu <= 0) {
                logTransition(time, running->pid, i, "running", "terminated");
                running->state = "terminated";
                freeMemory(running->partition);
                printMemoryState(time, i);
                continue;
            }

            // I/O event?
            if ((running->total_cpu - running->remaining_cpu) % running->io_freq == 0) {
                logTransition(time, running->pid, i, "running", "waiting");
                running->state = "waiting";
                time += running->io_dur;
                logTransition(time, running->pid, i, "waiting", "ready");
            } else {
                logTransition(time, running->pid, i, "running", "ready");
            }

            running->state = "ready";
            ready.push(running);
        }

        cout << "RR simulation " + std::to_string(i) + " finished.\n";
    }
    return 0;
}
