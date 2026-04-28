# st_trace.h

A lightweight, C library for generating stack traces and handling fatal signals on Linux x86_64. 

This library uses raw **RBP (Base Pointer)** walking via FASM assembly to climb the stack

For the stack walker to work correctly, you **must** pass specific flags to the compiler to preserve the "metadata" required for tracing:

| Flag | Purpose |
| :--- | :--- |
| `-fno-omit-frame-pointer` | Prevents the compiler from using the RBP register for optimization. **Critical for the walker.** |
| `-rdynamic` | Tells the linker to add all symbols to the dynamic symbol table so `dladdr` can find them. |
| `-ldl` | Links the dynamic loader library. **Must be placed at the end of your compilation command.** |
| `-O0` | (Recommended) Disables inlining and optimizations that might hide functions from the stack. |

--

A sample makefile for compilation can be found in the repo root