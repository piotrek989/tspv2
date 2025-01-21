//
// Created by Piotr on 10.12.2024.
//

#include "Program.h"

#include "Timer.h"
#include "../Algorithms/Ants.h"

Program::Program()
    : solutionFromFile(-1),
      ifTesting(true)
{}

bool stringToBool(const std::string& str) {
    if (str == "true") {
        return true;
    } else if (str == "false") {
        return false;
    }
    // Jeśli string nie jest ani "true" ani "false", możesz zwrócić domyślną wartość, np. false, albo rzucić wyjątek
    throw std::invalid_argument("Invalid input: not a valid boolean string.");
}

void Program::wczytanieZPlikuKonfiguracyjnego() {
    // Ścieżka do pliku konfiguracyjnego - do uzupełnienia
    const std::string sciezkaDoPliku = "../Files/plikKonfiguracyjny.txt"; // Podaj tutaj ścieżkę do pliku

    std::ifstream plik(sciezkaDoPliku);
    if (!plik.is_open()) {
        std::cerr << "Nie udało się otworzyć pliku konfiguracyjnego: " << sciezkaDoPliku << std::endl;
        return;
    }

    std::string linia;
    while (std::getline(plik, linia)) {
        // Znajdź pozycję dwukropka
        std::size_t pozycjaDwukropka = linia.find(':');
        if (pozycjaDwukropka == std::string::npos) {
            std::cerr << "Niepoprawny format linii w pliku konfiguracyjnym: " << linia << std::endl;
            continue;
        }

        // Klucz to wszystko przed dwukropkiem (włącznie z nim)
        std::string klucz = linia.substr(0, pozycjaDwukropka + 1);
        // Wartość to wszystko po dwukropku
        std::string wartosc = linia.substr(pozycjaDwukropka + 1);

        // Usuń ewentualne białe znaki z wartości (np. spacje na początku)
        wartosc.erase(0, wartosc.find_first_not_of(" \t\r\n"));

        // Dodaj do mapy
        konfiguracja[klucz] = wartosc;

    }


    //uwaga w kluczu podajemy z dwukropkiem
    maxCzasAlgorytmow = std::stoi(konfiguracja["max czas wykonywania algorytmu[ms]:"]);
    iteracjeAlgorytmow = std::stoi(konfiguracja["liczba powtorzen algorytmow:"]);
    nazwaPlikuWejsciowego = konfiguracja["nazwa pliku wejsciowego:"];
    nazwaPlikuWyjsciowego = konfiguracja["nazwa pliku wyjsciowego:"];
    ifFromFile = stringToBool(konfiguracja["test z pojedynczego pliku:"]);
    iterationsWithoutImprove = std::stoi(konfiguracja["ilosc iteracji bez poprawy:"]);
    procentageOfLowerBound = std::stof(konfiguracja["procent dolnego ograniczenia:"]);
    // NUM_ANTS = std::stoi(konfiguracja["liczba mrowek:"]);
    ALPHA = std::stod(konfiguracja["alpha:"]);       // Wpływ feromonu
    BETA = std::stod(konfiguracja["beta:"]);         // Wpływ odległości
    EVAPORATION = std::stod(konfiguracja["wspolczynnik parowania feromonu:"]);  // Współczynnik parowania feromonu
    Q = std::stod(konfiguracja["stala:"]);
    plik.close();
}

void Program::printConfigFile() {
    std::cout << "max czas wykonywania algorytmu[ms]: " << maxCzasAlgorytmow << "\n";
    std::cout << "liczba powtorzen algorytmow: " << iteracjeAlgorytmow << "\n";
    std::cout << "Nazwa pliku we: " << nazwaPlikuWejsciowego << std::endl;
    std::cout << "Nazwa pliku wy: " << nazwaPlikuWyjsciowego << std::endl;
    std::cout << "Procent dolnego ograniczenia: " << procentageOfLowerBound << std::endl;
    std::cout << "Ilosc iteracji bez poprawy: " << iterationsWithoutImprove << std::endl;
    std::cout << "alfa: " << ALPHA << std::endl;
    std::cout << "beta: " << BETA << std::endl;
    std::cout << "wspolczynnik parowania feromonu: " << EVAPORATION << std::endl;
    std::cout << "stala: " << Q << std::endl;
}


