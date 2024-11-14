#include <iostream>
#include <fstream>
#include <cstring>
#include <numeric>
#include <vector>
#include <sstream>
using namespace std;

/*
    Square.cpp es el problema presentado
    main03.cpp es el problema resuelto y implementado para realizar pruebas
    los 3 primeros casos son los ejempllos propuestos en el documento de la pagina
*/

int M, sum, sticks[20];
int memo[1 << 20];

int search(int length, int bitMask) {
    if (memo[bitMask] != -1)
        return memo[bitMask];
    if (length > sum / 4)
        return 0;
    else if (length == sum / 4) {
        if (bitMask == (1 << M) - 1)
            return 1;
        length = 0;
    }
    for (int i = 0; i < M; ++i)
        if (((bitMask & (1 << i)) == 0) && search(length + sticks[i], bitMask | (1 << i))) {
            return memo[bitMask] = 1;
        }
    return memo[bitMask] = 0;
}

int solve() {
    sum = accumulate(sticks, sticks + M, 0);
    if (sum % 4 != 0)
        return 0;
    memset(memo, -1, sizeof(memo));
    return search(0, 0);
}

int main() {
    ifstream inputFile("input10.csv");
    //Los 3 primeros casos, son los ejemplos propuestos en el documento de la pagina
    ofstream outputFile("output10.csv");
    
    if (!inputFile.is_open() || !outputFile.is_open()) {
        cerr << "Error al abrir los archivos." << endl;
        return 1;
    }

    string line;
    getline(inputFile, line); // Leer la primera línea para obtener N

    int N;
    stringstream ss(line);
    ss >> N; // Número de casos de prueba

    outputFile << "Case,Result" << endl;
    for (int caseNum = 1; caseNum <= N; ++caseNum) {
        if (!getline(inputFile, line)) break;
        
        stringstream caseStream(line);
        string value;

        // Leer el valor de M
        getline(caseStream, value, ',');
        M = stoi(value);

        // Leer las longitudes de los palos
        for (int i = 0; i < M; ++i) {
            getline(caseStream, value, ',');
            sticks[i] = stoi(value);
        }

        string result = solve() ? "yes" : "no";
        outputFile << caseNum << "," << result << endl;
    }

    inputFile.close();
    outputFile.close();
    return 0;
}
