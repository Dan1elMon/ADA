#include <bits/stdc++.h>
using namespace std;

int nT, mM, P[900][70];
int EF[900][70], LS[900][70];
mt19937 Rand( time (0));
void cargar(string nombre){
    ifstream archivo (nombre);
    archivo >>nT>>mM;
    int basura;
    for( int j=0; j<nT;j++){
        for(int i=0; i<mM; i++){
            archivo >>basura>>P[j][i];
        }
    }
}

int makespan(vector <int> & S){
    fill(& EF[0][0], &EF[0][mM],0);
    int k=1;
    for(auto j:S){
        EF[k][0] = EF[k-1][0]+P[j][0];
        for(int i = 1; i<mM; i++){
            EF[k][i] = max(EF[k-1][i], EF[k][i-1])+P[j][i];
        }
        k++;
    }
    return EF[S.size()][mM-1];
}


//genera una permutacion de trabajos
//con el orden no creciente de sus tiempos totales
void PrioridadNEH(vector<int> &Orden){
    vector<int>TT;
    TT.size(nT);
    //calcula el tiempo total para cada trabajo
    for(int j=0; j<nT; j++){
        int t=0;
        for(int i=0; i<mM; i++){
            t=t+P[j][i];
            TT[j]=t;
        }
    }
    Orden.resize(nT);
    iota(Orden.begin(), Orden.end(),0);
    sort(Orden.begin(), Orden.end(),[&TT](const int &i, const &j){
        return (TT[i]>TT[j] or (TT[i]==TT[j]and i<j));
        
    });
}

int main(){
    vector <int> ss;
    cargar("flowshop/br66");

    cout<<"Ejemplo de prioridad NEH" <<endl;
    PrioridadNEH(ss);
    for(auto &j : ss){
        cout <<j <<",";
    }
    cout<<endl;
}