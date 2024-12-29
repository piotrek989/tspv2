//
// Created by Piotr on 10.12.2024.
//

#include "makeGraph.h"


makeGraph::makeGraph()
{

}

void makeGraph::getFromFile(std::string nazwaPlikuWe, std::vector<std::vector<int>>& graph, int& V, int& solutionFromFile) {
    std::ifstream inputFile(nazwaPlikuWe);

    if (!inputFile.is_open()) {
        throw std::runtime_error("Nie udało się otworzyć pliku: " + nazwaPlikuWe);
    }

    // Wczytanie liczby wierzchołków
    inputFile >> V;
    if (inputFile.fail()) {
        throw std::runtime_error("Błąd podczas odczytu liczby wierzchołków z pliku.");
    }

    // Wczytanie najlepszego znalezionego wyniku
    inputFile >> solutionFromFile;
    if (inputFile.fail()) {
        throw std::runtime_error("Błąd podczas odczytu wartości shortest_path z pliku.");
    }

    // Wczytanie macierzy sąsiedztwa
    graph.resize(V, std::vector<int>(V));
    for (int i = 0; i < V; ++i) {
        for (int j = 0; j < V; ++j) {
            inputFile >> graph[i][j];
            if (inputFile.fail()) {
                throw std::runtime_error("Błąd podczas odczytu macierzy sąsiedztwa z pliku.");
            }
        }
    }

    inputFile.close();
}




void makeGraph::generateGraph(std::vector<std::vector<int>>& graph, int density, bool isDirected, int V) {
    if (density < 0 || density > 100 || V <= 0) {
        std::cerr << "Invalid input parameters: density must be between 0 and 100, and V > 0.\n";
        return;
    }

    // Inicjalizacja pustej macierzy sąsiedztwa
    graph.clear();
    graph.resize(V, std::vector<int>(V, -1));

    // Obliczenie maksymalnej liczby krawędzi i docelowej liczby krawędzi
    int maxEdges = isDirected ? V * (V - 1) : V * (V - 1) / 2;
    int targetEdges = (maxEdges * density) / 100;

    // Generator liczb pseudolosowych
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distVertex(0, V - 1); // Zakres wierzchołków
    std::uniform_int_distribution<> distWeight(1, 1000);  // Zakres wag krawędzi

    // Dodawanie krawędzi
    int edgesAdded = 0;

    while (edgesAdded < targetEdges) {
        int u = distVertex(gen); // Losowy wierzchołek początkowy
        int v = distVertex(gen); // Losowy wierzchołek końcowy
        int edgeWeight = distWeight(gen);

        // Sprawdzanie warunków: brak pętli własnych i brak duplikatów
        if (u != v && graph[u][v] == -1) {
            graph[u][v] = edgeWeight;
            edgesAdded++;

            if (!isDirected) {
                graph[v][u] = edgeWeight; // Krawędź w obu kierunkach
            }
        }
    }

//     std::cout << "Generated graph (adjacency matrix):\n";
//     for (const auto& row : graph) {
//         for (int val : row) {
//             std::cout<<val << " ";
//         }
//         std::cout << "\n";
//     }
}

bool makeGraph::isSymetric(std::vector<std::vector<int>>& graph, int V){
    for(int i = 0 ; i < V ; i ++){
        for (int j = 0 ; j < V ; j++){
            if(graph[i][j] != graph[j][i])
                return false;
        }
    }
    return true;
}
