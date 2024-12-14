#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <tuple>
using namespace std;

// Variables globales
int nT, mM, P[900][70];
int EF[900][70], LS[900][70];
mt19937 Rand(time(0));

// Función para cargar datos desde un archivo
void cargar(string nombre) {
    ifstream archivo(nombre);
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo: " << nombre << endl;
        exit(1);
    }
    archivo >> nT >> mM;
    for (int j = 0; j < nT; j++) {
        for (int i = 0; i < mM; i++) {
            archivo >> P[j][i];
        }
    }
    archivo.close();
}

// Calcula el makespan para un orden dado
int makespan(vector<int> &S) {
    fill(&EF[0][0], &EF[0][0] + 900 * 70, 0); // Inicializa toda la matriz
    int k = 1;
    for (auto j : S) {
        EF[k][0] = EF[k - 1][0] + P[j][0];
        for (int i = 1; i < mM; i++) {
            EF[k][i] = max(EF[k - 1][i], EF[k][i - 1]) + P[j][i];
        }
        k++;
    }
    return EF[S.size()][mM - 1];
}
tuple<int, vector<int>::iterator> MejorPosicionInsercion(vector<int> &S, int nj) {
    // Calcula tiempos de fin tempranos (EF; e de Tailard, 1990)
    fill(&EF[0][0], &EF[0][0] + 900 * 70, 0); // Inicializa la matriz EF
    for (int k = 1; k <= S.size(); k++) {
        int j = S[k - 1];
        EF[k][0] = EF[k - 1][0] + P[j][0];
        for (int i = 1; i < mM; i++) {
            EF[k][i] = max(EF[k - 1][i], EF[k][i - 1]) + P[j][i];
        }
    }

    // Calcula tiempos de fin tempranos relativos (e' de Tailard, 1990)
    // para el nuevo trabajo `nj` en cada posición de inserción en `S`
    for (int k = 0; k <= S.size(); k++) {
        EF[k][0] += P[nj][0];
        for (int i = 1; i < mM; i++) {
            EF[k][i] = max(EF[k][i], EF[k][i - 1]) + P[nj][i];
        }
    }

    // Calcula tiempos de inicio tardíos (LS; q de Tailard, 1990)
    fill(&LS[S.size()][0], &LS[S.size()][0] + 70, 0); // Inicializa la última fila de LS
    for (int k = S.size() - 1; k >= 0; k--) {
        int j = S[k];
        LS[k][mM - 1] = LS[k + 1][mM - 1] + P[j][mM - 1];
        for (int i = mM - 2; i >= 0; i--) {
            LS[k][i] = max(LS[k][i + 1], LS[k + 1][i]) + P[j][i];
        }
    }

    // Busca la posición `pos` que resulta en min(Cmax_k),
    // donde Cmax_k = max_i(e' + q) de Tailard (1990)
    int bmk = numeric_limits<int>::max(), mk, pos;
    for (int k = 0; k <= S.size(); k++) {
        mk = 0; // Calcula el makespan máximo
        for (int i = 0; i < mM; i++) {
            mk = max(mk, EF[k][i] + LS[k][i]);
        }
        // Actualiza el mejor makespan y su posición
        if (mk < bmk) {
            bmk = mk;
            pos = k;
        }
    }

    return {bmk, S.begin() + pos};
}

// Genera una permutación de trabajos según la prioridad NEH
void PrioridadNEH(vector<int> &Orden) {
    vector<int> TT(nT, 0);
    for (int j = 0; j < nT; j++) {
        for (int i = 0; i < mM; i++) {
            TT[j] += P[j][i];
        }
    }
    Orden.resize(nT);
    iota(Orden.begin(), Orden.end(), 0); // Llena con {0, 1, ..., nT-1}
    sort(Orden.begin(), Orden.end(), [&TT](const int &i, const int &j) {
        return (TT[i] > TT[j] || (TT[i] == TT[j] && i < j));
    });
}

// Función NEH principal
int NEH(vector<int> &S) {
    int mk;
    vector<int> orden;
    PrioridadNEH(orden);
    S = {orden[0]};
    for (int k = 1; k < nT; k++) {
        vector<int>::iterator pos;
        tie(mk, pos) = MejorPosicionInsercion(S, orden[k]); // Necesitas implementar esta función
        S.insert(pos, orden[k]);
    }
    return mk;
}

int main() {
    vector<int> ss;
    vector<string> Instancias = {"br66", "ta021", "ta022", "ta023", "ta024", "ta025", "ta026", "ta027", "ta028", "ta029", "ta030"};
    cout << "Soluciones producidas por NEH" << endl;
    for (auto &instancia : Instancias) {
        cargar("flowshop/" + instancia);
        cout << instancia << "\t" << NEH(ss) << ": ";
        for (auto &j : ss) cout << j << ", ";
        cout << endl;
    }
    return 0;
}
