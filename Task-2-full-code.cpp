#include <iostream>
#include <queue>
using namespace std;

struct Node {
    int key;
    bool empty;
    Node* left;
    Node* right;

    Node(int _key) : key(_key), empty(false), left(nullptr), right(nullptr) {}
};

class ExtendedPerfectBinaryTree {
public:
    Node* root;
    int potential; // Potential = number of non-empty nodes
    int realCost;
    int credits; // For Accounting method

    ExtendedPerfectBinaryTree() {
        root = nullptr;
        potential = 0;
        realCost = 0;
        credits = 0;
    }

    // Manually build initial tree
    void buildInitialTree() {
        root = new Node(5);
        root->left = new Node(7);
        root->left->left = new Node(10);
        root->left->right = new Node(8);
        root->left->left->left = new Node(18);
        root->left->left->right = new Node(14);
        root->left->right->left = new Node(12);
        root->left->right->right = new Node(15);
        root->left->left->left->left = new Node(21);
        root->left->left->left->right = new Node(19);
        root->left->left->right->left = new Node(17);
        root->left->left->right->right = new Node(21);
        root->left->right->left->left = new Node(25);
        root->left->right->left->right = new Node(16);
        root->left->right->right->left = new Node(18);
        root->left->right->right->right = new Node(29);
        potential = 15;
    }

    // Helper for level order insertion (BFS)
    Node* findInsertSpot() {
        if (!root) return nullptr;
        queue<Node*> q;
        q.push(root);
        while (!q.empty()) {
            Node* n = q.front();
            q.pop();
            if (n->left == nullptr) return n;
            else q.push(n->left);
            if (n->right == nullptr) return n;
            else q.push(n->right);
        }
        return nullptr;
    }

    void insert(int key) {
        realCost = 0; // reset real cost
        int oldPotential = potential;

        Node* newNode = new Node(key);
        if (!root) {
            root = newNode;
        } else {
            Node* parent = findInsertSpot();
            if (!parent->left) parent->left = newNode;
            else if (!parent->right) parent->right = newNode;
        }
        potential++;
        realCost += 1; // count insertion as 1 real work

        int deltaPotential = potential - oldPotential;
        int amortizedPotential = realCost + deltaPotential;
        int amortizedAccounting = realCost - credits; // using credits if available

        credits += amortizedPotential - realCost; // add surplus into credits

        cout << "\n--- Inserted " << key << " ---\n";
        printTree();
        printAmortizedSummary(realCost, amortizedPotential, amortizedAccounting);
    }

    void extractMin() {
        realCost = 0;
        if (!root || root->empty) return;
        int oldPotential = potential;

        Node* x = root;

        while (true) {
            if (x->left && x->right) {
                Node* smallerChild = nullptr;
                if (x->left->empty && x->right->empty) break;
                else if (x->left->empty) smallerChild = x->right;
                else if (x->right->empty) smallerChild = x->left;
                else smallerChild = (x->left->key <= x->right->key) ? x->left : x->right;

                x->key = smallerChild->key;
                x = smallerChild;
                realCost++; // moving key up = 1 unit of real work
            } else if (x->left) {
                if (x->left->empty) break;
                x->key = x->left->key;
                x = x->left;
                realCost++;
            } else if (x->right) {
                if (x->right->empty) break;
                x->key = x->right->key;
                x = x->right;
                realCost++;
            } else {
                break;
            }
        }
        x->empty = true;
        potential--;

        int deltaPotential = potential - oldPotential;
        int amortizedPotential = realCost + deltaPotential;
        int amortizedAccounting = realCost - credits;

        credits += amortizedPotential - realCost;

        cout << "\n--- After Extract-Min ---\n";
        printTree();
        printAmortizedSummary(realCost, amortizedPotential, amortizedAccounting);
    }

    void printTree() {
        if (!root) return;
        queue<Node*> q;
        q.push(root);
        cout << "\nCurrent Tree (Level Order):\n";
        while (!q.empty()) {
            int size = q.size();
            while (size--) {
                Node* n = q.front();
                q.pop();
                if (n) {
                    if (n->empty) cout << "X ";
                    else cout << n->key << " ";
                    q.push(n->left);
                    q.push(n->right);
                } else {
                    cout << "- ";
                }
            }
            cout << endl;
        }
        cout << "Current Potential (non-empty nodes): " << potential << endl;
        cout << "Credits (Accounting Method): " << credits << endl;
        cout << "--------------------------------------------------------" << endl;
    }

    void printAmortizedSummary(int realCost, int amortizedPotential, int amortizedAccounting) {
        cout << "Real Cost: " << realCost << endl;
        cout << "Amortized Cost (Potential Method): " << amortizedPotential << endl;
        cout << "Amortized Cost (Accounting Method): " << realCost + (credits) << endl;
        cout << "--------------------------------------------------------" << endl;
    }
};

