//
// Created by piotr on 25.12.2024.
//

#include "Ants.h"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>

Ants::Ants(Timer timer, double alfa, double beta, double parowanie, double stala, double procentLB, int iterWithoutImprovement, int solFromFile)
        : timer_(timer),
          ALPHA(alfa),
          BETA(beta),
          EVAPORATION(parowanie),
          Q(stala),
          bestCost(INT_MAX),
          procentageOfLowerBound(procentLB),
          iterationsWithoutImprove(iterWithoutImprovement),
          solutionFromFile(solFromFile){
    srand(time(0)); // Inicjalizacja generatora liczb losowych
}

void Ants::initializePheromones(int V) {
    pheromones = std::vector<std::vector<double>>(V, std::vector<double>(V, 1.0));
}

std::vector<int> Ants::generatePath(int start, int& cost, int V, int typeOfAlgorithm, bool isSymetric) {
    std::vector<int> path;
    std::vector<bool> visited(V, false);

    int currentCity = start;
    path.push_back(currentCity);
    visited[currentCity] = true;

    while (path.size() < V) {
        int nextCity = selectNextCity(currentCity, visited, V);
        path.push_back(nextCity);
        visited[nextCity] = true;
        cost += distances[currentCity][nextCity];
        if (typeOfAlgorithm == 2){//gdy (DAS)
//            evaporatePheromoneForEdge(isSymetric,currentCity, nextCity);
            updatePheromoneForEdge(2, isSymetric, currentCity, nextCity, distances[currentCity][nextCity]);
        } else if (typeOfAlgorithm == 3) {//gdy QAS
//            evaporatePheromoneForEdge(isSymetric,currentCity, nextCity);
            updatePheromoneForEdge(3, isSymetric, currentCity, nextCity, distances[currentCity][nextCity]);
        }
        currentCity = nextCity;
    }

    // Powrót do początkowego miasta
    cost += distances[currentCity][path[0]];
    if (typeOfAlgorithm == 2){//gdy (DAS)
//        evaporatePheromoneForEdge(isSymetric,currentCity, path[0]);
        updatePheromoneForEdge(2, isSymetric, currentCity, path[0], distances[currentCity][path[0]]);
    } else if (typeOfAlgorithm == 3) {//gdy QAS
//        evaporatePheromoneForEdge(isSymetric,currentCity, path[0]);
        updatePheromoneForEdge(3, isSymetric, currentCity, path[0], distances[currentCity][path[0]]);
    }
    path.push_back(path[0]);//dodanie do sciezki poczatkowego wierzcholka
    return path;
}

int Ants::selectNextCity(int currentCity, const std::vector<bool>& visited, int V) {
    std::vector<double> probabilities(V, 0.0);
    double sum = 0.0;

    // Oblicz prawdopodobieństwa
    for (int i = 0; i < V; ++i) {
        if (!visited[i]) {
            probabilities[i] = pow(pheromones[currentCity][i], ALPHA) * pow(1.0 / distances[currentCity][i], BETA);
            sum += probabilities[i];
        }
    }

    // Jeśli suma prawdopodobieństw wynosi 0 (co nie powinno się zdarzyć), zwróć losowe miasto
    if (sum == 0.0) {
        for (int i = 0; i < V; ++i) {
            if (!visited[i]) {
                return i;
            }
        }
    }

    // Normalizuj prawdopodobieństwa
    for (int i = 0; i < V; ++i) {
        probabilities[i] /= sum;
    }

    // Stwórz tablicę skumulowanych prawdopodobieństw
    std::vector<double> cumulative(V, 0.0);
    cumulative[0] = probabilities[0];
    for (int i = 1; i < V; ++i) {
        cumulative[i] = cumulative[i - 1] + probabilities[i];
    }

    // Wylosuj wartość i znajdź odpowiadające miasto
    int z = 0;
    while (true) {

        double random = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
        for (int i = 0; i < V; ++i) {
            if (random <= cumulative[i]) {
                if (!visited[i])
                    return i;
                break;
            }
        }
        z++;
        // std::cout<<"PIZZA: "<<z<<std::endl;
    }

    // // Zabezpieczenie awaryjne — zwróć pierwsze nieodwiedzone miasto
    // for (int i = 0; i < V; ++i) {
    //     if (!visited[i]) {
    //         visited[i] = true;
    //         return i;
    //     }
    // }
    std::cout<<"ZWROCONO BLAD"<<std::endl;

    return 0; // W razie błędu
}


