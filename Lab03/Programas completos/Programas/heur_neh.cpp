#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <algorithm>
#include <random>
#include <limits>
using namespace std;

random_device rd;
mt19937 Rand(rd());

// Variables globales
vector<vector<int>> tiempos;
int nT = 0; // Número de trabajos
int nM = 0; // Número de máquinas

const int MAX_JOBS = 100;
const int MAX_MACHINES = 50;
int P[MAX_JOBS][MAX_MACHINES]; // Tiempos de procesamiento
int EF[MAX_JOBS][MAX_MACHINES]; // Early Finish
int LS[MAX_JOBS][MAX_MACHINES]; // Late Start

// Cargar instancias de problemas
void cargar(const string& path) {
    // Simulación: cargar "nT" trabajos y "nM" máquinas desde el archivo
    // Aquí deberías implementar la lectura del archivo "path"
    tiempos = {
        {3, 2, 4},
        {1, 3, 2},
        {4, 2, 1},
        {2, 4, 3}
    };
    nT = tiempos.size();
    nM = tiempos[0].size();

    // Inicialización para la otra representación
    for (int i = 0; i < nT; ++i) {
        for (int j = 0; j < nM; ++j) {
            P[i][j] = tiempos[i][j];
        }
    }
}

// Calcular el tiempo total para un orden dado
int makespan(const vector<int>& secuencia) {
    vector<int> tiempo_maquina(nM, 0);
    for (int job : secuencia) {
        tiempo_maquina[0] += P[job][0];
        for (int m = 1; m < nM; ++m) {
            tiempo_maquina[m] = max(tiempo_maquina[m], tiempo_maquina[m - 1]) + P[job][m];
        }
    }
    return tiempo_maquina[nM - 1];
}

// Función para calcular prioridades de NEH
void PrioridadNEH(vector<int>& orden) {
    vector<pair<int, int>> suma_tiempos;
    for (int i = 0; i < nT; ++i) {
        int suma = 0;
        for (int j = 0; j < nM; ++j) {
            suma += tiempos[i][j];
        }
        suma_tiempos.push_back({suma, i});
    }
    sort(suma_tiempos.rbegin(), suma_tiempos.rend()); // Orden descendente
    for (const auto& [suma, index] : suma_tiempos) {
        orden.push_back(index);
    }
}

// Encontrar la mejor posición para insertar un trabajo
tuple<int, vector<int>::iterator> MejorPosicionInsercion(vector<int>& secuencia, int trabajo) {
    int mejor_makespan = numeric_limits<int>::max();
    auto mejor_pos = secuencia.end();
    for (auto it = secuencia.begin(); it <= secuencia.end(); ++it) {
        vector<int> prueba = secuencia;
        prueba.insert(it, trabajo);
        int mk = makespan(prueba);
        if (mk < mejor_makespan) {
            mejor_makespan = mk;
            mejor_pos = it;
        }
    }
    return {mejor_makespan, mejor_pos};
}

// Implementación del algoritmo NEH
int NEH(vector<int>& S) {
    int mk;
    vector<int> orden;
    PrioridadNEH(orden);
    S = {orden[0]};
    for (int k = 1; k < nT; ++k) {
        vector<int>::iterator pos;
        tie(mk, pos) = MejorPosicionInsercion(S, orden[k]);
        S.insert(pos, orden[k]);
    }
    return mk;
}

// Búsqueda Local
int BusquedaLocal(vector<int>& S, int pmk = 0) {
    vector<int> orden(S);
    shuffle(orden.begin(), orden.end(), Rand);
    int k = 0; // Índice para orden
    int c = 0; // Contador de iteraciones sin mejora
    int bmk = pmk; // Última mejora

    if (bmk == 0) {
        bmk = makespan(S);
    }

    do {
        int mk;
        vector<int>::iterator pos;

        // Eliminar el elemento de S correspondiente al índice en orden
        S.erase(find(S.begin(), S.end(), orden[k]));

        // Encontrar la mejor posición de inserción
        tie(mk, pos) = MejorPosicionInsercion(S, orden[k]);

        // Insertar el elemento en la mejor posición encontrada
        S.insert(pos, orden[k]);

        // Si se encuentra una mejora
        if (mk < bmk) {
            bmk = mk;
            c = 0;
        }

        k++;
        if (k >= (int)orden.size()) {
            k = 0;
        }

        c++;
    } while (c < (int)orden.size());

    return bmk;
}

int main() {
    vector<int> ss;
    vector<string> Instancias = {"br66", "ta021", "ta022", "ta023", "ta024", "ta025", "ta026", "ta027", "ta028", "ta029", "ta030"};
    cout << "Soluciones producidas por NEH" << endl;
    for (auto& instancia : Instancias) {
        cargar("flowshop/" + instancia);
        cout << instancia << "\t" << NEH(ss) << ": ";
        for (auto& j : ss) cout << j << ", ";
        cout << endl;
    }
    return 0;
}