void Program::glownyProgram() {
    wczytanieZPlikuKonfiguracyjnego();
    printConfigFile();

    std::vector<std::vector<int>> graph;
    int V;

    if(ifTesting) {//jesli testy
        int z = 0;
        float all = 27;

        std::vector<std::string> listaInstancji = {
            "5s", "6s", "7s", "8s", "9s", "10s", "11s", "12s", "13s",
            "5a", "6a", "7a", "8a", "9a", "10a", "11a", "12a", "13a",
            "17a", "34a", "65a", "100a", "171a",
            "14s", "29s", "100s", "137s", "150s", "200s"
        };

        std::vector<double> alphas = {1.0, 3.0, 5.0};
        std::vector<double> betas = {2.0, 3.0, 5.0};
        std::vector<double> evaporations = {0.25, 0.5, 0.75};//29 testów powinno byc

        Q = 5;
        EVAPORATION = 0.5;//to chyba nawet nie uzywane do testow
        iteracjeAlgorytmow = 5;
        maxCzasAlgorytmow = 900000;//900k
        procentageOfLowerBound = 0.0;
        iterationsWithoutImprove = 10;

        makeGraph makegraph;//obiekt do wczytywania plikow
        Timer timer(maxCzasAlgorytmow);//moj obiekt zegara
        Timer timer2(maxCzasAlgorytmow);

        std::vector<double> times;
        double avgTime;
        double absErrorAvg;
        double relativeErrorAvg;
        std::vector<int> absoluteErrors;
        std::vector<double> relativeErrors;
        std::vector<int> solutions;

        for (double ro : evaporations) {
            for (double alpha : alphas) {
                for (double beta : betas) {
                    makeGraph::writeAboutAnts(ro, alpha, beta, Q, "outputCAS.txt");
                    for (const auto& instancja : listaInstancji)  {
                        avgTime = 0.0;
                        absErrorAvg = 0.0;
                        relativeErrorAvg = 0.0;

                        std::string nazwaPojInstancji = "../Files/file_" + instancja + ".txt";//pelna nazwa badanej instacji wrazze sciezka
                        makegraph.getFromFile(nazwaPojInstancji, graph, V, solutionFromFile);//wczytywanie do zmiennej graf
                        isUnDirected = makegraph.isSymetric(graph, V);//sprawdzamy czy graf symetryczny
                        if(V < 25) {//dla wiekszych grafow
                            procentageOfLowerBound = 0;
                        } else if(V >= 25 && V <= 74)
                            procentageOfLowerBound = 3;
                        else
                            procentageOfLowerBound = 8;

                        int j;
                        timer2.startCounter();
                        for(j = 0 ; j < iteracjeAlgorytmow ; j++) {//ACO CAS
                            if(timer2.getCounter() > maxCzasAlgorytmow)
                                break;
                            Ants antCAS(timer, alpha, beta, ro, Q, procentageOfLowerBound, iterationsWithoutImprove, solutionFromFile);//ro ---> to parowanie
                            timer.startCounter();
                            antCAS.ACOCAS(graph, V, isUnDirected);
                            double t1 = timer.getCounter();
                            times.push_back(t1);
                            avgTime += t1;
                            int lowestCost = antCAS.getBestCost();
                            solutions.push_back(lowestCost);

                            int absoluteError = antCAS.countAbsoluteError();
                            double relError = antCAS.countRelativeError();
                            absErrorAvg += absoluteError;
                            relativeErrorAvg += relError;
                            absoluteErrors.push_back(absoluteError);
                            relativeErrors.push_back(relError);
                        }
                        avgTime /= j;
                        relativeErrorAvg /= j;
                        absErrorAvg /= j;
                        makeGraph::writeToFileTimesAndAvg(times, absoluteErrors, relativeErrors, solutions, avgTime, absErrorAvg, relativeErrorAvg);//wpisuje do pliku output1.txt
                        makeGraph::writeMainInfoForExcel(V, avgTime, absErrorAvg, relativeErrorAvg, "outputCAS.txt");
                        //czyszczenie
                        times.clear();
                        absoluteErrors.clear();
                        relativeErrors.clear();
                        solutions.clear();


                        avgTime = 0.0;//ACO DAS
                        absErrorAvg = 0.0;
                        relativeErrorAvg = 0.0;
                        timer2.startCounter();
                        for(j = 0 ; j < iteracjeAlgorytmow ; j++) {
                            if(timer2.getCounter() > maxCzasAlgorytmow)
                                break;
                            Ants antDAS(timer, alpha, beta, ro, Q, procentageOfLowerBound, iterationsWithoutImprove, solutionFromFile);
                            timer.startCounter();
                            antDAS.ACODAS(graph, V, isUnDirected);
                            double t1 = timer.getCounter();
                            times.push_back(t1);
                            avgTime += t1;
                            int lowestCost = antDAS.getBestCost();
                            solutions.push_back(lowestCost);

                            int absoluteError = antDAS.countAbsoluteError();
                            double relError = antDAS.countRelativeError();
                            absErrorAvg += absoluteError;
                            relativeErrorAvg += relError;
                            absoluteErrors.push_back(absoluteError);
                            relativeErrors.push_back(relError);
                        }
                        avgTime /= j;
                        relativeErrorAvg /= j;
                        absErrorAvg /= j;
                        makeGraph::writeToFileTimesAndAvg(times, absoluteErrors, relativeErrors, solutions, avgTime, absErrorAvg, relativeErrorAvg);//wpisuje do pliku output1.txt
                        makeGraph::writeMainInfoForExcel(V, avgTime, absErrorAvg, relativeErrorAvg, "outputDAS.txt");
                        //czyszczenie
                        times.clear();
                        absoluteErrors.clear();
                        relativeErrors.clear();
                        solutions.clear();


                        avgTime = 0.0;
                        absErrorAvg = 0.0;
                        relativeErrorAvg = 0.0;
                        timer2.startCounter();
                        for(j = 0 ; j < iteracjeAlgorytmow ; j++) {
                            if(timer2.getCounter() > maxCzasAlgorytmow)
                                break;
                            Ants antQAS(timer, alpha, beta, ro, Q, procentageOfLowerBound, iterationsWithoutImprove, solutionFromFile);
                            timer.startCounter();
                            antQAS.ACOQAS(graph, V, isUnDirected);
                            double t1 = timer.getCounter();
                            times.push_back(t1);
                            avgTime += t1;
                            int lowestCost = antQAS.getBestCost();
                            solutions.push_back(lowestCost);

                            int absoluteError = antQAS.countAbsoluteError();
                            double relError = antQAS.countRelativeError();
                            absErrorAvg += absoluteError;
                            relativeErrorAvg += relError;
                            absoluteErrors.push_back(absoluteError);
                            relativeErrors.push_back(relError);
                        }
                        avgTime /= j;
                        relativeErrorAvg /= j;
                        absErrorAvg /= j;
                        makeGraph::writeToFileTimesAndAvg(times, absoluteErrors, relativeErrors, solutions, avgTime, absErrorAvg, relativeErrorAvg);//wpisuje do pliku output1.txt
                        makeGraph::writeMainInfoForExcel(V, avgTime, absErrorAvg, relativeErrorAvg, "outputQAS.txt");
                        //czyszczenie
                        times.clear();
                        absoluteErrors.clear();
                        relativeErrors.clear();
                        solutions.clear();


                    }
                    z++;//27 opcji
                    std::cout<<"Postep: "<<(static_cast<double>(z)/all)*100.0<<"/100%"<<std::endl;
                }
            }
        }
    } else {//jesli zajecia
        int i = 0;
        while (i < iteracjeAlgorytmow) {
            makeGraph make_graph;//graph/getosc/czyskierowany(false --> inst. syme)/liczba wierzcholkow
            isUnDirected = false;//domyślnie ustawiamy ze graf nie jest skierowany (tzn ze jest sym)
            if (ifFromFile) {
                make_graph.getFromFile(nazwaPlikuWejsciowego, graph, V, solutionFromFile);
                isUnDirected = make_graph.isSymetric(graph, V);//sprawdzamy czy graf symetryczny
                std::cout<<"Czy graf symetryczny: "<< isUnDirected << std::endl;
            } else {
                V = 10;
                make_graph.generateGraph(graph, 100, isUnDirected, V);//czy symetryczny
            }

            std::cout << "Rozwiazanie z pliku: " << solutionFromFile << std::endl;

            Timer timer(maxCzasAlgorytmow);
            timer.startCounter();
            Ants instanceQAS(timer, ALPHA, BETA, EVAPORATION, Q, procentageOfLowerBound, iterationsWithoutImprove, solutionFromFile);//inicjalizacja parametrow przez konstruktor
            instanceQAS.ACOQAS(graph, V, isUnDirected);//odpalenie Algorytmu mrowkowego
            double t4 = timer.getCounter();//przechwycenie upłyniętego czasu
            std::cout<<"QAS: "<<t4<<"ms, najnizszy koszt: "<<instanceQAS.getBestCost()<<std::endl;
            instanceQAS.printBestPath();
            int absError = instanceQAS.countAbsoluteError();
            float relError = instanceQAS.countRelativeError();
            std::cout<<"Bezwzgledny blad: "<<absError<<std::endl;
            std::cout<<"Wzgledny blad: "<<relError<<std::endl;
            std::cout<<"Wzgledny w [%]: "<<relError * 100.0<<std::endl;
            std::cout<<std::endl;

            timer.startCounter();
            Ants instanceCAS(timer, ALPHA, BETA, EVAPORATION, Q, procentageOfLowerBound, iterationsWithoutImprove, solutionFromFile);//inicjalizacja parametrow przez konstruktor
            instanceCAS.ACOCAS(graph, V, isUnDirected);//odpalenie Algorytmu mrowkowego
            double t2 = timer.getCounter();//przechwycenie upłyniętego czasu
            std::cout<<"CAS: "<<t2<<"ms, najnizszy koszt: "<<instanceCAS.getBestCost()<<std::endl;
            instanceCAS.printBestPath();
            absError = instanceCAS.countAbsoluteError();
            relError = instanceCAS.countRelativeError();
            std::cout<<"Bezwzgledny blad: "<<absError<<std::endl;
            std::cout<<"Wzgledny blad: "<<relError<<std::endl;
            std::cout<<"Wzgledny w [%]: "<<relError * 100.0<<std::endl;
            std::cout<<std::endl;

            timer.startCounter();
            Ants instanceDAS(timer,ALPHA, BETA, EVAPORATION, Q, procentageOfLowerBound, iterationsWithoutImprove, solutionFromFile);//inicjalizacja parametrow przez konstruktor
            instanceDAS.ACODAS(graph, V, isUnDirected);//odpalenie Algorytmu mrowkowego
            double t3 = timer.getCounter();//przechwycenie upłyniętego czasu
            std::cout<<"DAS: "<<t3<<"ms, najnizszy koszt: "<<instanceDAS.getBestCost()<<std::endl;
            instanceDAS.printBestPath();
            absError = instanceDAS.countAbsoluteError();
            relError = instanceDAS.countRelativeError();
            std::cout<<"Bezwzgledny blad: "<<absError<<std::endl;
            std::cout<<"Wzgledny blad: "<<relError<<std::endl;
            std::cout<<"Wzgledny w [%]: "<<relError * 100.0<<std::endl;
            std::cout<<std::endl;

            i++;
        }
    }
}
