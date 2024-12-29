//
// Created by piotr on 25.12.2024.
//

#include "Ants.h"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>

Ants::Ants(Timer timer, int ants, double alfa, double beta, double parowanie, double stala, double procentLB, int iterWithoutImprovement, int solFromFile)
        : timer_(timer),
          NUM_ANTS(ants),
          ALPHA(alfa),
          BETA(beta),
          EVAPORATION(parowanie),
          Q(stala),
          bestCost(std::numeric_limits<double>::infinity()),
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

    for (int i = 0; i < V; ++i) {
        if (!visited[i]) {
            probabilities[i] = pow(pheromones[currentCity][i], ALPHA) * pow(1.0 / distances[currentCity][i], BETA);
            sum += probabilities[i];
        }
    }


    double random = ((double)rand() / RAND_MAX) * sum;
    for (int i = 0; i < V; ++i) {
        if (!visited[i]) {
            random -= probabilities[i];
            if (random <= 1e-10) {
                return i;
            }
        }
    }
    std::cout<<"WARTOSC DOUBLEA: "<<random<<std::endl;
    return 0; // Na wypadek błędu
}

void Ants::evaporatePheromonesCAS(int V) {
    for (int i = 0; i < V; ++i) {
        for (int j = 0; j < V; ++j) {
            pheromones[i][j] *= (1.0 - EVAPORATION);
        }
    }
}

void Ants::updatePheromonesCAS(const std::vector<std::vector<int>>& paths, const std::vector<int>& costs, bool isSymetric) {
    for (int ant = 0; ant < NUM_ANTS; ++ant) {
        double pheromoneDeposit = Q / costs[ant];
        for (size_t i = 0; i < paths[ant].size() - 1; ++i) {
            int from = paths[ant][i];
            int to = paths[ant][i + 1];
            if(!isSymetric) {
                pheromones[from][to] += pheromoneDeposit;//tylko ta linia powinna być dla asym xD
            } else{
                pheromones[from][to] += pheromoneDeposit;
                pheromones[to][from] += pheromoneDeposit;
                }
        }
    }
}

void Ants::ACOCAS(std::vector<std::vector<int>>& graph, int V, bool isSymetric) {//cykliczny cycle ant system (typ = 1)
    distances = graph;
    initializePheromones(V);

    int k = 0;

    while(!ifOptimumFound() && !ifInProcentageOfLowerBound()){
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
        }

        evaporatePheromonesCAS(V);
        updatePheromonesCAS(antPaths, antCosts, isSymetric);

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
    distances = graph;
    initializePheromones(V);

    int k = 0;

    while(!ifOptimumFound() && !ifInProcentageOfLowerBound()){
        double elapsed_time = timer_.getCounter();
        if (elapsed_time >= timer_.time_limit) {
            return;
        }

        if (k >= iterationsWithoutImprove)
            return;

        std::vector<std::vector<int>> antPaths(NUM_ANTS);
        std::vector<int> antCosts(NUM_ANTS, 0);

        evaporatePheromonesCAS(V);

        for (int ant = 0; ant < NUM_ANTS; ++ant) {
            int startCity = rand() % V;
            antPaths[ant] = generatePath(startCity, antCosts[ant], V, 2, isSymetric);
        }



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
    distances = graph;
    initializePheromones(V);

    int k = 0;

    while(!ifOptimumFound() && !ifInProcentageOfLowerBound()){
        double elapsed_time = timer_.getCounter();
        if (elapsed_time >= timer_.time_limit) {
            return;
        }

        if (k >= iterationsWithoutImprove)
            return;

        std::vector<std::vector<int>> antPaths(NUM_ANTS);
        std::vector<int> antCosts(NUM_ANTS, 0);

        evaporatePheromonesCAS(V);

        for (int ant = 0; ant < NUM_ANTS; ++ant) {
            int startCity = rand() % V;
            antPaths[ant] = generatePath(startCity, antCosts[ant], V, 3, isSymetric);
        }

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

double Ants::getBestCost(){
    return bestCost;
}

bool Ants::ifOptimumFound() {
    if(solutionFromFile == -1 || solutionFromFile != bestCost)
        return false;
    else
        return true;
}

bool Ants::ifInProcentageOfLowerBound(){
    if (solutionFromFile == -1)//nie przerywamy bo nie mamy nawet podstaw na to
        return false;
    else{
        double LC = static_cast<double>(bestCost);
        double SFF = static_cast<double>(solutionFromFile);
        if (100.0 * ((LC - SFF)/SFF) <= procentageOfLowerBound)
            return true;
        else
            return false;
    }
}