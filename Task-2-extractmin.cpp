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
    int potential; // Number of non-empty nodes

    ExtendedPerfectBinaryTree() {
        root = nullptr;
        potential = 0;
    }

    // Manually build your custom initial tree
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

        potential = 15; // 15 nodes initially
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
        cout << "--------------------------------------------------------" << endl;
    }

    // Extract-Min using hole-filling
    void extractMin() {
        if (!root || root->empty) return;

        Node* x = root;

        while (true) {
            if (x->left && x->right) {
                // Both children exist
                Node* smallerChild = nullptr;
                if (x->left->empty && x->right->empty) break;
                else if (x->left->empty) smallerChild = x->right;
                else if (x->right->empty) smallerChild = x->left;
                else smallerChild = (x->left->key <= x->right->key) ? x->left : x->right;

                x->key = smallerChild->key;
                x = smallerChild;
            } else if (x->left) {
                // Only left child exists
                if (x->left->empty) break;
                x->key = x->left->key;
                x = x->left;
            } else if (x->right) {
                // Only right child exists
                if (x->right->empty) break;
                x->key = x->right->key;
                x = x->right;
            } else {
                break; // Leaf node
            }
        }

        // Final hole becomes empty
        x->empty = true;
        potential--;
    }
};

int main() {
    ExtendedPerfectBinaryTree T4;
    T4.buildInitialTree();

    cout << "Initial Tree:" << endl;
    T4.printTree();

    // Extract 5
    cout << "\nAfter Extracting 5:" << endl;
    T4.extractMin();
    T4.printTree();

    // Extract 7
    cout << "\nAfter Extracting 7:" << endl;
    T4.extractMin();
    T4.printTree();

    // Extract 8
    cout << "\nAfter Extracting 8:" << endl;
    T4.extractMin();
    T4.printTree();

    // Extract 10
    cout << "\nAfter Extracting 10:" << endl;
    T4.extractMin();
    T4.printTree();

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
--------------------------------------------------------

After Extracting 5:

Current Tree (Level Order):
7 
8 - 
10 12 
18 14 16 15 
21 19 17 21 25 X 18 29 
- - - - - - - - - - - - - - - - 
Current Potential (non-empty nodes): 14
--------------------------------------------------------

After Extracting 7:

Current Tree (Level Order):
8 
10 - 
14 12 
18 17 16 15 
21 19 X 21 25 X 18 29 
- - - - - - - - - - - - - - - - 
Current Potential (non-empty nodes): 13
--------------------------------------------------------

After Extracting 8:

Current Tree (Level Order):
10 
12 - 
14 15 
18 17 16 18 
21 19 X 21 25 X X 29 
- - - - - - - - - - - - - - - - 
Current Potential (non-empty nodes): 12
--------------------------------------------------------

After Extracting 10:

Current Tree (Level Order):
12 
14 - 
17 15 
18 21 16 18 
21 19 X X 25 X X 29 
- - - - - - - - - - - - - - - - 
Current Potential (non-empty nodes): 11
--------------------------------------------------------

*/