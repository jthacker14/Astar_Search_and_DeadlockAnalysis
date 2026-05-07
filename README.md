# Optimal Solitaire Solver: A* Search & Deadlock Analysis 🃏

## 📌 Project Overview
[cite_start]This project implements a novel method for finding **provably optimal solutions** for skill-based solitaire games like FreeCell[cite: 2, 14]. [cite_start]Since finding the fewest possible moves is NP-hard [cite: 10, 11][cite_start], this solver uses the **A* (A-star) search algorithm** guided by a custom, admissible heuristic[cite: 15, 24].

## 🧠 How it Works
[cite_start]The core innovation is modeling the game state as a **Directed Graph**[cite: 16, 33]:
* [cite_start]**Dependency Edges:** Represent the goal of building foundation piles[cite: 38].
* **Blocking Edges:** Represent cards trapped in tableau columns[cite: 41].
* [cite_start]**Deadlock Detection:** Mathematical cycles in the graph indicate deadlocks[cite: 17, 44]. 

[cite_start]The heuristic function $h(n)$ calculates the minimum moves needed to break these cycles, ensuring the search finds the absolute shortest path to victory[cite: 17, 75, 81].


## 📂 Repository Contents
* `solver.cpp`: The core optimal solver implemented in C++[cite: 93].
* [cite_start]`deadlock_demo.html`: A practical web-based demo for 1-suit cycle detection using Tailwind CSS[cite: 142, 152].
* `Report.pdf`: The full research paper detailing methodology and results[cite: 2].

## 🚀 Getting Started

### C++ Solver
To run the optimization engine:
1. Compile the code: `g++ -O3 solver.cpp -o solver`
2. Run the executable: `./solver`

### Web Demo
Simply open `deadlock_demo.html` in any modern web browser to interactively find deadlocks on a sample board[cite: 232, 235].

## 📊 Performance Results
Testing on 5,000 Microsoft FreeCell games yielded[cite: 93, 101]:
* [cite_start]**Success Rate:** 100% optimal solutions found[cite: 101].
* [cite_start]**Average Solution Length:** ~77 moves[cite: 103].
* **Best Heuristic:** $h_{2}$ (balancing 1-suit and 2-suit cycles)[cite: 97, 106].

## 👥 Authors
* [cite_start]**Ishita Kaur Sahni** [cite: 3]
* [cite_start]**Jiya Thacker** [cite: 4]
* *MBA Tech (Data Science), NMIMS* [cite: 5]

## 📜 References
Based on: G. Paul and M. Helmert, "Optimal Solitaire Game Solutions using A* Search and Deadlock Analysis" (2016)[cite: 668].