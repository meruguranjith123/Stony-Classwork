#include <iostream>
#include <vector>
#include <climits>
using namespace std;

enum UnionMode { LAZY, EAGER };
enum CostAnalysis { NONE, POTENTIAL, ACCOUNTING };

struct FibonacciNode {
    int key;
    int degree;
    bool mark;
    FibonacciNode* parent;
    FibonacciNode* child;
    FibonacciNode* left;
    FibonacciNode* right;

    FibonacciNode(int _key) {
        key = _key;
        degree = 0;
        mark = false;
        parent = child = nullptr;
        left = right = this;
    }
};

class FibonacciHeap {
private:
    FibonacciNode* minNode;
    int totalNodes;
    UnionMode mode;
    CostAnalysis analysis;

    // Cost analysis tracking
    int actualCost = 0;
    int totalCredits = 0;   // Accounting
    int potential = 0;      // Potential

    int insertCount = 0;
    int extractMinCount = 0;
    int decreaseKeyCount = 0;

public:
    FibonacciHeap(UnionMode m = LAZY, CostAnalysis a = NONE) {
        minNode = nullptr;
        totalNodes = 0;
        mode = m;
        analysis = a;
    }

    void insert(int key) {
        insertCount++;
        FibonacciNode* node = new FibonacciNode(key);
        if (!minNode) {
            minNode = node;
        } else {
            insertIntoRootList(node);
            if (node->key < minNode->key) {
                minNode = node;
            }
        }

        if (analysis == ACCOUNTING) totalCredits++;
        if (analysis == POTENTIAL) potential++;

        totalNodes++;
        actualCost++;
    }

    void unionHeap(FibonacciHeap* other) {
        if (!other->minNode) return;

        if (!minNode) {
            minNode = other->minNode;
            totalNodes = other->totalNodes;
            return;
        }

        mergeRootLists(other->minNode);
        if (other->minNode->key < minNode->key) {
            minNode = other->minNode;
        }

        if (mode == EAGER) {
            consolidate();
        }

        totalNodes += other->totalNodes;
    }

    void decreaseKey(FibonacciNode* x, int newKey) {
        decreaseKeyCount++;
        if (newKey > x->key) {
            cout << "New key is greater than current key!" << endl;
            return;
        }
        x->key = newKey;
        FibonacciNode* y = x->parent;

        if (y && x->key < y->key) {
            cut(x, y);
            cascadingCut(y);
        }

        if (x->key < minNode->key) {
            minNode = x;
        }

        actualCost++;
    }

    FibonacciNode* extractMin() {
        extractMinCount++;
        FibonacciNode* z = minNode;
        if (z) {
            if (z->child) {
                FibonacciNode* child = z->child;
                do {
                    FibonacciNode* next = child->right;
                    insertIntoRootList(child);
                    child->parent = nullptr;
                    child = next;
                } while (child != z->child);
            }

            removeFromRootList(z);

            if (z == z->right) {
                minNode = nullptr;
            } else {
                minNode = z->right;
                consolidate();
            }

            if (analysis == ACCOUNTING) totalCredits--;
            if (analysis == POTENTIAL) potential--;

            totalNodes--;
        }

        actualCost++;
        return z;
    }

    void printSummary(string heapName) {
        cout << "\nSummary for " << heapName << endl;
        cout << "Total Inserts: " << insertCount << endl;
        cout << "Total Extract-Mins: " << extractMinCount << endl;
        cout << "Total Decrease-Keys: " << decreaseKeyCount << endl;
        cout << "Actual Total Cost: " << actualCost << endl;

        if (analysis == POTENTIAL) {
            cout << "Final Potential: " << potential << endl;
            cout << "Amortized Cost (Potential Method): " << actualCost + potential << endl;
        } else if (analysis == ACCOUNTING) {
            cout << "Final Credits: " << totalCredits << endl;
            cout << "Amortized Cost (Accounting Method): " << actualCost + totalCredits << endl;
        }

        cout << "-----------------------------------------" << endl;
    }

    FibonacciNode* getMin() {
        return minNode;
    }

private:
    void insertIntoRootList(FibonacciNode* node) {
        node->left = minNode;
        node->right = minNode->right;
        minNode->right->left = node;
        minNode->right = node;
    }

    void removeFromRootList(FibonacciNode* node) {
        node->left->right = node->right;
        node->right->left = node->left;
    }

    void mergeRootLists(FibonacciNode* otherMin) {
        FibonacciNode* thisNext = minNode->right;
        FibonacciNode* otherPrev = otherMin->left;

        minNode->right = otherMin;
        otherMin->left = minNode;
        thisNext->left = otherPrev;
        otherPrev->right = thisNext;
    }

