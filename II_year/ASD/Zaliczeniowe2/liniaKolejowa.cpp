#include <bits/stdc++.h>
using namespace std;
#include <iostream>

#include <cmath>

struct node {
	int key;
	struct node* left;
	struct node* right;
	int height;
	int count;
    int size;
};


int height(struct node* N)
{
	if (N == NULL)
		return 0;
	return N->height;
}

int max(int a, int b)
{
	return (a > b) ? a : b;
}

int getSize(node *r) {
    if(r != NULL) return r->size;
    else return 0;
}

void updateSize(node *r) {
    if(r != NULL) {
        r->size = r->count + getSize(r->left) + getSize(r->right);
    }
}


struct node* newNode(int key)
{
	struct node* node = (struct node*)
		malloc(sizeof(struct node));
	node->key = key;
	node->left = NULL;
	node->right = NULL;
	node->height = 1; 
	node->count = 1;
    updateSize(node);
	return (node);
}

struct node* rightRotate(struct node* y)
{
	struct node* x = y->left;
	struct node* T2 = x->right;

	x->right = y;
	y->left = T2;

	y->height = max(height(y->left), height(y->right)) + 1;
	x->height = max(height(x->left), height(x->right)) + 1;
    updateSize(T2);
    updateSize(y);
    updateSize(x);

	return x;
}


struct node* leftRotate(struct node* x)
{
	struct node* y = x->right;
	struct node* T2 = y->left;

	y->left = x;
	x->right = T2;

	x->height = max(height(x->left), height(x->right)) + 1;
	y->height = max(height(y->left), height(y->right)) + 1;

    updateSize(T2);
    updateSize(x);
    updateSize(y);

	return y;
}

int getBalance(struct node* N)
{
	if (N == NULL)
		return 0;
	return height(N->left) - height(N->right);
}

struct node* insert(struct node* node, int key)
{
	if (node == NULL)
		return (newNode(key));

	if (key == node->key) {
		(node->count)++;
        node->size++;
		return node;
	}

	if (key < node->key)
		node->left = insert(node->left, key);
	else
		node->right = insert(node->right, key);

	node->height = max(height(node->left), height(node->right)) + 1;


	int balance = getBalance(node);


	if (balance > 1 && key < node->left->key)
		return rightRotate(node);

	if (balance < -1 && key > node->right->key)
		return leftRotate(node);

	if (balance > 1 && key > node->left->key) {
		node->left = leftRotate(node->left);
		return rightRotate(node);
	}

	if (balance < -1 && key < node->right->key) {
		node->right = rightRotate(node->right);
		return leftRotate(node);
	}
    updateSize(node);

	return node;
}


struct node* minValueNode(struct node* node)
{
	struct node* current = node;

	while (current->left != NULL)
		current = current->left;

	return current;
}

struct node* deleteNode(struct node* root, int key)
{

	if (root == NULL)
		return root;

	if (key < root->key)
		root->left = deleteNode(root->left, key);

	else if (key > root->key)
		root->right = deleteNode(root->right, key);


	else {

		if (root->count > 1) {
			(root->count)--;
            (root->size)--;
			return root;
		}

		if ((root->left == NULL) || (root->right == NULL)) {
			struct node* temp = root->left ? root->left : root->right;

			if (temp == NULL) {
				temp = root;
				root = NULL;
			}
			else 
				*root = *temp; 
			free(temp);
		}
		else {

			struct node* temp = minValueNode(root->right);

			root->key = temp->key;
			root->count = temp->count;
			temp->count = 1;

			root->right = deleteNode(root->right, temp->key);
			updateSize(root);
		}
	}

	if (root == NULL)
		return root;

	updateSize(root);

	root->height = max(height(root->left), height(root->right)) + 1;


	int balance = getBalance(root);


	if (balance > 1 && getBalance(root->left) >= 0)
		return rightRotate(root);

	if (balance > 1 && getBalance(root->left) < 0) {
		root->left = leftRotate(root->left);
		return rightRotate(root);
	}

	if (balance < -1 && getBalance(root->right) <= 0)
		return leftRotate(root);

	if (balance < -1 && getBalance(root->right) > 0) {
		root->right = rightRotate(root->right);
		return leftRotate(root);
	}
    updateSize(root);

	return root;
}


class NodeSpecial  
{  
    public: 
    int t;  
    int l;
    int p;  
}; 

NodeSpecial *newNodeSpecial(int t, int l, int p) {
    NodeSpecial *node = new NodeSpecial();
    node->t = t;
    node->l = l;
    node->p = p;
    return node;
}
bool my_cmp(NodeSpecial *a, NodeSpecial *b)
{
    return a->t < b->t;
}

int findNumberOfGreaterNodes(node *root, int value) {
    if(root != NULL) {
        if(root->key > value) return findNumberOfGreaterNodes(root->left,value) + root->count + getSize(root->right);
        else if(root->key == value) return getSize(root->right);
        else return findNumberOfGreaterNodes(root->right,value);
    }
    else return 0;
}

int findNumberOfSmallerNode(node *root, int value) {
    if(root != NULL) {
        if(root->key > value) return findNumberOfSmallerNode(root->left,value);
        else if(root->key == value) return getSize(root->left);
        else return findNumberOfSmallerNode(root->right,value) + root->count + getSize(root->left);
    }
    else return 0;
}



int main() 
{ 
    int n = 0;
    int k = 0;
    cin >> n >> k;
    long long count = 0;
    int a, b, t = 0;
    vector<NodeSpecial*> vec;
    for(int i = 0; i < n; i++) {
        cin >> a >> b >> t;
        vec.push_back(newNodeSpecial(t,a,b));
    }
    sort(vec.begin(), vec.end(), my_cmp);	

    int p = 0;
    int i = 0;
    int currentT = 0;
    node *rootBeg = NULL;
    node *rootEnd = NULL;
    while(i < n) {
        currentT = vec[i]->t;
        while(p < n && vec[p]->t - currentT <= k) {
            rootBeg = insert(rootBeg,vec[p]->l);
            rootEnd = insert(rootEnd,vec[p]->p);
            p++;
        }         
        int greaterThanP = findNumberOfGreaterNodes(rootBeg, vec[i]->p);
        int smallerThanL = findNumberOfSmallerNode(rootEnd, vec[i]->l);

        count += p - i - 1 - greaterThanP - smallerThanL;
		rootBeg = deleteNode(rootBeg,vec[i]->l);
		rootEnd = deleteNode(rootEnd,vec[i]->p);
        i++;
    }

    cout << count << '\n';  

} 