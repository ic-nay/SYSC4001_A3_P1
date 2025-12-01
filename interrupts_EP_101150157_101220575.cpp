#include "interrupts_101150157_101220575.hpp"

int main() {
    for (int i = 1; i <= 20; i ++){
        loadInputFile("input_files/case"+std::to_string(i)+".txt");
        ofstream("output_files/execution"+std::to_string(i)+".txt").close(); // clear file

        int time = 0;
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
                if (time > 100000) break;
                continue;
            }

            // Sort by priority (highest = runs first)
            sort(ready.begin(), ready.end(),
                [](PCB* a, PCB* b) { return a->priority > b->priority; });

            PCB *running = ready.front();
            ready.erase(ready.begin());

            logTransition(time, running->pid, i, running->state, "running");
            running->state = "running";

            // Run to completion OR I/O
            int nextIO = running->io_freq;
            int runTime = min(nextIO, running->remaining_cpu);

            time += runTime;
            running->remaining_cpu -= runTime;

            if (running->remaining_cpu <= 0) {
                logTransition(time, running->pid, i, "running", "terminated");
                running->state = "terminated";
                freeMemory(running->partition);
                printMemoryState(time, i);
                continue;
            }

            // I/O
            logTransition(time, running->pid, i, "running", "waiting");
            running->state = "waiting";
            time += running->io_dur;

            // I/O complete
            logTransition(time, running->pid, i, "waiting", "ready");
            running->state = "ready";

            ready.push_back(running);
        }

        cout << "EP simulation" + std::to_string(i) + "finished.\n";
    }
    return 0;
}
