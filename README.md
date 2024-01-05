# 15-Puzzle-Solver
## Overview
This repository contains a C implementation of an A* search algorithm to solve the classic 15 Puzzle problem. The problem revolves around a 4x4 grid containing 15 blocks and a vacant space. The objective is to arrange the blocks sequentially by maneuvering them through the empty space. In this problem the blank tile is labeled with number 0 and other tiles are labeled with non-zero numbers from 1~15.

## Features
- A* Search Algorithm: Utilizes the A* search algorithm to efficiently explore possible states and find the optimal solution to the 15 Puzzle problem.
- **Heuristic Functions:** Implements both Manhattan distance and misplaced tiles heuristic functions to guide the search towards the goal state.
- **Node Filtering:** Implements a node filtering mechanism to reduce redundant exploration by checking and filtering nodes that are already in the open or closed lists.
- **Path Tracing:** Once a solution is found, the program traces back the optimal path and prints the sequence of moves required to reach the goal state.

## Usage
### Clone the repository
```
git clone https://github.com/danielxurbina/15-Puzzle-Solver.git
```
### Compile and Run
```
cd 15-Puzzle-Solver
gcc 15_puzzle_solver.c -o 15_puzzle_solver
./15_puzzle_solver [initial state]
```
Replace [initial state] with the initial configuration of the 15 Puzzle. For example:
```
./15_puzzle_solver 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 0
```
You can use `GenGemPuzzle.c` to generate initial states to use for the puzzle. It takes an integer as its argument, which is the number of moves it makes before it finishes. When running the program with the inital states, it should have 16 unique integers between 0 and 15 (including 0 and 15).
