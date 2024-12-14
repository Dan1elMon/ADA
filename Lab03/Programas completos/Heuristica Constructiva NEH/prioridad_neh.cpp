#include <bits/stdc++.h>
using namespace std;

int nT, mM, P[900][70];
int EF[900][70], LS[900][70];
mt19937 Rand(time(0));

// Carga los datos desde un archivo
void cargar(string nombre) {
    ifstream archivo(nombre);
    archivo >> nT >> mM;
    int basura;
    for (int j = 0; j < nT; j++) {
        for (int i = 0; i < mM; i++) {
            archivo >> basura >> P[j][i];
        }
    }
}

// Calcula el makespan para una secuencia dada
int makespan(vector<int>& S) {
    fill(&EF[0][0], &EF[0][0] + 900 * 70, 0); // Reinicia la matriz EF
    int k = 1; // Índice para los trabajos en la secuencia
    for (auto j : S) {
        EF[k][0] = EF[k - 1][0] + P[j][0]; // Calcula el primer tiempo
        for (int i = 1; i < mM; i++) {
            EF[k][i] = max(EF[k - 1][i], EF[k][i - 1]) + P[j][i];
        }
        k++;
    }
    return EF[S.size()][mM - 1]; // Devuelve el makespan final
}

// Genera una permutación de trabajos con orden no creciente de sus tiempos totales
void PrioridadNEH(vector<int>& Orden) {
    vector<int> TT(nT, 0); // Vector para los tiempos totales

    // Calcula el tiempo total para cada trabajo
    for (int j = 0; j < nT; j++) {
        for (int i = 0; i < mM; i++) {
            TT[j] += P[j][i];
        }
    }

    Orden.resize(nT);
    iota(Orden.begin(), Orden.end(), 0); // Inicializa el vector con {0, 1, ..., nT-1}

    // Ordena los trabajos por tiempo total de mayor a menor
    sort(Orden.begin(), Orden.end(), [&TT](const int& i, const int& j) {
        return (TT[i] > TT[j] || (TT[i] == TT[j] && i < j));
    });
}

int main() {
    vector<int> ss;

    // Carga los datos del archivo especificado
    cargar("../flowshop/br66");

    cout << "Ejemplo de prioridad NEH" << endl;

    // Genera la secuencia de trabajos usando el método NEH
    PrioridadNEH(ss);

    // Muestra la secuencia obtenida
    for (auto& j : ss) {
        cout << j << ",";
    }
    cout << endl;

    return 0;
}