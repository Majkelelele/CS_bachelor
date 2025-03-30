#include <iostream>
#include <vector>
using namespace std;
#include <bits/stdc++.h>

int main() {
    
    string line;
    getline(cin, line);
    long long numberOfProducts = stoi(line);
    

    getline(cin,line);
    vector <long long> productsPrices;
     
  
    stringstream check1(line);     
    string intermediate;
     
   
    while(getline(check1, intermediate, ' '))
    {
        productsPrices.push_back(stoi(intermediate));
    }

    getline(cin, line);
    long long numberOfDays = stoi(line);

    vector<long long> numberOfProductsPerDay;
    long long i = 0;
    while(i < numberOfDays) {
        getline(cin, line);
        numberOfProductsPerDay.push_back(stoi(line));
        i++;
    }
        

    long long sums[numberOfProducts];
    long long sum = 0;
    for(int i = numberOfProducts - 1; i >= 0; i--) {
        sum += productsPrices[i];
        sums[i] = sum;
    }
    long long lowestOddNumRight[numberOfProducts];
    long long lowestEvenNumRight[numberOfProducts];
    long long lowestOdd = -1;
    long long lowestEven = -1;
    for(long long i = numberOfProducts - 1; i >= 0; i--) {
        if(productsPrices[i] % 2 == 0) {
            lowestEven = productsPrices[i];
        }
        else {
            lowestOdd = productsPrices[i];
        }
        lowestEvenNumRight[i] = lowestEven;
        lowestOddNumRight[i] = lowestOdd;
        
    }
    long long highestOddNumLeft[numberOfProducts];
    long long highestEvenNumLeft[numberOfProducts];
    long long highestOdd = -1;
    long long highestEven = -1;
    for(long long i = 0; i < numberOfProducts; i++) {
        highestEvenNumLeft[i] = highestEven;
        highestOddNumLeft[i] = highestOdd;
        if(productsPrices[i] % 2 == 0) {
            highestEven = productsPrices[i];
        }
        else {
            highestOdd = productsPrices[i];
        }
    }

    for(long long productsNumber : numberOfProductsPerDay) {
        long long index = numberOfProducts - productsNumber;
        if(index < 0 || index >= numberOfProducts) {
            cout << -1 << endl;
        }
        sum = sums[index];
        if(sum % 2 == 1) cout << sum << endl;
        else {
            lowestEven = lowestEvenNumRight[index];
            lowestOdd = lowestOddNumRight[index];
            highestEven = highestEvenNumLeft[index];
            highestOdd = highestOddNumLeft[index];
            if((lowestEven == -1 || highestOdd == -1) && (highestEven == -1 || lowestOdd == -1)) cout << -1 << endl;
            else {
                long long lowestDifference;
                if(lowestEven == -1 || highestOdd == -1) lowestDifference = lowestOdd-highestEven;
                else if(highestEven == -1 || lowestOdd == -1) lowestDifference = lowestEven-highestOdd;
                else lowestDifference = min(lowestEven-highestOdd, lowestOdd-highestEven);
                cout << sum - lowestDifference << endl;
            }
        }
    }
    return 0;
}





