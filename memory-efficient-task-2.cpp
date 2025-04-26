#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <cmath>
#include <algorithm>
using namespace std;

struct PBTNode {
    int key;
    bool empty;
    PBTNode* left;
    PBTNode* right;
    
    PBTNode(int _key = INT_MAX, bool _empty = true) 
        : key(_key), empty(_empty), left(nullptr), right(nullptr) {}
};

class PerfectBinaryHeap {
private:
    vector<PBTNode*> trees;  // Eager union: array of tree pointers
    int totalNodes;
    int potential;          // For potential method analysis
    int credits;            // For accounting method analysis
    
    // Build a perfect tree of height k
    PBTNode* buildTree(int k, int val = INT_MAX) {
        if (k == 0) return nullptr;
        PBTNode* node = new PBTNode(val, val == INT_MAX);
        node->left = buildTree(k-1, val);
        node->right = buildTree(k-1, val);
        return node;
    }
    
    // Pull-up implementation for extractMin
    void pullUp(PBTNode* node) {
        while (true) {
            PBTNode* smaller = nullptr;
            
            if (node->left && !node->left->empty) {
                smaller = node->left;
            }
            
            if (node->right && !node->right->empty) {
                if (!smaller || node->right->key < smaller->key) {
                    smaller = node->right;
                }
            }
            
            if (!smaller) break;
            
            node->key = smaller->key;
            node->empty = false;
            node = smaller;
        }
        
        node->empty = true;
    }
    
    // Count empty nodes in a tree
    int countEmptyNodes(PBTNode* node) {
        if (!node) return 0;
        return (node->empty ? 1 : 0) + countEmptyNodes(node->left) + countEmptyNodes(node->right);
    }
    
    // Collect non-empty values from a tree
    void collectNonEmptyValues(PBTNode* node, vector<int>& values) {
        if (!node) return;
        if (!node->empty) {
            values.push_back(node->key);
        }
        collectNonEmptyValues(node->left, values);
        collectNonEmptyValues(node->right, values);
    }
    
    // Delete a tree
    void deleteTree(PBTNode* node) {
        if (!node) return;
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }
    
    // Rebuild a tree when too many empty nodes
    void rebuildTree(int index) {
        vector<int> values;
        collectNonEmptyValues(trees[index], values);
        deleteTree(trees[index]);
        
        if (values.empty()) {
            trees.erase(trees.begin() + index);
            return;
        }
        
        int newHeight = ceil(log2(values.size() + 1));
        int treeSize = 1 << newHeight;
        totalNodes -= (1 << (int)log2(treeSize)) - values.size();
        
        int valIndex = 0;
        trees[index] = buildTree(newHeight);
        fillTree(trees[index], values, valIndex);
    }
    
    // Fill tree with values
    void fillTree(PBTNode* node, const vector<int>& values, int& index) {
        if (!node || index >= values.size()) return;
        node->key = values[index++];
        node->empty = false;
        fillTree(node->left, values, index);
        fillTree(node->right, values, index);
    }
    
    // Update potential function
    void updatePotential() {
        potential = 0;
        for (auto tree : trees) {
            potential += countEmptyNodes(tree);
        }
    }
    
public:
    PerfectBinaryHeap() : totalNodes(0), potential(0), credits(0) {}
    
    ~PerfectBinaryHeap() {
        for (auto tree : trees) {
            deleteTree(tree);
        }
    }
    
    // Make-Heap operation - O(1)
    void makeHeap() {
        // Nothing to do, constructor already initialized everything
        recordOperation(0, 1, 1); // Actual O(1), amortized O(1)
    }
    
    // Insert operation - O(1) amortized
    void insert(int key) {
        int oldPotential = potential;
        
        // Create new tree of height 0 with this key
        PBTNode* newTree = new PBTNode(key, false);
        trees.push_back(newTree);
        totalNodes++;
        
        // Update potential and credits
        updatePotential();
        int actualCost = 1;
        int amortizedCost = actualCost + (potential - oldPotential);
        credits += amortizedCost - actualCost;
        
        recordOperation(actualCost, amortizedCost, 2); // Charge 2, 1 for actual cost
    }
    
