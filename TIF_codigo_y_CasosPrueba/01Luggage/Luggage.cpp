#include <iostream>
#include <cstdio>
#include <string>
#include <sstream>
#include <algorithm>

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
    int test, el, i, sum;
    string s;
    stringstream ss;

    scanf("%d", &test);
    getchar(); // Lee el salto de línea después del número de test

    while(test--) {
        getline(cin, s);
        ss.clear();
        ss.str(s);
        i = 0;
        sum = 0;

        while(ss >> el) {
            wt[++i] = el;
            sum += el;
        }

        if((sum & 1) == 0) {
            if(divide(i, sum >> 1) == (sum >> 1))
                puts("YES");
            else
                puts("NO");
        } else {
            puts("NO");
        }
    }

    return 0;
}