#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>

using namespace std;

class Node {
   public:
    int value;
    vector<Node*> neigh;
};

Node* newNode(int value) {
    Node *node = new Node();
    node->value = value;
    return node;
}

void obliczZaleznosci(int *saver, Node *currentNode) {
    if(currentNode != NULL) {
        for(int i = 0; i < currentNode->neigh.size(); i++) {
            obliczZaleznosci(&currentNode->value,currentNode->neigh[i]);
        }
        currentNode->neigh.clear();
        if(currentNode->value > *saver) *saver = currentNode->value;
    }   
}

bool compareByLength(Node *a, Node *b)
{
    return a->value < b->value;
}

int main() {

    int liczbaZaleznosci = 0;
    int liczbaProjektów = 0;
    int liczbaMinProjektów = 0;
    cin >> liczbaProjektów >> liczbaZaleznosci >> liczbaMinProjektów;
    vector<Node*> projekty;
    int value = 0;
    for(int i = 0; i < liczbaProjektów; i++) {
        cin >> value;
        projekty.push_back(newNode(value));
    }
    int src, tgt = 0;
    for(int i = 0; i < liczbaZaleznosci; i++) {
        cin >> src >> tgt;
        projekty[src-1]->neigh.push_back(projekty[tgt-1]);
    }
    for(int i = 0; i < liczbaProjektów; i++) {
        obliczZaleznosci(&projekty[i]->value,projekty[i]);
    }    
    sort(projekty.begin(), projekty.end(),compareByLength);
    cout << projekty[liczbaMinProjektów-1]->value << endl;
    // for(int i = 0; i < liczbaProjektów; i++) {
    //     cout << projekty[i]->value << ", " << '\n';
    // }    



    return 0;
}