#include <iostream>
#include <vector>
using namespace std;
#include <bits/stdc++.h>
#include <algorithm>

int power = pow(10,9);

int findMax(vector<int> &vec) {
    int max = vec[0];
    for(int i = 1; i < vec.size(); i++) {
        if(vec[i] > max) max = vec[i];
    }
    return max;
}

int findIfGivenLengthNeeded(vector<int> &vec, int len, vector<int> &indeces) {
    int count = 0;
    for(int i = 0; i < vec.size(); i++) {
        if(vec[i] == len){
            indeces[count] = i;
            count++;
        } 
    }
    return count;
}

void copyVector(vector<int> &copied, vector<int> &copiedInto){
    for(int i = 0; i < copied.size(); i++) {
        copiedInto[i] = copied[i];
    }
}

int main() {
    string line;   
    getline(cin,line);
    vector <int>  info;
    stringstream check(line);     
    string intermediate;
    while(getline(check, intermediate, ' '))
    {
        info.push_back(stoi(intermediate));
    }
    int numberOfMozaikaTocheck = info[0];
    int maxDetails = info[1];

    getline(cin,line);
    vector <int>  MozaikaLengths;
    stringstream check1(line);     
    while(getline(check1, intermediate, ' '))
    {
        MozaikaLengths.push_back(stoi(intermediate));
    }

    int maxLength = findMax(MozaikaLengths); 
    vector<int> dataPrevious(maxDetails+1,1);
    vector<int> dataCurrent(maxDetails+1,1);
    vector<int> results(MozaikaLengths.size(),-1);
    vector<int> indeces(MozaikaLengths.size(),-1);

   
    for(int i = 0; i < maxLength; i++) {
        for(int j = 0; j < maxDetails+1 && i > 0; j++) {
            if(j == 0) {
                dataCurrent[j] = (dataPrevious[j] + dataPrevious[j+1])%power;
            } 
            else if(j == maxDetails){
                dataCurrent[j] = (dataPrevious[j] + dataPrevious[j-1])%power;
            }
            else {
                dataCurrent[j] = (dataPrevious[j] + dataPrevious[j-1])%power;
                dataCurrent[j] = (dataCurrent[j] + dataPrevious[j+1])%power;
            }
        }
        int count = findIfGivenLengthNeeded(MozaikaLengths,i+1, indeces);
        if(count > 0) {
            int sum = 0;
            for(int k = 0; k < maxDetails+1; k++) {
               sum = (sum + dataCurrent[k])%power;
            }
            for(int index = 0; index < count; index++) results[indeces[index]] = sum;
        }
        copyVector(dataCurrent,dataPrevious);
    }
    for(int i = 0; i < results.size(); i++) {
        cout << results[i] << " ";
    }
    cout << endl;
    return 0;
}