/*
 * To use this #define ONLY ONCE `ST_TRACE_IMPLEMENTATION` like:
 *
 * #define ST_TRACE_IMPLEMENTATION
 * #include "st_trace.h"
 *
 * -------------------------------------------------
 *
 * Required flags in compilation:
 * -fno-omit-frame-pointer | keep stack pointers intact
 * -rdynamic               | tells linker to add symbols
 * -ldl                    | provides `dladdr` (add this in the last step of compilation [linking])
 * RECOMMENDED:
 * -O0                     | no optimizations
 * ------------------------------------------------
 * 
 * include ds_array.h for dynamic arrays
 * and include stack.o (compiled from stack.asm with fasm) for getting rbp
 * 
 * example Makefile is in the repo root
 * 
 * ------------------------------------------------
 * 
 * Stack is from the oldest to the newest
 * 
 * A known bug is that the function in which the signal is created wont be showed
 * 
 * ------------------------------------------------
 * 
 * made for fun and learning by @tomasmo-dev
 * 
*/
#ifndef ST_TRACE_H
#define ST_TRACE_H

#define _GNU_SOURCE

#include <stdio.h>
#include <inttypes.h>
#include <dlfcn.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>

#include "ds_array.h"

typedef struct stack_frame {
    struct stack_frame* next_rbp;
    void* return_address;
} stack_frame_t;

extern stack_frame_t* get_rbp();

uintptr_t* __walk_stack();

void __st_print_stack_trace(size_t index, uintptr_t address);
void __st_generate_and_print_stack_trace();

void __st_handle_fault(int sig);

void handle_interactive();
void handle_illegal_instr();
void handle_abnorm_termination();
void handle_err_arithm_oper();
void handle_segfault();
void handle_term_req();
void handle_hangup();
void handle_quit();
void handle_breakpoint();
void handle_kill();
void handle_broken_pipe();
void handle_alarm();

void hook_all();
void hook_fatal_error();


#endif // ST_TRACE_H

#ifdef ST_TRACE_IMPLEMENTATION

uintptr_t* __walk_stack() {
    stack_frame_t* frame = get_rbp();

    uintptr_t* call_stack = NULL;

    void* current_rip;
    __asm__ ("lea 0(%%rip), %0" : "=r"(current_rip));
    ds_push(call_stack, (uintptr_t)current_rip);

    while (frame != NULL)
    {
        //printf("Called from: %p\n", frame->return_address);
        ds_push(call_stack, (uintptr_t)frame->return_address);

        frame = frame->next_rbp;

        if ((uintptr_t)frame < 0x1000) break;
    }

    ds_reverse(call_stack);
    
    return call_stack;
}

void __st_print_stack_trace(size_t index, uintptr_t address) {
    Dl_info info;

    if (dladdr((void*)address, &info) && info.dli_sname) {
        printf("[%zu] %s (in %s) [Address: %p]\n", 
               index, 
               info.dli_sname, // Function name
               info.dli_fname, // Library/Executable name
               (void*)address);
    } else {
        if (index == 0) {
            printf("[%zu] Unknown Function {possibly [_start]} [Address: %p]\n", index, (void*)address);
        } else {
            printf("[%zu] Unknown Function [Address: %p]\n", index, (void*)address);
        }
    }
}

void __st_generate_and_print_stack_trace() {

    uintptr_t* stack = __walk_stack();

    for(size_t i = 0; i < DS_COUNT(stack); i++) {
        // printf("[%ld] Address: 0x%016" PRIxPTR"\n", i, stack[i]);
        __st_print_stack_trace(i, stack[i]);
    }

    ds_free(stack);
}

void __st_handle_fault(int sig) {
    printf("------- CRASH DETECTED (signal: [%d]) -------\n", sig);

    __st_generate_and_print_stack_trace();

    printf("----- END OF STACK TRACE (signal: [%d]) -----\n", sig);

    exit(sig);
}

// Signal handlers

void handle_interactive() {
    signal(SIGINT, __st_handle_fault);
}

void handle_illegal_instr() {
    signal(SIGILL, __st_handle_fault);
}

void handle_abnorm_termination() {
    signal(SIGABRT, __st_handle_fault);
}

void handle_err_arithm_oper() {
    signal(SIGFPE, __st_handle_fault);
}

void handle_segfault() {
    signal(SIGSEGV, __st_handle_fault);
}

void handle_term_req() {
    signal(SIGTERM, __st_handle_fault);
}

void handle_hangup() {
    signal(SIGHUP, __st_handle_fault);
}

void handle_quit() {
    signal(SIGQUIT, __st_handle_fault);
}

void handle_breakpoint() {
    signal(SIGTRAP, __st_handle_fault);
}

void handle_kill() {
    signal(SIGKILL, __st_handle_fault);
}

void handle_broken_pipe() {
    signal(SIGPIPE, __st_handle_fault);
}

void handle_alarm() {
    signal(SIGALRM, __st_handle_fault);
}

void hook_all() {
    handle_interactive();
    handle_illegal_instr();
    handle_abnorm_termination();
    handle_err_arithm_oper();
    handle_segfault();
    handle_term_req();
    handle_hangup();
    handle_quit();
    handle_breakpoint();
    handle_kill();
    handle_broken_pipe();
    handle_alarm();
}

void hook_fatal_error() {
    handle_err_arithm_oper();
    handle_segfault();
    handle_illegal_instr();
    handle_abnorm_termination();
}

// Signal handlers

#endif // ST_TRACE_IMPLEMENTATION