#define ST_TRACE_IMPLEMENTATION
#include "st_trace.h"

void test() {
    int* ptr = NULL;
    *ptr = 1;
}

void test2() {
    test();
}

int main() {
    hook_all(); // hook all possible signals (described in [signum-generic.h])
    
    test2(); // sample function that faults

    return 0;
}