int main() {
    ExtendedPerfectBinaryTree T4;
    T4.buildInitialTree();

    cout << "Initial Tree:" << endl;
    T4.printTree();

    // Insert a few elements
    T4.insert(13);
    T4.insert(11);

    // Extract-Min a few times
    T4.extractMin();
    T4.extractMin();
    T4.extractMin();
    T4.extractMin();

    // Insert again
    T4.insert(6);
    T4.insert(9);

    // Extract again
    T4.extractMin();

    return 0;
}


/*

Initial Tree:

Current Tree (Level Order):
5 
7 - 
10 8 
18 14 12 15 
21 19 17 21 25 16 18 29 
- - - - - - - - - - - - - - - - 
Current Potential (non-empty nodes): 15
Credits (Accounting Method): 0
--------------------------------------------------------

--- Inserted 13 ---

Current Tree (Level Order):
5 
7 13 
10 8 - - 
18 14 12 15 
21 19 17 21 25 16 18 29 
- - - - - - - - - - - - - - - - 
Current Potential (non-empty nodes): 16
Credits (Accounting Method): 1
--------------------------------------------------------
Real Cost: 1
Amortized Cost (Potential Method): 2
Amortized Cost (Accounting Method): 2
--------------------------------------------------------

--- Inserted 11 ---

Current Tree (Level Order):
5 
7 13 
10 8 11 - 
18 14 12 15 - - 
21 19 17 21 25 16 18 29 
- - - - - - - - - - - - - - - - 
Current Potential (non-empty nodes): 17
Credits (Accounting Method): 2
--------------------------------------------------------
Real Cost: 1
Amortized Cost (Potential Method): 2
Amortized Cost (Accounting Method): 3
--------------------------------------------------------

--- After Extract-Min ---

Current Tree (Level Order):
7 
8 13 
10 12 11 - 
18 14 16 15 - - 
21 19 17 21 25 X 18 29 
- - - - - - - - - - - - - - - - 
Current Potential (non-empty nodes): 16
Credits (Accounting Method): 1
--------------------------------------------------------
Real Cost: 4
Amortized Cost (Potential Method): 3
Amortized Cost (Accounting Method): 5
--------------------------------------------------------

--- After Extract-Min ---

Current Tree (Level Order):
8 
10 13 
14 12 11 - 
18 17 16 15 - - 
21 19 X 21 25 X 18 29 
- - - - - - - - - - - - - - - - 
Current Potential (non-empty nodes): 15
Credits (Accounting Method): 0
--------------------------------------------------------
Real Cost: 4
Amortized Cost (Potential Method): 3
Amortized Cost (Accounting Method): 4
--------------------------------------------------------

--- After Extract-Min ---

Current Tree (Level Order):
10 
12 13 
14 15 11 - 
18 17 16 18 - - 
21 19 X 21 25 X X 29 
- - - - - - - - - - - - - - - - 
Current Potential (non-empty nodes): 14
Credits (Accounting Method): -1
--------------------------------------------------------
Real Cost: 4
Amortized Cost (Potential Method): 3
Amortized Cost (Accounting Method): 3
--------------------------------------------------------

--- After Extract-Min ---

Current Tree (Level Order):
12 
14 13 
17 15 11 - 
18 21 16 18 - - 
21 19 X X 25 X X 29 
- - - - - - - - - - - - - - - - 
Current Potential (non-empty nodes): 13
Credits (Accounting Method): -2
--------------------------------------------------------
Real Cost: 4
Amortized Cost (Potential Method): 3
Amortized Cost (Accounting Method): 2
--------------------------------------------------------

--- Inserted 6 ---

Current Tree (Level Order):
12 
14 13 
17 15 11 6 
18 21 16 18 - - - - 
21 19 X X 25 X X 29 
- - - - - - - - - - - - - - - - 
Current Potential (non-empty nodes): 14
Credits (Accounting Method): -1
--------------------------------------------------------
Real Cost: 1
Amortized Cost (Potential Method): 2
Amortized Cost (Accounting Method): 0
--------------------------------------------------------

--- Inserted 9 ---

Current Tree (Level Order):
12 
14 13 
17 15 11 6 
18 21 16 18 9 - - - 
21 19 X X 25 X X 29 - - 
- - - - - - - - - - - - - - - - 
Current Potential (non-empty nodes): 15
Credits (Accounting Method): 0
--------------------------------------------------------
Real Cost: 1
Amortized Cost (Potential Method): 2
Amortized Cost (Accounting Method): 1
--------------------------------------------------------

--- After Extract-Min ---

Current Tree (Level Order):
13 
14 6 
17 15 11 X 
18 21 16 18 9 - - - 
21 19 X X 25 X X 29 - - 
- - - - - - - - - - - - - - - - 
Current Potential (non-empty nodes): 14
Credits (Accounting Method): -1
--------------------------------------------------------
Real Cost: 2
Amortized Cost (Potential Method): 1
Amortized Cost (Accounting Method): 1
--------------------------------------------------------




*/