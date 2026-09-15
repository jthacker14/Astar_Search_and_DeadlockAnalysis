# Optimal Solitaire Solver: A* Search & Deadlock Analysis 🃏

## 📌 Project Overview
This project implements a novel method for finding **provably optimal solutions** for skill-based solitaire games like FreeCell. Since finding the fewest possible moves is NP-hard , this solver uses the **A* (A-star) search algorithm** guided by a custom, admissible heuristic.

## 🧠 How it Works
The core innovation is modeling the game state as a **Directed Graph**:
* **Dependency Edges:** Represent the goal of building foundation piles.
* **Blocking Edges:** Represent cards trapped in tableau columns.
* **Deadlock Detection:** Mathematical cycles in the graph indicate deadlocks. 

The heuristic function $h(n)$ calculates the minimum moves needed to break these cycles, ensuring the search finds the absolute shortest path to victory.


## 📂 Repository Contents
* `solver.cpp`: The core optimal solver implemented in C++.
* `deadlock_demo.html`: A practical web-based demo for 1-suit cycle detection using Tailwind CSS.
* `Report.pdf`: The full research paper detailing methodology and results.

## 🚀 Getting Started

### C++ Solver
To run the optimization engine:
1. Compile the code: `g++ -O3 solver.cpp -o solver`
2. Run the executable: `./solver`

### Web Demo
Simply open `deadlock_demo.html` in any modern web browser to interactively find deadlocks on a sample board.

## 📊 Performance Results
Testing on 5,000 Microsoft FreeCell games yielded:
* **Success Rate:** 100% optimal solutions found.
* **Average Solution Length:** ~77 moves.
* **Best Heuristic:** $h_{2}$ (balancing 1-suit and 2-suit cycles).

## 👥 Authors
* **Ishita Kaur Sahni** 
* **Jiya Thacker** 
* *MBA Tech (Data Science), NMIMS* 

## 📜 References
Based on: G. Paul and M. Helmert, "Optimal Solitaire Game Solutions using A* Search and Deadlock Analysis" (2016).