void Ants::evaporatePheromonesCAS(int V) {
    for (int i = 0; i < V; ++i) {
        for (int j = 0; j < V; ++j) {
            pheromones[i][j] *= (1.0 - EVAPORATION);
        }
    }
}

void Ants::updatePheromonesCAS(const std::vector<int>& path, int cost, bool isSymetric) {
    double pheromoneDeposit = Q / cost;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        int from = path[i];
        int to = path[i + 1];
        if(!isSymetric) {//asy
            pheromones[from][to] += pheromoneDeposit;//tylko ta linia powinna być dla asym xD
        } else{//sym
            pheromones[from][to] += pheromoneDeposit;
            pheromones[to][from] += pheromoneDeposit;
            }
    }
}

void Ants::ACOCAS(std::vector<std::vector<int>>& graph, int V, bool isSymetric) {//cykliczny cycle ant system (typ = 1)
    int LB = Prim(graph);
    NUM_ANTS = V;
    distances = graph;
    initializePheromones(V);

    int k = 0;

    while(!ifOptimumFound() && !ifInProcentageOfLowerBound(LB)){
        double elapsed_time = timer_.getCounter();
        if (elapsed_time >= timer_.time_limit) {
            return;
        }

        if (k >= iterationsWithoutImprove)
            return;

        std::vector<std::vector<int>> antPaths(NUM_ANTS);
        std::vector<int> antCosts(NUM_ANTS, 0);

        for (int ant = 0; ant < NUM_ANTS; ++ant) {
            int startCity = rand() % V;
            antPaths[ant] = generatePath(startCity, antCosts[ant], V, 1, isSymetric);
            updatePheromonesCAS(antPaths[ant], antCosts[ant], isSymetric);
        }

        evaporatePheromonesCAS(V);


        for (int ant = 0; ant < NUM_ANTS; ++ant) {//aktulizacja najlepszego rozwiazania
            if (antCosts[ant] < bestCost) {
                bestCost = antCosts[ant];
                bestPath = antPaths[ant];
                k = 0;//zerowanie zmiennej liczacej ile iteracji bez poprawy
            }
        }

        k++;
    }
}

void Ants::ACODAS(std::vector<std::vector<int>>& graph, int V, bool isSymetric) {//gestosciowy dennsity ant system (typ = 2)
    int LB = Prim(graph);
    NUM_ANTS = V;
    distances = graph;
    initializePheromones(V);

    int k = 0;

    while(!ifOptimumFound() && !ifInProcentageOfLowerBound(LB)){
        double elapsed_time = timer_.getCounter();
        if (elapsed_time >= timer_.time_limit) {
            return;
        }

        if (k >= iterationsWithoutImprove)
            return;

        std::vector<std::vector<int>> antPaths(NUM_ANTS);
        std::vector<int> antCosts(NUM_ANTS, 0);

        for (int ant = 0; ant < NUM_ANTS; ++ant) {
            int startCity = rand() % V;
            antPaths[ant] = generatePath(startCity, antCosts[ant], V, 2, isSymetric);
        }

        evaporatePheromonesCAS(V);

        for (int ant = 0; ant < NUM_ANTS; ++ant) {//aktulizacja najlepszego rozwiazania
            if (antCosts[ant] < bestCost) {
                bestCost = antCosts[ant];
                bestPath = antPaths[ant];
                k = 0;//zerowanie zmiennej liczacej ile iteracji bez poprawy
            }
        }

        k++;
    }
}

void Ants::evaporatePheromoneForEdge(bool isSymetric, int i, int j) {
    if(!isSymetric) {
        pheromones[i][j] *= (1.0 - EVAPORATION);//tylko ta linia powinna być dla asym xD
    } else{
        pheromones[i][j] *= (1.0 - EVAPORATION);//aktualizacja jednej i tej samej krawedzi na ins symetrycznej
        pheromones[j][i] *= (1.0 - EVAPORATION);
    }
}

void Ants::updatePheromoneForEdge(int typeOfAlgorithm, bool isSymetric, int i, int j, int cost) {
    double pheromoneDeposit;
    if(typeOfAlgorithm == 2){
        pheromoneDeposit = Q;//taki sam dla kazdej krawedzi
    } else if(typeOfAlgorithm == 3){
        pheromoneDeposit = Q/static_cast<double>(cost);
    }

    if(!isSymetric) {//aktualizacja feromonu na danych klrawedziach w zaleznosci od instacji problemu
        pheromones[i][j] += pheromoneDeposit;//tylko ta linia powinna być dla asym xD
    } else{
        pheromones[i][j] += pheromoneDeposit;
        pheromones[j][i] += pheromoneDeposit;
    }
}

