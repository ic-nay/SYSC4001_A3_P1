#!/bin/bash

g++ -std=c++17 interrupts_EP_101150157_101220575.cpp -o ep
g++ -std=c++17 interrupts_RR_101150157_101220575.cpp -o rr
g++ -std=c++17 interrupts_EP_RR_101150157_101220575.cpp -o ep_rr

echo "Build complete."
