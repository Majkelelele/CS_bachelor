#include <iostream>
#include <vector>
using namespace std;
#include <bits/stdc++.h>
#include <algorithm>


void process(vector<int> &tree, vector<int> &colors, int l, int r, int currentL, int currentR,
int colorToChange, int colorToPush, int i) {
    if(currentL <= currentR) {
        if(colorToPush == 2) colorToPush = colors[i];
        if(currentL >= l && currentR <= r) {
            colors[i] = colorToChange;
            if(colorToChange == 0) tree[i] = currentR - currentL + 1;
            else tree[i] = 0;
        }
        else if((currentL + currentR)/2 >= l && (currentL + currentR)/2 + 1 <= r){
            process(tree,colors,l,r,currentL,(currentL + currentR)/2,colorToChange,colorToPush,2*i);
            process(tree,colors,l,r,(currentL + currentR)/2 + 1, currentR,colorToChange,colorToPush,2*i+1);
            tree[i] = tree[2*i] + tree[2*i+1];
        }
        else if((currentL + currentR)/2 >= l) {
            if(colorToPush != 2) {
                colors[2*i+1] = colorToPush;
                if(colorToPush == 0) tree[2*i+1] = currentR - (currentL + currentR)/2;
                else tree[2*i+1] = 0;
            }
            process(tree,colors,l,r,currentL,(currentL + currentR)/2,colorToChange,colorToPush,2*i);
            tree[i] = tree[2*i] + tree[2*i+1];

        }
        else if((currentL + currentR)/2 + 1 <= r){
            if(colorToPush != 2) {
                colors[2*i] = colorToPush;
                if(colorToPush == 0) tree[2*i] = (currentL + currentR)/2 - currentL + 1;
                else tree[2*i] = 0;
            }
            process(tree,colors,l,r,(currentL + currentR)/2 + 1, currentR,colorToChange,colorToPush,2*i+1);
            tree[i] = tree[2*i] + tree[2*i+1];
        }
        if(!(currentL >= l && currentR <= r)) {
            if(colors[2*i] == colors[2*i + 1]) {
                colors[i] = colors[2*i];
            }
            else colors[i] = 2;
        }
    }

}

int main() {
    int roadLength = 0;
    int daysCount = 0;
    cin >> roadLength >> daysCount;
    int M = 3000000;
    vector<int> tree(M,0);
    vector<int> colors(M, 1);
    
   
    for(int day = 0; day < daysCount; day++) {
        char COLOR;
        int beg, end = 0;
        cin >> beg >> end >> COLOR;
        int color = COLOR == 'C';
        process(tree,colors,beg,end,1,roadLength,color,2,1);
        cout << tree[1] << endl;   
    }


    return 0;
}