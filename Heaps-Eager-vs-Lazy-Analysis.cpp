#include <iostream>
#include <vector>
#include <climits>
using namespace std;

enum UnionMode { LAZY, EAGER };
enum CostAnalysis { NONE, ACCOUNTING, POTENTIAL };

struct BinomialNode {
    int key;
    int degree;
    BinomialNode* parent;
    BinomialNode* child;
    BinomialNode* sibling;

    BinomialNode(int _key) : key(_key), degree(0), parent(nullptr), child(nullptr), sibling(nullptr) {}
};

class BinomialHeap {
private:
    BinomialNode* head;
    UnionMode mode;
    CostAnalysis analysis;

    // For cost tracking
    int totalCredits = 0;       // Accounting
    int potential = 0;          // Potential
    int actualCost = 0;         // Raw step count

    int insertCount = 0;
    int extractMinCount = 0;

public:
    BinomialHeap(UnionMode m = EAGER, CostAnalysis a = NONE) : head(nullptr), mode(m), analysis(a) {}

    static BinomialNode* mergeRootLists(BinomialNode* h1, BinomialNode* h2, int& mergeCostCounter) {
        if (!h1) return h2;
        if (!h2) return h1;

        BinomialNode* head = nullptr;
        BinomialNode* tail = nullptr;

        if (h1->degree <= h2->degree) {
            head = tail = h1;
            h1 = h1->sibling;
        } else {
            head = tail = h2;
            h2 = h2->sibling;
        }

        while (h1 && h2) {
            mergeCostCounter++;  // cost for comparing/merging
            if (h1->degree <= h2->degree) {
                tail->sibling = h1;
                h1 = h1->sibling;
            } else {
                tail->sibling = h2;
                h2 = h2->sibling;
            }
            tail = tail->sibling;
        }

        tail->sibling = (h1) ? h1 : h2;
        return head;
    }

    static void linkTrees(BinomialNode* y, BinomialNode* z) {
        y->parent = z;
        y->sibling = z->child;
        z->child = y;
        z->degree += 1;
    }

    void insert(int key) {
        insertCount++;

        BinomialHeap* temp = new BinomialHeap(mode, analysis);
        temp->head = new BinomialNode(key);

        if (analysis == ACCOUNTING) totalCredits += 1;  // assign 1 credit
        if (analysis == POTENTIAL) potential += 1;      // +1 tree

        if (mode == LAZY) {
            lazyUnion(temp);
        } else {
            eagerUnion(temp);
        }
        printCosts("Insert " + to_string(key));
    }

    void lazyUnion(BinomialHeap* other) {
        int mergeSteps = 0;
        head = mergeRootLists(head, other->head, mergeSteps);
        actualCost += mergeSteps;
    }

    void eagerUnion(BinomialHeap* other) {
        int mergeSteps = 0;
        head = mergeRootLists(head, other->head, mergeSteps);
        actualCost += mergeSteps;
        if (!head) return;

        BinomialNode* prev = nullptr;
        BinomialNode* curr = head;
        BinomialNode* next = curr->sibling;

        while (next) {
            actualCost++;  // each comparison
            if ((curr->degree != next->degree) ||
                (next->sibling && next->sibling->degree == curr->degree)) {
                prev = curr;
                curr = next;
            } else {
                if (curr->key <= next->key) {
                    curr->sibling = next->sibling;
                    linkTrees(next, curr);
                    if (analysis == ACCOUNTING) totalCredits -= 1;
                    if (analysis == POTENTIAL) potential -= 1;
                } else {
                    if (!prev) {
                        head = next;
                    } else {
                        prev->sibling = next;
                    }
                    linkTrees(curr, next);
                    curr = next;
                    if (analysis == ACCOUNTING) totalCredits -= 1;
                    if (analysis == POTENTIAL) potential -= 1;
                }
            }
            next = curr->sibling;
        }
    }

    int extractMin() {
        if (!head) return -1;

        extractMinCount++;

        BinomialNode* minNode = head;
        BinomialNode* minPrev = nullptr;
        BinomialNode* curr = head;
        BinomialNode* prev = nullptr;

        int min = curr->key;
        while (curr) {
            actualCost++;
            if (curr->key < min) {
                min = curr->key;
                minNode = curr;
                minPrev = prev;
            }
            prev = curr;
            curr = curr->sibling;
        }

        if (minPrev) {
            minPrev->sibling = minNode->sibling;
        } else {
            head = minNode->sibling;
        }

        BinomialNode* child = minNode->child;
        BinomialNode* reversed = nullptr;
        while (child) {
            actualCost++;
            BinomialNode* next = child->sibling;
            child->sibling = reversed;
            child->parent = nullptr;
            reversed = child;
            child = next;
        }

        BinomialHeap* temp = new BinomialHeap(mode, analysis);
        temp->head = reversed;

        if (analysis == ACCOUNTING) {
            totalCredits -= 1;  // removing min root
        }
        if (analysis == POTENTIAL) {
            potential -= 1;
        }

        if (mode == LAZY) {
            lazyUnion(temp);
        } else {
            eagerUnion(temp);
        }

        printCosts("ExtractMin (removed " + to_string(minNode->key) + ")");
        return minNode->key;
    }

    void printCosts(string operation) {
        cout << "After Operation: " << operation << endl;
        cout << "Actual Cost so far: " << actualCost << endl;
        if (analysis == ACCOUNTING) {
            cout << "Total Credits: " << totalCredits << endl;
            cout << "Amortized Cost (Accounting Method): " << (actualCost + totalCredits) << endl;
        } else if (analysis == POTENTIAL) {
            cout << "Potential: " << potential << endl;
            cout << "Amortized Cost (Potential Method): " << (actualCost + potential) << endl;
        }
        cout << "-------------------------------------" << endl;
    }

    void printSummary() {
        cout << "\n========== FINAL SUMMARY ==========\n";
        cout << "Insert Operations: " << insertCount << endl;
        cout << "Extract-Min Operations: " << extractMinCount << endl;
        cout << "Total Actual Cost: " << actualCost << endl;

        if (analysis == ACCOUNTING) {
            cout << "Final Total Credits: " << totalCredits << endl;
            cout << "Total Amortized Cost (Accounting): " << (actualCost + totalCredits) << endl;
        } else if (analysis == POTENTIAL) {
            cout << "Final Potential: " << potential << endl;
            cout << "Total Amortized Cost (Potential): " << (actualCost + potential) << endl;
        }
        cout << "====================================\n";
    }
};
int main() {
    cout << "Using Eager Union + Accounting Method\n\n";
    BinomialHeap heap(EAGER, ACCOUNTING);

    heap.insert(10);
    heap.insert(20);
    heap.insert(5);
    heap.insert(1);
    heap.insert(15);

    heap.extractMin();
    heap.extractMin();

    heap.printSummary();

    cout << "\nUsing Lazy Union + Potential Method\n\n";
    BinomialHeap heap2(LAZY, POTENTIAL);

    heap2.insert(10);
    heap2.insert(20);
    heap2.insert(5);
    heap2.insert(1);
    heap2.insert(15);

    heap2.extractMin();
    heap2.extractMin();

    heap2.printSummary();

    return 0;
}
