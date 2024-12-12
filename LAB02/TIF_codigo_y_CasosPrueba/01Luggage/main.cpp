#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
/*
    Luggage.cpp es el problema presentado
    main.cpp es el problema resuelto y implementado para realizar pruebas
    los 3 primeros casos son los ejempllos propuestos en el documento de la pagina
*/
using namespace std;

int wt[21];
int C[101];

int divide(int n, int W) {
    for(int i = 0; i <= W; i++) C[i] = 0;
    for(int i = 1; i <= n; i++)
        for(int j = W; j >= wt[i]; j--)
            C[j] = max(C[j], wt[i] + C[j - wt[i]]);
    return C[W];
}

int main() {
    ifstream archivoEntrada("prueba10.csv");
        //Los 3 primeros casos, son los ejemplos propuestos en el documento de la pagina

    ofstream archivoSalida("resultados10.csv");
    
    if (!archivoEntrada.is_open()) {
        cerr << "No se pudo abrir el archivo de entrada.\n";
        return 1;
    }
    if (!archivoSalida.is_open()) {
        cerr << "No se pudo abrir el archivo de salida.\n";
        return 1;
    }

    int test;
    string linea;
    archivoSalida << "Prueba,Resultado\n"; // Encabezados del archivo CSV

    // Leer el número de pruebas desde el archivo
    getline(archivoEntrada, linea);
    test = stoi(linea);

    for (int t = 1; t <= test; t++) {
        if (!getline(archivoEntrada, linea)) break; // Leer cada línea de prueba
        stringstream ss(linea);
        
        int el, i = 0, sum = 0;
        while (ss >> el) {
            wt[++i] = el;
            sum += el;
        }

        string resultado;
        if ((sum & 1) == 0) {
            if (divide(i, sum >> 1) == (sum >> 1))
                resultado = "YES";
            else
                resultado = "NO";
        } else {
            resultado = "NO";
        }

        // Escribir el número de prueba y el resultado en el archivo de salida
        archivoSalida << t << "," << resultado << "\n";
    }

    archivoEntrada.close();
    archivoSalida.close();

    return 0;
}
