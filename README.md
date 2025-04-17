#  Travelling salesman problem 2

This project implements **three variations of Ant Colony Optimization (ACO)** algorithms to solve the **Traveling Salesman Problem (TSP)**. Each algorithm differs in how ants deposit pheromones during their pathfinding.

## 🚀 Overview

The three implemented ACO strategies are:

1. **Cycle Ant System (CAS)** – Pheromones are updated after all ants complete their routes.
2. **Density Ant System (DAS)** – Pheromones are deposited by each ant during path construction with a constant value.
3. **Quantity Ant System (QAS)** – Similar to DAS, but pheromone deposit is inversely proportional to the traveled distance (i.e., better solutions leave more pheromone).

Each approach simulates artificial ants building solutions based on a probabilistic model influenced by pheromone trails and heuristic distance information.

## 📄 Structure

- **Ants.h / Ants.cpp** – Core logic for each ACO algorithm, including pheromone updates, path generation, and stopping criteria.
- **Timer class** – Used to limit the algorithm’s execution time.
- **Prim algorithm** – Used for estimating a lower bound in the stopping criteria.
- **Console output** – Prints best found path and its cost.
- **plikKonfiguracyjny.txt** - used to adjust key parameters of algorithms

## 📊 Results

A detailed comparison of all three algorithms is available in the file:

📁 `Raport_2.pdf`

This includes:

- Execution times
- Solution quality
- Relative and absolute errors comparison

## ⚙️ Key Parameters

- `ALPHA`, `BETA` – Influence of pheromones vs. distances in path selection
- `EVAPORATION` – Pheromone evaporation rate
- `Q` – Constant pheromone amount
- `iterationsWithoutImprove` – Max iterations without improvement
- `procentageOfLowerBound` – Stopping condition based on closeness to a computed lower bound

## 🧠 Algorithm Differences (Summary)

| Algorithm | Pheromone Update Timing | Pheromone Amount                     |
|-----------|--------------------------|--------------------------------------|
| CAS       | End of iteration          | Based on total path length           |
| DAS       | During path construction | Constant                              |
| QAS       | During path construction | Inversely proportional to edge cost |


## 🚀 **How to Run the Project**

1. **Build the project using Clion and CMake**:

    ```bash
    cmake -B cmake-build-debug
    cmake --build cmake-build-debug
    ```

2. **Navigate to the output folder**:

    ```bash
    cd cmake-build-debug
    ```

3. **Run the program**:

    ```bash
    ./zad_4.exe
    ```

Or in Windows command line (CMD), from the main project directory:

1. **Go into cmake-build-debug**
```cmd
 cd cmake-build-debug
```
2. **Run exe**
```cmd
   .\zad_4.exe
```