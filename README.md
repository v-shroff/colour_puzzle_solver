
# Colour Sort Puzzle Solver

I was really struggling to play a mobile game where you sort coloured balls into tubes, so I turned it into a small weekend project and built a solver for it.

<img width="1174" height="295" alt="screenshot" src="https://github.com/user-attachments/assets/ca18fea8-752d-4a0d-b236-6fd93d219202" />


This project solves a colour sorting puzzle where balls are arranged in tubes.

Rules:
- You can only move the top ball from a tube
- A ball can only be placed onto:
  - an empty tube, or
  - a tube where the top ball has the same colour

The goal is to fully sort the puzzle so that each tube contains only one color or is empty.

## Solver Approach

The solver uses a weighted A* search algorithm.

It gets within ~10 moves of optimal solutions, but using weighted A* means the runtime is often much lower than a non-weighted approach.

The heuristic I used was pretty simple:
- It checks how many balls in a tube match the bottom structure of that tube
- The heuristic is weighted around 3 times higher than the g score (number of moves to a certain state)

It makes the program faster, but it also means solutions aren't always optimal.

---

## Performance

For a large puzzle configuration (16 tubes, 14 colors):

- Runtime: ~5ms
- Memory usage: ~40MB
- Solutions are typically within ~10 moves of optimal
- Solver explores 40k states

---

## Prototype Comparison

The initial version (`prototype.cpp`) was a quick implementation that used really inefficient data structures.

- It often explored 3M+ states
- Runtime could exceed 60 seconds for large puzzles

I used better hashing and more efficient data structures for the new version.

---

## Build and Run

To build and run the solver:

```bash
make build/solver
./build/solver
```
You can modify `solver.cpp` to test other puzzles, and it will output stats like the number of explored states.

<img width="261" height="75" alt="screenshot_2026-06-06_23-28-40" src="https://github.com/user-attachments/assets/8c9a7c97-87cd-4c8d-b348-60dc2ab64b95" />


## Web Demo

I also made a website to demonstrate the solver:

https://v-shroff.github.io/colour_puzzle_solver/

This version compiles the same C++ solver to WebAssembly using Emscripten and provides a web interface for configuring puzzles by clicking tubes. It also visualizes the solution step-by-step.

The web implementation is located in:
```
src/web.cpp
```
It's essentially the same as the other one, but it includes a few Emscripten bindings to let JS interact with the underlying WASM. 
