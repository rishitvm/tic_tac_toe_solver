# Overview
This repository contains a Tic-Tac-Toe solver implemented in C using parallel programming with OpenMP. The solver allows a user to play against the computer, providing an efficient and challenging gameplay experience.

# Features
- User vs Computer: Play against a computer opponent.
- Parallel Processing: Utilizes OpenMP for parallel computation, enhancing performance.
- Efficient Algorithm: Implements advanced algorithms to ensure optimal moves by the computer.
  
# Prerequisites
- GCC Compiler
- OpenMP library

# How It Works
The Tic-Tac-Toe solver uses a parallelized version of the Minimax algorithm with alpha-beta pruning to determine the best possible move for the computer. OpenMP is employed to distribute the computation across multiple threads, significantly reducing the time required to evaluate potential game states.

# License
This project is licensed under the MIT License. See the LICENSE file for details.