void Ants::ACOQAS(std::vector<std::vector<int>>& graph, int V, bool isSymetric) {//ilosciowy quantity ant system (typ = 3)
    int LB = Prim(graph);

    NUM_ANTS = V;
    distances = graph;
    initializePheromones(V);

    int k = 0;

    while(!ifOptimumFound() && !ifInProcentageOfLowerBound(LB)){
        double elapsed_time = timer_.getCounter();
        if (elapsed_time >= timer_.time_limit) {
            return;
        }

        if (k >= iterationsWithoutImprove)
            return;

        std::vector<std::vector<int>> antPaths(NUM_ANTS);
        std::vector<int> antCosts(NUM_ANTS, 0);

        for (int ant = 0; ant < NUM_ANTS; ++ant) {
            int startCity = rand() % V;
            antPaths[ant] = generatePath(startCity, antCosts[ant], V, 3, isSymetric);
        }

        evaporatePheromonesCAS(V);

        for (int ant = 0; ant < NUM_ANTS; ++ant) {//aktulizacja najlepszego rozwiazania
            if (antCosts[ant] < bestCost) {
                bestCost = antCosts[ant];
                bestPath = antPaths[ant];
                k = 0;//zerowanie zmiennej liczacej ile iteracji bez poprawy
            }
        }

        k++;
    }
}

void Ants::printBestPath() {
    std::cout << "Najlepsza sciezka: ";
    for (int city : bestPath) {
        std::cout << city << " ";
    }
    std::cout << std::endl;
    std::cout << "Koszt: " << bestCost << std::endl;
}

void Ants::printPheromones() {
    std::cout << "Macierz feromonów:" << std::endl;
    for (size_t i = 0; i < pheromones.size(); ++i) {
        for (size_t j = 0; j < pheromones[i].size(); ++j) {
            std::cout << pheromones[i][j] << " ";
        }
        std::cout << std::endl; // Nowa linia po każdej kolumnie
    }
}
const std::vector<int>& Ants::getBestPath() {
    return bestPath;
}

int Ants::getBestCost(){
    return static_cast<int>(bestCost);
}

bool Ants::ifOptimumFound() {
    if(solutionFromFile == -1 || solutionFromFile != bestCost)
        return false;
    else
        return true;
}

bool Ants::ifInProcentageOfLowerBound(int LB){
    if (LB == -1)//nie przerywamy bo nie mamy nawet podstaw na to
        return false;
    else{
        auto LC = static_cast<double>(bestCost);
        auto lower_bound = static_cast<double>(LB);
        if (100 * ((LC - lower_bound)/lower_bound)<= procentageOfLowerBound)
            return true;
        else
            return false;
    }
}


int Ants::countAbsoluteError() {//blad bezwzgledny
    if(solutionFromFile != -1)
        return bestCost - solutionFromFile;
    return -1;
}

double Ants::countRelativeError() {
    if(solutionFromFile != -1)
        return (static_cast<double>(bestCost) - static_cast<double>(solutionFromFile))/static_cast<double>(solutionFromFile);
    return -1.0;
}

int Ants::Prim(std::vector<std::vector<int>> &graph) {
    int n = graph.size(); // Liczba wierzchołków
    std::vector<bool> visited(n, false); // Czy wierzchołek został odwiedzony
    std::vector<int> min_edge(n, INT_MAX); // Minimalny koszt dotarcia do wierzchołka
    min_edge[0] = 0; // Startujemy od wierzchołka 0

    int mst_cost = 0; // Koszt MST

    for (int i = 0; i < n; ++i) {
        // Znajdź nieodwiedzony wierzchołek o najmniejszym koszcie dotarcia
        int u = -1;
        for (int j = 0; j < n; ++j) {
            if (!visited[j] && (u == -1 || min_edge[j] < min_edge[u])) {
                u = j;
            }
        }

        // Jeśli nie ma dostępnych wierzchołków, przerywamy
        if (min_edge[u] == INT_MAX) {
            std::cout << "Graf jest niespójny!" << std::endl;
            return -1;
        }

        visited[u] = true; // Oznacz wierzchołek jako odwiedzony
        mst_cost += min_edge[u]; // Dodaj koszt do MST

        // Zaktualizuj minimalne koszty dla sąsiadów
        for (int v = 0; v < n; ++v) {
            if (graph[u][v] != 0 && !visited[v]) { // Jeśli istnieje krawędź i wierzchołek nieodwiedzony
                min_edge[v] = std::min(min_edge[v], graph[u][v]);
            }
        }
    }

    return mst_cost; // Zwróć koszt MST
}