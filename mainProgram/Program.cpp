//
// Created by Piotr on 10.12.2024.
//

#include "Program.h"

#include "Timer.h"
#include "../Algorithms/Ants.h"

Program::Program() {
    solutionFromFile = -1;
}

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
    NUM_ANTS = std::stoi(konfiguracja["liczba mrowek:"]);
    ALPHA = std::stod(konfiguracja["alpha:"]);       // Wpływ feromonu
    BETA = std::stod(konfiguracja["beta:"]);         // Wpływ odległości
    EVAPORATION = std::stod(konfiguracja["wspolczynnik parowania feromonu:"]);  // Współczynnik parowania feromonu
    Q = std::stod(konfiguracja["stala:"]);
    plik.close();
}


void Program::glownyProgram() {
    wczytanieZPlikuKonfiguracyjnego();
    std::cout << "max czas wykonywania algorytmu[ms]: " << maxCzasAlgorytmow << "\n";
    std::cout << "liczba powtorzen algorytmow: " << iteracjeAlgorytmow << "\n";
    std::cout << "Nazwa pliku we: " << nazwaPlikuWejsciowego << std::endl;
    std::cout << "Nazwa pliku wy: " << nazwaPlikuWyjsciowego << std::endl;
    std::cout << "Procent dolnego ograniczenia: " << procentageOfLowerBound << std::endl;
    std::cout << "Ilosc iteracji bez poprawy: " << iterationsWithoutImprove << std::endl;
    std::cout << "ilosc mrowek: " << NUM_ANTS << std::endl;
    std::cout << "alfa: " << ALPHA << std::endl;
    std::cout << "beta: " << BETA << std::endl;
    std::cout << "wspolczynnik parowania feromonu: " << EVAPORATION << std::endl;
    std::cout << "stala: " << Q << std::endl;


    std::vector<std::vector<int>> graph;
    int V = 100;

    int i = 0;
    while (i < 1) {
        makeGraph make_graph;//graph/getosc/czyskierowany(false --> inst. syme)/liczba wierzcholkow
        isUnDirected = false;//domyślnie ustawiamy ze graf nie jest skierowany (tzn ze jest sym)
        if (ifFromFile) {
            make_graph.getFromFile(nazwaPlikuWejsciowego, graph, V, solutionFromFile);
            isUnDirected = make_graph.isSymetric(graph, V);//sprawdzamy czy graf symetryczny
            std::cout<<"Czy graf symetryczny: "<< isUnDirected << std::endl;
        } else {
            make_graph.generateGraph(graph, 100, isUnDirected, V);//czy symetryczny
        }

        std::cout << "Rozwiazanie z pliku: " << solutionFromFile << std::endl;

        Timer timer(maxCzasAlgorytmow);
        timer.startCounter();
        Ants instanceQAS(timer, NUM_ANTS, ALPHA, BETA, EVAPORATION, Q, procentageOfLowerBound, iterationsWithoutImprove, solutionFromFile);//inicjalizacja parametrow przez konstruktor
        instanceQAS.ACOQAS(graph, V, isUnDirected);//odpalenie Algorytmu mrowkowego
        double t4 = timer.getCounter();//przechwycenie upłyniętego czasu
        std::cout<<"QAS: "<<t4<<"ms, najnizszy koszt: "<<instanceQAS.getBestCost()<<std::endl;

        timer.startCounter();
        Ants instanceCAS(timer, NUM_ANTS, ALPHA, BETA, EVAPORATION, Q, procentageOfLowerBound, iterationsWithoutImprove, solutionFromFile);//inicjalizacja parametrow przez konstruktor
        instanceCAS.ACOCAS(graph, V, isUnDirected);//odpalenie Algorytmu mrowkowego
        double t2 = timer.getCounter();//przechwycenie upłyniętego czasu
        std::cout<<"CAS: "<<t2<<"ms, najnizszy koszt: "<<instanceCAS.getBestCost()<<std::endl;

        timer.startCounter();
        Ants instanceDAS(timer, NUM_ANTS, ALPHA, BETA, EVAPORATION, Q, procentageOfLowerBound, iterationsWithoutImprove, solutionFromFile);//inicjalizacja parametrow przez konstruktor
        instanceDAS.ACODAS(graph, V, isUnDirected);//odpalenie Algorytmu mrowkowego
        double t3 = timer.getCounter();//przechwycenie upłyniętego czasu
        std::cout<<"DAS: "<<t3<<"ms, najnizszy koszt: "<<instanceDAS.getBestCost()<<std::endl;

        i++;
    }
}