    void consolidate() {
        if (!minNode) return;

        int maxDegree = 45;  // log2(max n), can adjust
        vector<FibonacciNode*> A(maxDegree, nullptr);

        vector<FibonacciNode*> roots;
        FibonacciNode* curr = minNode;
        do {
            roots.push_back(curr);
            curr = curr->right;
        } while (curr != minNode);

        for (FibonacciNode* w : roots) {
            FibonacciNode* x = w;
            int d = x->degree;

            while (A[d]) {
                FibonacciNode* y = A[d];
                if (x->key > y->key) swap(x, y);

                link(y, x);
                A[d] = nullptr;
                d++;
            }
            A[d] = x;
        }

        minNode = nullptr;
        for (FibonacciNode* node : A) {
            if (node) {
                if (!minNode) {
                    node->left = node->right = node;
                    minNode = node;
                } else {
                    insertIntoRootList(node);
                    if (node->key < minNode->key) {
                        minNode = node;
                    }
                }
            }
        }
    }

    void link(FibonacciNode* y, FibonacciNode* x) {
        removeFromRootList(y);
        if (!x->child) {
            x->child = y;
            y->left = y->right = y;
        } else {
            FibonacciNode* child = x->child;
            y->left = child;
            y->right = child->right;
            child->right->left = y;
            child->right = y;
        }
        y->parent = x;
        x->degree++;
        y->mark = false;
    }

    void cut(FibonacciNode* x, FibonacciNode* y) {
        if (x->right == x) {
            y->child = nullptr;
        } else {
            if (y->child == x) y->child = x->right;
            x->left->right = x->right;
            x->right->left = x->left;
        }
        y->degree--;

        insertIntoRootList(x);
        x->parent = nullptr;
        x->mark = false;

        if (analysis == ACCOUNTING) totalCredits++;
        if (analysis == POTENTIAL) potential++;
        actualCost++;
    }

    void cascadingCut(FibonacciNode* y) {
        FibonacciNode* z = y->parent;
        if (z) {
            if (!y->mark) {
                y->mark = true;
                if (analysis == ACCOUNTING) totalCredits++;
                if (analysis == POTENTIAL) potential += 2;
                actualCost++;
            } else {
                cut(y, z);
                cascadingCut(z);
            }
        }
    }
};


void accounting()
{
    
    cout << "Testing Lazy Union + Accouting Analysis" << endl;
    FibonacciHeap lazyHeap(LAZY, ACCOUNTING);

    lazyHeap.insert(10);
    lazyHeap.insert(20);
    lazyHeap.insert(5);
    lazyHeap.insert(1);
    lazyHeap.extractMin();
    lazyHeap.insert(3);

    lazyHeap.printSummary("Lazy Heap (Accouting)");

    cout << "\nTesting Eager Union + Accouting Analysis" << endl;
    FibonacciHeap eagerHeap(EAGER, ACCOUNTING);

    eagerHeap.insert(10);
    eagerHeap.insert(20);
    eagerHeap.insert(5);
    eagerHeap.insert(1);
    eagerHeap.extractMin();
    eagerHeap.insert(3);

    eagerHeap.printSummary("Eager Heap (Potential)");

}


void potential()
{
     cout << "Testing Lazy Union + Potential Analysis" << endl;
    FibonacciHeap lazyHeap(LAZY, POTENTIAL);

    lazyHeap.insert(10);
    lazyHeap.insert(20);
    lazyHeap.insert(5);
    lazyHeap.insert(1);
    lazyHeap.extractMin();
    lazyHeap.insert(3);

    lazyHeap.printSummary("Lazy Heap (Potential)");

    cout << "\nTesting Eager Union + Potential Analysis" << endl;
    FibonacciHeap eagerHeap(EAGER, POTENTIAL);

    eagerHeap.insert(10);
    eagerHeap.insert(20);
    eagerHeap.insert(5);
    eagerHeap.insert(1);
    eagerHeap.extractMin();
    eagerHeap.insert(3);

    eagerHeap.printSummary("Eager Heap (Potential)");
}
int main() {
   

    // accounting();
    potential();





    return 0;
}


/*

Testing Lazy Union + Potential Analysis

Summary for Lazy Heap (Potential)
Total Inserts: 5
Total Extract-Mins: 1
Total Decrease-Keys: 0
Actual Total Cost: 6
Final Potential: 4
Amortized Cost (Potential Method): 10
-----------------------------------------

Testing Eager Union + Potential Analysis

Summary for Eager Heap (Potential)
Total Inserts: 5
Total Extract-Mins: 1
Total Decrease-Keys: 0
Actual Total Cost: 6
Final Potential: 4
Amortized Cost (Potential Method): 10
-----------------------------------------



Testing Lazy Union + Accouting Analysis

Summary for Lazy Heap (Accouting)
Total Inserts: 5
Total Extract-Mins: 1
Total Decrease-Keys: 0
Actual Total Cost: 6
Final Credits: 4
Amortized Cost (Accounting Method): 10
-----------------------------------------

Testing Eager Union + Accouting Analysis

Summary for Eager Heap (Potential)
Total Inserts: 5
Total Extract-Mins: 1
Total Decrease-Keys: 0
Actual Total Cost: 6
Final Credits: 4
Amortized Cost (Accounting Method): 10
-----------------------------------------

*/
