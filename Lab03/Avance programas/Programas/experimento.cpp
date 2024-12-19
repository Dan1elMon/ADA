#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <tuple>
#include <random> // Para std::default_random_engine

using namespace std;

// Variables globales
int nT, mM;
vector<vector<int>> P; // Matriz de tiempos

// Función para medir el tiempo transcurrido
int elapsed(bool reset = false) {
    static clock_t start = clock();
    if (reset) start = clock();
    return (1000.00 * double(clock() - start)) / double(CLOCKS_PER_SEC);
}

// Función para cargar datos
void cargar(const string& filename) {
    ifstream fin(filename);
    fin >> nT >> mM;
    P.resize(nT, vector<int>(mM));
    for (int i = 0; i < nT; ++i) {
        for (int j = 0; j < mM; ++j) {
            fin >> P[i][j];
        }
    }
    fin.close();
}

// Simulación de NEH (se debe implementar el algoritmo de NEH real)

void NEH(std::vector<int>& S) {
    std::random_device rd; // Generador aleatorio basado en hardware (si está disponible)
    std::default_random_engine rng(rd()); // Motor de números aleatorios
    std::shuffle(S.begin(), S.end(), rng); // Sustituye random_shuffle por shuffle
}


// Búsqueda local (simulada, se debe implementar el algoritmo real)
int BusquedaLocal(vector<int>& S) {
    return rand() % 1000; // Devuelve un valor aleatorio como resultado
}

// Función de Mejor Inserción (simulada)
pair<int, vector<int>::iterator> MejorInsercion(vector<int>& NS, int j) {
    int costo = rand() % 1000;
    auto pos = NS.begin() + rand() % (NS.size() + 1);
    return {costo, pos};
}

// Implementación del método ILS_B
int ILS_B(vector<int>& BS) {
    vector<int> S;
    int mk, bmk;
    elapsed(true);
    NEH(S);
    bmk = mk = BusquedaLocal(S);
    BS = S;

    while (elapsed() < 15 * nT * mM) {
        auto b1 = S.begin() + rand() % S.size();
        auto b2 = S.begin() + rand() % S.size();
        swap(*b1, *b2);
        mk = BusquedaLocal(S);

        if (bmk > mk) {
            BS = S;
            bmk = mk;
        } else {
            S = BS;
            mk = bmk;
        }
    }
    return bmk;
}

// Implementación del método ILS_SA
int ILS_SA(vector<int>& BS) {
    vector<int> S, NS;
    int mk, bmk, nmk;
    int sum_p = 0;

    for (int j = 0; j < nT; ++j) {
        for (int i = 0; i < mM; ++i) {
            sum_p += P[j][i];
        }
    }

    double T = double(sum_p) / (nT * mM * 25);
    elapsed(true);
    NEH(S);
    bmk = mk = BusquedaLocal(S);
    BS = S;

    while (elapsed() < 15 * nT * mM) {
        NS = S;
        auto b1 = NS.begin() + rand() % NS.size();
        auto b2 = NS.begin() + rand() % NS.size();
        swap(*b1, *b2);
        nmk = BusquedaLocal(NS);

        if (nmk < mk) {
            S = NS;
            mk = nmk;
            if (mk < bmk) {
                BS = S;
                bmk = mk;
            }
        } else if (double(rand()) / RAND_MAX <= exp(-(double(nmk - mk) / T))) {
            S = NS;
            mk = nmk;
        }
    }
    return bmk;
}

// Experimento para evaluar los métodos
typedef int (*tMetodo)(vector<int>&);

void Experimento(const string& sMetodo, tMetodo pMetodo) {
    vector<int> SS;
    ofstream fout(sMetodo + ".txt");
    fout << sMetodo << endl;

    vector<string> Instancias = {
        "ta051", "ta052", "ta053", "ta054", "ta055", "ta056", "ta057", "ta058", "ta059", "ta060"
    };

    for (auto& instancia : Instancias) {
        cargar("flowshop/" + instancia);

        fout << instancia;
        for (int i = 0; i < 10; ++i) { // Repetir 10 experimentos por instancia
            fout << ", " << (*pMetodo)(SS);
        }
        fout << endl;
    }
    fout.close();
}

int main() {
    Experimento("ilsb", ILS_B);
    Experimento("ilssa", ILS_SA);
    return 0;
    //Experimento ("ilsb",ILS_B);//better
    //Experimento ("ilsrw",ILS_RW);//randomwalk
    //Experimento ("ilssa",ILS_SA);simmulated annealing
    //Experimento ("ig",IG);iterated greedy
    //Experimento ("igsin",IGsin); iterated greedy sin busqueda local
    
}
