//
// Created by piotr on 25.12.2024.
//

#ifndef ZADANIE_3_ANTS_H
#define ZADANIE_3_ANTS_H

#include <vector>
#include <limits>
#include "../mainProgram/Timer.h"

class Ants {
private:
    Timer& timer_;
    int NUM_ANTS;
    double ALPHA;       // Wpływ feromonu
    double BETA;        // Wpływ odległości
    double EVAPORATION; // Współczynnik parowania feromonu
    double Q;           // Stała wzmocnienia feromonu

    std::vector<std::vector<double>> pheromones; // Macierz feromonów
    std::vector<std::vector<int>> distances;    // Macierz odległości
    std::vector<int> bestPath;                  // Najlepsza ścieżka
    double bestCost;                            // Koszt najlepszej ścieżki

    double procentageOfLowerBound;
    int iterationsWithoutImprove;
    int solutionFromFile;

    void initializePheromones(int V);
    std::vector<int> generatePath(int start, int& cost, int V, int typeOfAlgorithm, bool isSymetric);
    int selectNextCity(int currentCity, const std::vector<bool>& visited, int V);
    void evaporatePheromonesCAS(int V);
    void updatePheromonesCAS(const std::vector<std::vector<int>>& paths, const std::vector<int>& costs, bool);
    void evaporatePheromoneForEdge(bool isSymetric, int i, int j);
    void updatePheromoneForEdge(int typeOfAlgorithm, bool isSymetric, int i, int j, int cost);
    bool ifOptimumFound();
    bool ifInProcentageOfLowerBound();

public:
    Ants(Timer, int, double, double, double, double, double, int, int);
    void ACOCAS(std::vector<std::vector<int>>& graph, int V, bool isSymetric);
    void ACODAS(std::vector<std::vector<int>>& graph, int V, bool isSymetric);
    void ACOQAS(std::vector<std::vector<int>>& graph, int V, bool isSymetric);
    void printBestPath();
    void printPheromones();
    const std::vector<int>& getBestPath();
    double getBestCost();
};

#endif //ZADANIE_3_ANTS_H
