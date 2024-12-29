//
// Created by Piotr on 10.12.2024.
//

#ifndef MAKEGRAPH_H
#define MAKEGRAPH_H
#include <vector>
#include <iostream>
#include <random>
#include <ctime>
#include <fstream>

class makeGraph {


public:
    makeGraph();
    void generateGraph(std::vector<std::vector<int>>& graph, int density, bool isDirected, int V);
    void getFromFile(std::string nazwaPlikuWe, std::vector<std::vector<int>>&, int&, int&);
    bool isSymetric(std::vector<std::vector<int>>& graph, int V);
};



#endif //MAKEGRAPH_H
