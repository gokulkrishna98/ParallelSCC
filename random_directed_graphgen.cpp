#include <iostream>
#include <vector>
#include <random>
#include <unordered_set>

using namespace std;

// Function to generate a random directed graph
void generateRandomDirectedGraph(int numNodes, int numEdges) {
    cout << "#Nodes: " << numNodes << " Edges: " << numEdges << endl;
    
    // Initialize random number generator
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(0, numNodes - 1);

    // Set to store generated edges
    unordered_set<int> generatedEdges;

    // Generate random edges
    for (int i = 0; i < numEdges; ++i) {
        int src = dis(gen);
        int dest = dis(gen);
        // Avoid self-loops and duplicate edges
        while (dest == src || generatedEdges.count(src * numNodes + dest)) {
            src = dis(gen);
            dest = dis(gen);
        }
        generatedEdges.insert(src * numNodes + dest);
        cout << src << " " << dest << endl;
    }
}

int main() {
    int numNodes = 100000; // Change this to adjust the number of nodes
    int numEdges = 100000; // Change this to adjust the number of edges
    generateRandomDirectedGraph(numNodes, numEdges);
    return 0;
}