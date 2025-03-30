#include <iostream>
#include <vector>
using namespace std;
#include <bits/stdc++.h>
#include <algorithm>


int main() {

    string line;
    getline(cin,line);
    vector <int> tokens;
    stringstream check1(line);
    string intermediate;
    while(getline(check1, intermediate, ' '))
    {
        tokens.push_back(stoi(intermediate));
    }
    int numberOfPlayers = tokens[0];
    int numberOfMatches = tokens[1];

    vector<vector<int>> vec(numberOfMatches , vector<int> (numberOfPlayers, 0));

    for(int i = 0; i < numberOfMatches; i++) {
        getline(cin,line);     
        stringstream check(line);
        
        int j = 0;
        while(getline(check, intermediate, ' ')) {
            vec[i][j] = stoi(intermediate);
            j++;
        }
    }
    
    long long count = 1;
    long long sums[numberOfPlayers];
    fill_n(sums, numberOfPlayers, 0);   


    for(int i = 0; i < numberOfMatches; i++) {
        for(int j = numberOfPlayers/2; j < numberOfPlayers; j++) {
            sums[vec[i][j]-1] += count;
        }
        count *= 2;
    }
     
    sort(sums, sums + numberOfPlayers);

    bool check = false;
    
    for(int i = 0; i < numberOfPlayers-1 && !check; i++) {
        if(sums[i] == sums[i+1]) check = true;
    }
    if(check) cout << "NIE" << endl;
    else cout << "TAK" << endl;


    return 0;
} 