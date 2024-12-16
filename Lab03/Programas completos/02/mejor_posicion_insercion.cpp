#include <iostream>
#include <vector>
#include <tuple>
#include <limits>
#include <algorithm>

using namespace std;

// Dimensiones máximas (ajustar según tus necesidades)
const int MAX_JOBS = 900;
const int MAX_MACHINES = 70;

// Variables globales
int EF[MAX_JOBS][MAX_MACHINES];
int LS[MAX_JOBS][MAX_MACHINES];
int P[MAX_JOBS][MAX_MACHINES];
int mM; // Número de máquinas

// Función para cargar datos desde un archivo
void cargar(const string& archivo) {
    // Simulación de carga (reemplazar con lectura de archivo real)
    mM = 3; // Número de máquinas
    P[0][0] = 5; P[0][1] = 8; P[0][2] = 6;
    P[1][0] = 7; P[1][1] = 10; P[1][2] = 4;
    P[2][0] = 6; P[2][1] = 7; P[2][2] = 5;
    P[3][0] = 8; P[3][1] = 6; P[3][2] = 9;
    P[4][0] = 9; P[4][1] = 8; P[4][2] = 7;
}

// Función para calcular el makespan de una secuencia dada
int makespan(const vector<int>& S) {
    fill(&EF[0][0], &EF[0][0] + MAX_JOBS * MAX_MACHINES, 0);
    for (int k = 1; k <= S.size(); k++) {
        int j = S[k - 1];
        EF[k][0] = EF[k - 1][0] + P[j][0];
        for (int i = 1; i < mM; i++) {
            EF[k][i] = max(EF[k - 1][i], EF[k][i - 1]) + P[j][i];
        }
    }
    return EF[S.size()][mM - 1];
}

// Función principal de mejor posición de inserción
tuple<int, vector<int>::iterator> MejorPosicionInsercion(vector<int>& S, int nj) {
    // Calcula tiempos de fin tempranos (EF; e de Taillard, 1990)
    fill(&EF[0][0], &EF[0][0] + MAX_JOBS * MAX_MACHINES, 0);
    for (int k = 1; k <= S.size(); k++) {
        int j = S[k - 1];
        EF[k][0] = EF[k - 1][0] + P[j][0];
        for (int i = 1; i < mM; i++) {
            EF[k][i] = max(EF[k - 1][i], EF[k][i - 1]) + P[j][i];
        }
    }

    // Calcula tiempos de fin tempranos relativos (e' de Taillard, 1990)
    for (int k = 1; k <= S.size(); k++) {
        EF[k][0] += P[nj][0];
        for (int i = 1; i < mM; i++) {
            EF[k][i] = max(EF[k][i], EF[k][i - 1]) + P[nj][i];
        }
    }

    // Calcula tiempos de inicio tardíos (LS; q de Taillard, 1990)
    fill(&LS[0][0], &LS[0][0] + MAX_JOBS * MAX_MACHINES, 0);
    for (int k = S.size() - 1; k >= 0; k--) {
        int j = S[k];
        LS[k][mM - 1] = LS[k + 1][mM - 1] + P[j][mM - 1];
        for (int i = mM - 2; i >= 0; i--) {
            LS[k][i] = max(LS[k][i + 1], LS[k + 1][i]) + P[j][i];
        }
    }

    // Busca la posición pos que resulta en min(Cmax_k)
    int bmk = numeric_limits<int>::max(), mk, pos;
    for (int k = 0; k <= S.size(); k++) {
        mk = 0; // Calcula el makespan
        for (int i = 0; i < mM; i++) {
            mk = max(mk, EF[k][i] + LS[k][i]);
        }
        if (mk < bmk) {
            bmk = mk;
            pos = k;
        }
    }
    return {bmk, S.begin() + pos};
}

int main() {
    vector<int> ss;
    cargar("../flowshop/br66");
    cout << "Ejemplo de aceleración de Taillard" << endl;
    ss = {4, 3, 1, 0};
    cout << "Makespan inicial: " << makespan(ss) << endl;
    int mk;
    vector<int>::iterator pj;
    tie(mk, pj) = MejorPosicionInsercion(ss, 2);
    ss.insert(pj, 2);
    cout << "Mejor makespan: " << mk << " - Secuencia: ";
    for (auto& j : ss) cout << j << ",";
    cout << endl;

    return 0;
}
