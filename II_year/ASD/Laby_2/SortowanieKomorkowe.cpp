#include <iostream>
#include <vector>
using namespace std;
#include <bits/stdc++.h>
int potega = int(pow(10,9));

int iloscMozliwosciL(int pocz, int kon, std::vector<int> &dane, std::vector<std::vector<int>> &Ltab, vector<vector<int>> &Ptab);
int iloscMozliwosciP(int pocz, int kon, std::vector<int> &dane, std::vector<std::vector<int>> &tab,vector<vector<int>> &Ptab);


int iloscMozliwosciL(int pocz, int kon, vector<int> &dane, vector<vector<int>> &Ltab, vector<vector<int>> &Ptab) {
    if(Ltab[pocz][kon] >= 0) return Ltab[pocz][kon];
    else {
        Ltab[pocz][kon] = (iloscMozliwosciL(pocz+1,kon,dane,Ltab,Ptab)*(dane[pocz] < dane[pocz+1]) +
        iloscMozliwosciP(pocz+1,kon,dane,Ltab,Ptab)*(dane[pocz] < dane[kon]))%potega;
        if(pocz + 1 == kon) Ltab[pocz][kon] /= 2;
        return Ltab[pocz][kon];
    }
      
}

int iloscMozliwosciP(int pocz, int kon, vector<int> &dane, vector<vector<int>> &Ltab, vector<vector<int>> &Ptab) {
    if(Ptab[pocz][kon] >= 0) return Ptab[pocz][kon];
    else {
        Ptab[pocz][kon] = (iloscMozliwosciL(pocz,kon-1,dane,Ltab,Ptab)*(dane[pocz] < dane[kon]) +
        iloscMozliwosciP(pocz,kon-1,dane,Ltab,Ptab)*(dane[kon-1] < dane[kon]))%potega;
        if(pocz + 1 == kon) Ptab[pocz][kon] /= 2;
        return Ptab[pocz][kon];
    }
}

int main() {

    string line;
    getline(cin,line); 
    int size = stoi(line);

    getline(cin,line);

    vector <int> tokens;
     
    stringstream check1(line);
     
    string intermediate;
    
    while(getline(check1, intermediate, ' '))
    {
        tokens.push_back(stoi(intermediate));
    }


    if(size == 0) cout << 0 << endl;
    else if(size == 1) cout << 1 << endl;
    else {

        vector<vector<int>> vecL(size , vector<int> (size, -1));
        vector<vector<int>> vecP(size , vector<int> (size, -1));

        for(int i = 0; i < size; i++) vecL[i][i] = 1;  
        for(int i = 0; i < size; i++) vecP[i][i] = 1; 
         

        iloscMozliwosciL(0,size-1,tokens,vecL,vecP);
        iloscMozliwosciP(0,size-1,tokens,vecL,vecP);
        cout << (vecL[0][size-1] + vecP[0][size-1])%potega << endl;
    } 
}