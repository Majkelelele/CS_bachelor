#include <iostream>
#include <vector>
using namespace std;
#include <bits/stdc++.h>
#include <algorithm>

// AVL tree implementation in C++

#include <iostream>
using namespace std;

class Node {
   public:
  int sizeOfTree;
  int value;
  int countOfValues;
  Node *left;
  Node *right;
  int height;
};

int max(int a, int b);

// Calculate height
int height(Node *N) {
  if (N == NULL)
    return 0;
  return N->height;
}

int max(int a, int b) {
  return (a > b) ? a : b;
}

void updateCount(Node *root) {
  if(root != NULL) {
    int sum = 0;
    if(root->left != NULL) sum += root->left->sizeOfTree;
    if(root->right != NULL) sum += root->right->sizeOfTree;
    root->sizeOfTree = sum + root->countOfValues;
  }
  
  
} 

int getCount(Node *node) {
  if(node == NULL) return 0;
  else return node->sizeOfTree;
}

// New node creation
Node *newNode(int value, int count, Node *left, Node *right, int height) {
  Node *node = new Node();
  node->value = value;
  node->countOfValues = count;
  // node->sizeOfTree = 0;
  node->left = left;
  node->right = right;
  updateCount(node);
  node->height = height;
  return (node);
}



// Rotate right
Node *rightRotate(Node *y) {
  Node *x = y->left;
  Node *T2 = x->right;
  x->right = y;
  y->left = T2;
  if(T2 != NULL) {
    T2->height = max(height(T2->left),
              height(T2->right)) +
            1;
    }
  
  y->height = max(height(y->left),
          height(y->right)) +
        1;
  x->height = max(height(x->left),
          height(x->right)) +
        1;
  
  updateCount(T2);
  updateCount(y);
  updateCount(x);
  return x;
}

// Rotate left
Node *leftRotate(Node *x) {
  Node *y = x->right;
  Node *T2 = y->left;
  y->left = x;
  x->right = T2;
  if(T2 != NULL) {
    T2->height = max(height(T2->left),
              height(T2->right)) +
            1;
    }
  x->height = max(height(x->left),
          height(x->right)) +
        1;
  y->height = max(height(y->left),
          height(y->right)) +
        1;
  updateCount(T2);
  updateCount(x);
  updateCount(y);
  return y;
}

// Get the balance factor of each node
int getBalanceFactor(Node *N) {
  if (N == NULL)
    return 0;
  return height(N->left) -
       height(N->right);
}
void printTree(Node *root) {
  if(root != NULL) {
    printTree(root->left);
    for(int i = 0; i < root->countOfValues; i++) cout << root->value << ", ";

    printTree(root->right);
  }
}

// Insert a node
Node *insertNode(Node *node, int position, int value, int count) {
  // Find the correct postion and insert the node
  if (node == NULL) return newNode(value,count,NULL,NULL,1);
  if(getCount(node->left) >= position) {
    node->left = insertNode(node->left,position,value,count);
  }
  else if (getCount(node->left) + node->countOfValues > position) {
    int leftSplit = position - getCount(node->left);
    node->left = insertNode(node->left,getCount(node->left),node->value,leftSplit);
    node->right = insertNode(node->right,0,node->value,node->countOfValues - leftSplit);
    node->countOfValues = count;
    node->value = value;
    
  }
  else {
    // cout << "position: " << position << "node->countOfValues: "  << node->countOfValues << "getCount(node->left):" << getCount(node->left)
    //  <<  endl;
    // if(node->left != NULL) cout << "node->left->countOfValues: " << node->left->countOfValues << endl;
    node->right = insertNode(node->right,position - node->countOfValues - getCount(node->left),value,count);
  }
  updateCount(node);
  // Update the balance factor of each node and
  // balance the tree
  node->height = 1 + max(height(node->left),
               height(node->right));
  // node->sizeOfTree = getCount(node);
  int balanceFactor = getBalanceFactor(node);
  if (balanceFactor > 1) {
    if(height(node->left->left) > height(node->left->right)) {
      return rightRotate(node);
    }
    else {
      node->left = leftRotate(node->left);
      return rightRotate(node);
    }  
  }
  if (balanceFactor < -1) {
    // cout << "right tree too big" << endl;
    // printTree(node);
    if(height(node->right->right) > height(node->right->left)) {
      return leftRotate(node);
    }
    else {
      node->right = rightRotate(node->right);
      return leftRotate(node);
    }  
  }
  updateCount(node);
  return node;
}









int get(Node* root, int position) {
  if(root != NULL) {
    if(root->left == root->right) return root->value;
    else if(getCount(root->left) >  position) return get(root->left,position);
    else if(getCount(root->left) + root->countOfValues > position) return root->value;
    else return get(root->right, position - getCount(root->left) - root->countOfValues);
  } 
  return -5;
}


void freeTree(Node *root) {
  if(root != NULL) {
    freeTree(root->left);
    freeTree(root->right);
    free(root);
  }

}


int main(){
    ios_base::sync_with_stdio(false);
    int numberOfOperations = 0;
    cin >> numberOfOperations;
 
    
    char operation;
    Node *root = NULL;
    int result = 0;
    int currentTreeSize = 0; 
    // cout << endl;
    // cout << endl;
    for(int i = 0; i < numberOfOperations; i++) {
        cin >> operation;
        if(operation == 'g') {
            int index = 0;
            cin >> index;
            index = (index  + result)%currentTreeSize;
            result = get(root,index);
            // cout << endl << "Get returns: " << result << endl;
            cout << result << '\n';
        }
        else {
            int j, x, k = 0;
            cin >> j >> x >> k;
            j = (j + result)%(currentTreeSize + 1);
            // cout << "INSERT gets value :" << j << ", " << x << ", " << k<< endl;
            root = insertNode(root,j,x,k);
            // cout << endl;
            // cout << "root values: " << root->value << endl;
            // cout << getBalanceFactor(root) << endl;
            // printTree(root);
            currentTreeSize += k;
        }
    }

  freeTree(root);

    
}