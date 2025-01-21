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
#include <iomanip> // dla precyzji zapisu liczb
class makeGraph {


public:
    makeGraph();
    void generateGraph(std::vector<std::vector<int>>& graph, int density, bool isDirected, int V);
    void getFromFile(std::string nazwaPlikuWe, std::vector<std::vector<int>>&, int&, int&);
    bool isSymetric(std::vector<std::vector<int>>& graph, int V);
    static void writeToFileTimesAndAvg(std::vector<double> times, std::vector<int> absoluteErrors,
                              std::vector<double> relativeErrors,
                              std::vector<int> solutions,
                              double avgTime, double absErrorAvg, double relativeErrorAvg);
    static void writeMainInfoForExcel(int V, double avgTime, double absErrorAvg, double relativeErrorAvg, std::string);
    static void writeAboutAnts(double evaporation, double alfa, double beta, double Q, std::string file_name);
};



#endif //MAKEGRAPH_H