    // Union operation - O(1) amortized
    void unionHeap(PerfectBinaryHeap& other) {
        int oldPotential = potential;
        
        // Merge the tree lists - O(1) operation with eager union
        trees.insert(trees.end(), other.trees.begin(), other.trees.end());
        totalNodes += other.totalNodes;
        
        // Clear the other heap without deleting trees
        other.trees.clear();
        other.totalNodes = 0;
        
        // Update potential and credits
        updatePotential();
        int actualCost = 1;
        int amortizedCost = actualCost + (potential - oldPotential);
        credits += amortizedCost - actualCost;
        
        recordOperation(actualCost, amortizedCost, 1);
    }
    
    // Find minimum - O(log n) amortized (due to eager union)
    int findMin() {
        int minVal = INT_MAX;
        for (auto tree : trees) {
            if (tree && !tree->empty && tree->key < minVal) {
                minVal = tree->key;
            }
        }
        
        recordOperation(log2(totalNodes + 1), log2(totalNodes + 1), log2(totalNodes + 1));
        return minVal;
    }
    
    // Extract minimum - O(log n) amortized
    int extractMin() {
        if (trees.empty()) return INT_MAX;
        
        int oldPotential = potential;
        
        // Find tree with minimum root - O(log n) due to eager union
        int minIndex = -1;
        int minVal = INT_MAX;
        for (int i = 0; i < trees.size(); i++) {
            if (trees[i] && !trees[i]->empty && trees[i]->key < minVal) {
                minVal = trees[i]->key;
                minIndex = i;
            }
        }
        
        if (minIndex == -1) return INT_MAX;
        
        // Perform pull-up operation
        pullUp(trees[minIndex]);
        totalNodes--;
        
        // Check if we need to rebuild this tree
        int emptyCount = countEmptyNodes(trees[minIndex]);
        int threshold = (1 << ((int)log2(totalNodes + 1) - 1));
        
        if (emptyCount >= threshold) {
            rebuildTree(minIndex);
        }
        
        // Update potential and credits
        updatePotential();
        int actualCost = log2(totalNodes + 1); // Dominated by pull-up and find tree
        int amortizedCost = actualCost + (potential - oldPotential);
        credits += amortizedCost - actualCost;
        
        recordOperation(actualCost, amortizedCost, 2 * log2(totalNodes + 1));
        
        return minVal;
    }
    
    // Record operation for analysis
    void recordOperation(int actual, int amortizedPotential, int amortizedAccounting) {
        cout << "Operation Analysis:\n";
        cout << "  Actual Cost: " << actual << "\n";
        cout << "  Amortized Cost (Potential Method): " << amortizedPotential << "\n";
        cout << "  Amortized Cost (Accounting Method): " << amortizedAccounting << "\n";
        cout << "  Current Potential: " << potential << "\n";
        cout << "  Current Credits: " << credits << "\n\n";
    }
    
    // Print heap for visualization
    void printHeap() {
        cout << "Perfect Binary Heap Contents (" << trees.size() << " trees):\n";
        for (int i = 0; i < trees.size(); i++) {
            cout << "Tree " << i << " (Height " << (int)log2((trees[i] ? 1 : 0) + 1) << "): ";
            printTree(trees[i]);
            cout << endl;
        }
    }
    
    void printTree(PBTNode* node) {
        if (!node) {
            cout << "X";
            return;
        }
        if (node->empty) {
            cout << "X";
        } else {
            cout << node->key;
        }
        cout << "(";
        printTree(node->left);
        cout << ",";
        printTree(node->right);
        cout << ")";
    }
};

int main() {
    PerfectBinaryHeap heap;
    heap.makeHeap();
    
    // Insert elements to create the T4 example
    vector<int> elements = {5, 7, 10, 8, 18, 14, 12, 15, 21, 19, 17, 21, 25, 16, 18, 29};
    for (int elem : elements) {
        heap.insert(elem);
    }
    
    cout << "Initial heap (T4 example):\n";
    heap.printHeap();
    
    // Extract minimum (5) as shown in screenshot
    cout << "\nExtracting minimum...\n";
    int minVal = heap.extractMin();
    cout << "Extracted: " << minVal << "\n";
    
    cout << "\nHeap after extraction:\n";
    heap.printHeap();
    
    // Demonstrate union operation
    PerfectBinaryHeap heap2;
    heap2.insert(3);
    heap2.insert(9);
    
    cout << "\nSecond heap before union:\n";
    heap2.printHeap();
    
    heap.unionHeap(heap2);
    cout << "\nMerged heap after union:\n";
    heap.printHeap();
    
    return 0;
}
