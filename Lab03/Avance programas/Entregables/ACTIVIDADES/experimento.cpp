#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <tuple>
#include <limits>
#include <ctime>
using namespace std;

// definicion de las matrices globales
int nT, mM, P[900][70];  
int EF[900][70], LS[900][70];  // Matrices de tiempos de finalizacion tempranos y tardios
mt19937 Rand(time(0));  // Generador de numeros aleatorios

inline int elapsed(bool reset = false) {
    static clock_t start = clock();
    if (reset) {
        start = clock();
    }
    return static_cast<int>(1000.00 * double(clock() - start) / double(CLOCKS_PER_SEC));
}

// Funcion para cargar datos desde un archivo
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
int makespan(vector<int>& S) {
    fill(&EF[0][0], &EF[0][0] + 900 * 70, 0);  // Inicializa toda la matriz EF
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
    // Inicializa la matriz EF
    fill(&EF[0][0], &EF[0][0] + 900 * 70, 0);

    // calcular tiempos de finalizacion tempranos
    for (int k = 1; k <= S.size(); k++) {
        int j = S[k - 1];
        EF[k][0] = EF[k - 1][0] + P[j][0];
        for (int i = 1; i < mM; i++) {
            EF[k][i] = max(EF[k - 1][i], EF[k][i - 1]) + P[j][i];
        }
    }

    // calcula los tiempos de finalizacion tempranos relativos para el nuevo trabajo nj
    for (int k = 0; k <= S.size(); k++) {
        EF[k][0] += P[nj][0];
        for (int i = 1; i < mM; i++) {
            EF[k][i] = max(EF[k][i], EF[k][i - 1]) + P[nj][i];
        }
    }

    // Calcula los tiempos de inicio tardios LS
    fill(&LS[S.size()][0], &LS[S.size()][0] + 70, 0);
    for (int k = S.size() - 1; k >= 0; k--) {
        int j = S[k];
        LS[k][mM - 1] = LS[k + 1][mM - 1] + P[j][mM - 1];
        for (int i = mM - 2; i >= 0; i--) {
            LS[k][i] = max(LS[k][i + 1], LS[k + 1][i]) + P[j][i];
        }
    }

    // Busca la mejor posicion para minimizar el makespan
    int bmk = numeric_limits<int>::max(), mk, pos;
    for (int k = 0; k <= S.size(); k++) {
        mk = 0;
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

int BusquedaLocal(vector<int>& S, int pmk = 0) {
    vector<int> orden(S);
    shuffle(orden.begin(), orden.end(), Rand);
    int bmk = pmk == 0 ? makespan(S) : pmk;  // Calcula el makespan inicial si no se pasa pmk

    int c = 0; 
    int k = 0;  

    do {
        int mk;
        vector<int>::iterator pos;
        
        S.erase(find(S.begin(), S.end(), orden[k]));  // Elimina el trabajo
        tie(mk, pos) = MejorPosicionInsercion(S, orden[k]);  // Encuentra la mejor posicion
        S.insert(pos, orden[k]);  // Inserta el trabajo en la mejor posicion

        if (mk < bmk) {
            bmk = mk;
            c = 0;  // Reinicia el contador de iteraciones
        }
        
        k++;
        if (k >= nT) {
            k = 0;
        }

        c++;
    } while (c < nT);  // Termina si no mejora en nT iteraciones

    return bmk; 
}



// Genera una permutacion de trabajos segun la prioridad NEH
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



// Funcion NEH 
int NEH(vector<int> &S) {
    int mk;
    vector<int> orden;
    PrioridadNEH(orden);
    S = {orden[0]};
    for (int k = 1; k < nT; k++) {
        vector<int>::iterator pos;
        tie(mk, pos) = MejorPosicionInsercion(S, orden[k]); 
        S.insert(pos, orden[k]);
    }
    return mk;
}

//asd
// Criterio de aceptacion de ILS _better
int ILS_B(vector<int>& BS) {
    vector<int> S;
    int mk, bmk;

    elapsed(true);

    NEH(S);
    
    // Calcula el makespan de la solucion inicial usando BusquedaLocal
    bmk = mk = BusquedaLocal(S);
    BS = S;  // Guarda la mejor solucion hasta ahora

    // bucle busqueda iterativa hasta que el tiempo transcurrido alcance el limite
    while (elapsed() < 15 * nT * mM) {
        // Realiza dos intercambios aleatorios en la solucion S
        auto b1 = S.begin() + Rand() % S.size();
        auto b2 = S.begin() + Rand() % S.size();
        swap(*b1, *b2);  // Primer intercambio

        b1 = S.begin() + Rand() % S.size();
        b2 = S.begin() + Rand() % S.size();
        swap(*b1, *b2);  // Segundo intercambio

        //si la nueva solucion es mejor
        mk = BusquedaLocal(S);
        if (bmk > mk) {
            BS = S;  // Actualiza 
            bmk = mk;
        } else {
            S = BS;  // Restaura la mejor solucion encontrada
            mk = bmk;
        }
    }

    // Retorna el mejor makespan encontrado
    return bmk;
}
//asd
// Criterio de aceptacion de ILS rANDON WALK
int ILS_RW(vector<int>& BS) {
    vector<int> S;
    int mk, bmk;

    // Inicializa el tiempo transcurrido a 0
    elapsed(true);

    // Ejecuta  NEH para obtener una solucion inicial
    NEH(S);
    
    // Calcula el makespan 
    bmk = mk = BusquedaLocal(S);
    BS = S;  // Guarda la mejor solucion hasta ahora

    // bucle de busqueda iterativa hasta que el tiempo transcurrido alcance el limite
    while (elapsed() < 15 * nT * mM) {
        // Realiza dos intercambios aleatorios en la solucion S
        auto b1 = S.begin() + Rand() % S.size();
        auto b2 = S.begin() + Rand() % S.size();
        swap(*b1, *b2);  //  intercambio

        b1 = S.begin() + Rand() % S.size();
        b2 = S.begin() + Rand() % S.size();
        swap(*b1, *b2);  // Segundo intercambio

        // Evalua si la nueva solucion es mejor
        mk = BusquedaLocal(S);
        if (bmk > mk) {
            BS = S;
            bmk = mk;
        } /*else {
            S = BS;  //diferencia 
            mk = bmk;
        }*/
    }

    // Retorna el mejor makespan encontrado
    return bmk;
}


//asd
int IG_conLS(vector<int>&BS){
    vector<int>S,NS,R;
    R.resize(4);
    int mk,bmk,nmk;
    int sum_p=0;
    for(int j=0; j<nT;j++){
        for(int i=0; i<mM;i++){
            sum_p+=P[i][j];
        }
    }
    double T=double (sum_p)/(nT*mM*25);
    elapsed(true);
    bmk = mk =NEH(S);
    bmk = mk = BusquedaLocal(S);
    BS=S;

    while(elapsed()<15*nT*mM){

        NS=S;
        for(auto &j:R){
            auto b1=NS.begin()+Rand()%NS.size();
            j=*b1;
            NS.erase(b1);
        }
        for(auto &j:R){
            vector<int>::iterator pos;
            tie(nmk,pos)=MejorPosicionInsercion(NS,j);
            NS.insert(pos,j);

        }
        mk=BusquedaLocal(NS,nmk);
    }
    return bmk;
}




//asd
int ILS_SA(vector<int>& BS){
    vector<int>S, NS;
    int mk, bmk, nmk;

    int sum_p=0;

    for(int j=0;j<nT; j++ ){
        for(int i=0; i<mM;i++){
            sum_p+=P[j][i];
        }
    }
    double T=double (sum_p)/(nT*mM*25);
    elapsed(true);
    bmk=mk=NEH(S);
    bmk=mk=BusquedaLocal(S);
    BS=S;

    while(elapsed()<15*nT*mM){
        NS=S;
        auto b1 =NS.begin()+Rand()%NS.size();
        auto b2 =NS.begin()+Rand()%NS.size();
        swap(*b1,*b2);

        b1=NS.begin()+Rand()%NS.size();
        b2=NS.begin()+Rand()%NS.size();
        swap(*b1,*b2);

        nmk=BusquedaLocal(NS);
       
        if(nmk <mk){
            S=NS;mk = nmk;
            if(mk<bmk){
                BS=S;bmk=mk;
            }
        }
        else if(double(Rand())/Rand.max()<=exp(-(double(nmk-mk )/T))){
            S=NS;mk=nmk;
        }
    }
    return bmk;
}


//asd
int IG_noLs(vector<int>&BS){
    vector<int> S,NS, R;
    R.resize(4);
    int mk, bmk,nmk;
    int sum_p=0;
    
    for(int j=0; j<nT;j++)for(int i=0;i<mM;i++){
        sum_p=P[j][i];}

    double T=double (sum_p)/(nT*mM*25);
    elapsed(true);
    bmk=mk=NEH(S);
    //bmk = mk=BusquedaLocal(S);
    BS=S;
    while(elapsed()<15*nT*mM){
        NS=S;
        for(auto&j : R){
            auto b1=NS.begin()+Rand()%NS.size();
            j=*b1;
            NS.erase(b1);

        }
        for(auto &j: R){
            vector<int>::iterator pos;
            tie(nmk,pos)=MejorPosicionInsercion(NS,j);
            NS.insert(pos,j);

        }
        //nmk = BusquedaLocal(NS,nmk);
    }
   
   return bmk;
}


typedef int(*tMetodo)(vector<int> &);

void Experimento (string sMetodo, tMetodo pMetodo) {
    vector<int> SS;
    ofstream fout(sMetodo + ".txt");
    fout << sMetodo << endl;

    vector<string> Instancias = {
        "ta051", /*"ta052", "ta053", "ta054", "ta055", "ta056", "ta057", "ta058", "ta059", "ta060"*/
    };

    for (const auto &instancia : Instancias) {
        cargar("flowshop/" + instancia);

        fout << instancia;
        /*for (int i = 0; i < SS.size(); i++) {  */
            fout << ", " << (*pMetodo)(SS);
        //}
       /* fout << endl;*/
    }
    fout.close();
}

//asd

int main(){

    cout << "Iterated Local search better" << endl;
    Experimento ("00_ilsb_prueba",ILS_B);

    cout << "Iterated Local search randwom walk" << endl;
    Experimento ("01_ilsrw_prueba ",ILS_RW);

    cout << "Iterated Local search Simulated annealing" << endl;
    Experimento ("02_ilssa_prueba",ILS_SA);

    cout << "Iterated greedy con ls" << endl;
    Experimento ("03_ig_prueba",IG_conLS);

    cout << "Iterated greedy sin ls" << endl;
    Experimento ("04_igsin_prueba",IG_noLs);

}