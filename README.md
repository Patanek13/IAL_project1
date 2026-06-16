# IAL Project 1 — Data Structures and Expression Processing in C

This repository contains my solutions for selected assignments from the **IAL (Algorithms)** course at **FIT VUT Brno**.  
The project focuses on low-level C programming, manual memory management, and implementation of core data structures and algorithms.

## Project Highlights

- **Dynamic stack implementation** (`c202`) with robust boundary checks
- **Infix-to-postfix conversion and expression evaluation** (`c204`) using stack-based processing
- **Doubly linked list ADT implementation** (`c206`) with full list operation support
- **Extended packet queue simulation** (`c206-ext`) for priority-based packet handling

## Tech Stack

- **Language:** C (C11)
- **Compiler:** GCC
- **Build tooling:** Makefile

## Repository Structure

```
.
├── c202.c / c202.h       # Stack ADT (array-based)
├── c204.c / c204.h       # Infix → postfix conversion + expression evaluation
├── c206.c / c206.h       # Doubly linked list ADT
├── c206-ext.c / c206-ext.h # QoS packet queue extension built on c206
└── Makefile
```

## Build

```bash
make all
```

> Note: The included `Makefile` is configured for the original course test harness.  
> In this repository snapshot, some referenced test files are not present.

## What This Project Demonstrates

- Solid fundamentals in **data structures** (stack, doubly linked list)
- Practical understanding of **algorithmic expression parsing/evaluation**
- Ability to write **memory-safe C code** with explicit allocation/deallocation
- Experience with **modular C design** using header/source separation

## Author

**Patrik Lošťák**  
Student project for portfolio and technical demonstration.
