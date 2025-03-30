#include <iostream>
#include <vector>
using namespace std;
#include <bits/stdc++.h>
#include <algorithm>

int potega = pow(10,9);
const int N = 40000;
int tree[2 * N]; 


void zmienWartosc(int p, int value,int n)  
{  
    tree[p+n] = value; 
    p = p+n; 
    for (int i=p; i > 1; i /= 2) 
        tree[i/2] = (tree[i] + tree[i^1])%potega; 
} 
  
 
// int zsumuj(int l, int r, int n)  
// {  
//     int res = 0; 
//     r--;
//     l += n;
//     r += n;
//     while (l < r ) { 
//         if (l%2 == 1)  
//             res = (res+tree[l++])%potega; 
//         if (r%2 == 0)  
//             res = (res+tree[--r])%potega;
        
//         l /=2; 
//         r /= 2; 
//     } 
//     return res; 
// } 

int zsumuj(int l, int r, int n) { // suma od l do r wlacznie
    l += n;
    r += n;
    int sum = tree[l];
    if (l != r) {
        sum += tree[r];
    }
    while (l / 2 < r / 2) {
        if (l % 2 == 0) {
            sum = (sum+tree[l + 1])%potega;
        }
        if (r % 2 == 1) {
            sum = (sum + tree[r - 1])%potega;
        }

        l /= 2;
        r /= 2;
    }
    return sum;
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
    int n = info[0];
    int k = info[1];
    getline(cin,line);
    vector <int>  permutation;
    stringstream check1(line);        
    while(getline(check1, intermediate, ' '))
    {
        permutation.push_back(stoi(intermediate));
    }


    
    vector<vector<int>> counter(k, vector<int> (n, 0));  
    
    for(int i = 0; i < n; i++) counter[0][i] = 1;
    for(int i = 1; i < k; i++) {
        for(int m = 0; m < 2*(n+1); m++) tree[m] = 0;
        for(int j = 0; j < n; j++) {
            int max = n+1;
            int value = permutation[j];
            int min = value+1;
            int sum = zsumuj(min,max-1,n);
            counter[i][j] = sum;
            zmienWartosc(value,counter[i-1][j],n);        
        }
    }
    int sum = 0;
    for(int i = 0; i < n; i++) {
        sum = (sum + counter[k-1][i])%potega;
    }
    cout << sum << endl;
    
    return 0;
}