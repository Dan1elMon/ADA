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

// Definición de las matrices globales
int nT, mM, P[900][70];   // P es la matriz de tiempos de procesamiento
int EF[900][70], LS[900][70];  // Matrices de tiempos de finalización tempranos y tardíos
mt19937 Rand(time(0));  // Generador de números aleatorios

inline int elapsed(bool reset = false) {
    static clock_t start = clock();
    if (reset) {
        start = clock();
    }
    return static_cast<int>(1000.00 * double(clock() - start) / double(CLOCKS_PER_SEC));
}

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

// Función de búsqueda local
int BusquedaLocal(vector<int>& S, int pmk = 0) {
    vector<int> orden(S);
    shuffle(orden.begin(), orden.end(), Rand);  // Baraja el vector de trabajos aleatoriamente
    int k = 0;  // Índice para el orden
    int c = 0;  // Contador de iteraciones sin mejora
    int bmk = pmk;  // Última mejora
    if (bmk == 0) {
        bmk = makespan(S);  // Si no se pasa una mejora, calcula el makespan inicial
    }
    
    do {
        int mk;
        vector<int>::iterator pos;
        
        // Elimina el trabajo en la posición k de la lista S
        S.erase(find(S.begin(), S.end(), orden[k]));  
        
        // Encuentra la mejor posición de inserción para el trabajo orden[k]
        tie(mk, pos) = MejorPosicionInsercion(S, orden[k]);  
        
        // Inserta el trabajo en la mejor posición encontrada
        S.insert(pos, orden[k]);
        
        if (mk < bmk) {
            bmk = mk;  // Actualiza el mejor makespan
            c = 0;  // Reinicia el contador de iteraciones sin mejora
        }
        
        k++;  // Incrementa el índice de trabajo
        if (k >= nT) {
            k = 0;  // Si se llega al final, reinicia el índice
        }
        
        c++;  // Incrementa el contador de iteraciones
    } while (c < nT);  // Termina si no hay mejora en nT iteraciones

    return bmk;  // Devuelve el mejor makespan encontrado
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

// Función NEH 
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
    double T=double (sum_p)/(nT*mM*25);  // Ajusté la temperatura
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
        else if(double(Rand())/Rand.max()<=exp(-(double(nmk-mk )/T))){ // Corrección del cálculo de probabilidad
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
int main(){
    vector<int>SS;
    cargar("flowshop/ta023");
    cout<< nT <<" x "<< mM <<endl;

    /*
    cout <<"ilsb" <<ILS_B(SS)<<endl;
    for(auto &j:SS)cout <<j<<", ";
    cout <<endl;//*/
    
    cout <<"il_sa " << ILS_SA(SS)<<endl;
    for(auto &j:SS)cout <<j<<", ";
    cout <<endl;

    cout<<"ig_con ls "<< IG_conLS(SS)<<endl;
    for(auto &j:SS) cout <<j <<", ";
    cout <<endl;

    cout <<"ig n ls " << IG_noLs(SS) <<endl;
    for(auto &j: SS)cout<<j <<", ";
    cout << endl;
    
    //compeltadoo asdasd
}