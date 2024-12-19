#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <tuple>
#include <limits>
#include <ctime> // Para usar clock_t y clock()

using namespace std;

random_device rd;
mt19937 Rand(rd());

const int MAX_JOBS = 100;
const int MAX_MACHINES = 10;
int P[MAX_JOBS][MAX_MACHINES]; // Tiempos de procesamiento
int mM; // Número de máquinas

// Variables para la ejecución del tiempo
int nT = MAX_JOBS; // Número de trabajos
int nM = MAX_MACHINES; // Número de máquinas

// Función para medir el tiempo transcurrido en milisegundos
int elapsed(bool reset = false) {
    static clock_t start = clock(); // Almacena el tiempo de inicio
    if (reset) start = clock(); // Resetea el temporizador
    return (1000.0 * double(clock() - start)) / double(CLOCKS_PER_SEC); // Devuelve el tiempo en milisegundos
}

int makespan(const vector<int>& S) {
    vector<vector<int>> EF(S.size() + 1, vector<int>(mM, 0));

    for (int k = 1; k <= (int)S.size(); k++) {
        int job = S[k - 1];
        EF[k][0] = EF[k - 1][0] + P[job][0];

        for (int i = 1; i < mM; i++) {
            EF[k][i] = max(EF[k - 1][i], EF[k][i - 1]) + P[job][i];
        }
    }

    return EF[S.size()][mM - 1];
}

tuple<int, vector<int>::iterator> MejorPosicionInsercion(vector<int>& S, int elemento) {
    int bmk = numeric_limits<int>::max();
    vector<int>::iterator mejor_pos = S.begin();

    for (auto it = S.begin(); it <= S.end(); ++it) {
        S.insert(it, elemento);
        int mk = makespan(S);
        if (mk < bmk) {
            bmk = mk;
            mejor_pos = it;
        }
        S.erase(it);
    }

    return make_tuple(bmk, mejor_pos);
}

void cargar(const string& instancia) {
    // Simulación de datos (reemplazar por la carga real de archivo)
    if (instancia == "flowshop/br66") {
        mM = 3;
        P[0][0] = 5; P[0][1] = 8; P[0][2] = 6;
        P[1][0] = 7; P[1][1] = 10; P[1][2] = 4;
        P[2][0] = 6; P[2][1] = 7; P[2][2] = 5;
        P[3][0] = 8; P[3][1] = 6; P[3][2] = 9;
        P[4][0] = 9; P[4][1] = 8; P[4][2] = 7;
    }
}

int NEH(vector<int>& S) {
    vector<pair<int, int>> sumas;

    for (int j = 0; j < MAX_JOBS; j++) {
        int suma = 0;
        for (int i = 0; i < mM; i++) {
            suma += P[j][i];
        }
        sumas.emplace_back(suma, j);
    }

    sort(sumas.rbegin(), sumas.rend());
    S.clear();
    for (auto& [_, job] : sumas) {
        S.push_back(job);
    }

    return makespan(S);
}

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

int ILS_B(vector<int>& BS) {
    vector<int> S;
    int mk, bmk;

    elapsed(true); // Resetea el temporizador
    NEH(S);
    bmk = mk = BusquedaLocal(S);
    BS = S;

    // Búsqueda Local Iterativa (ILS) con perturbaciones
    while (elapsed() < 15 * nT * nM) {
        // Perturbación: intercambio de dos elementos aleatorios
        auto b1 = S.begin() + Rand() % S.size();
        auto b2 = S.begin() + Rand() % S.size();
        swap(*b1, *b2);

        // Otra perturbación: intercambio de otros dos elementos aleatorios
        b1 = S.begin() + Rand() % S.size();
        b2 = S.begin() + Rand() % S.size();
        swap(*b1, *b2);

        // Evaluar el makespan de la nueva solución
        mk = makespan(S);

        // Aceptación del criterio de mejora
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

int main() {
    vector<int> ss;
    vector<string> Instancias = {"br66", "ta021", "ta022", "ta023", "ta024", "ta025", "ta026", "ta027", "ta028", "ta029", "ta030"};

    cout << "Soluciones luego de una búsqueda local iterativa (ILS)" << endl;

    for (auto& instancia : Instancias) {
        cargar("flowshop/" + instancia);
        int mk = NEH(ss);
        mk = BusquedaLocal(ss, mk);

        // Medir el tiempo de ejecución para ILS
        cout << "Tiempo de ejecución para " << instancia << ": " << elapsed(true) << " ms" << endl;

        // Llamada a la función ILS_B
        vector<int> BS;
        mk = ILS_B(BS);

        cout << instancia << "\t" << mk << ": ";
        for (auto& j : BS) {
            cout << j << ", ";
        }
        cout << endl;
    }

    return 0;
}
