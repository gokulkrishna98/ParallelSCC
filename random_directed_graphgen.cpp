#include <iostream>
#include <vector>
#include <random>

using namespace std;

// Function to generate a random directed graph
void generateRandomDirectedGraph(int numNodes, int numEdges) {
    cout << "#Nodes: " << numNodes << " Edges: " << numEdges << endl;
    
    // Initialize random number generator
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(0, numNodes - 1);

    // Vector to store whether an edge between two nodes exists
    vector<vector<bool>> edges(numNodes, vector<bool>(numNodes, false));

    // Generate random edges
    for (int i = 0; i < numEdges; ++i) {
        int src = dis(gen);
        int dest = dis(gen);
        // Avoid self-loops
        while (dest == src) {
            dest = dis(gen);
        }
        // Avoid duplicate edges
        while (edges[src][dest]) {
            src = dis(gen);
            dest = dis(gen);
            while (dest == src) {
                dest = dis(gen);
            }
        }
        edges[src][dest] = true;
        cout << src << " " << dest << endl;
    }
}

int main() {
    int numNodes = 100000; // Change this to adjust the number of nodes
    int numEdges = 50000; // Change this to adjust the number of edges
    generateRandomDirectedGraph(numNodes, numEdges);
    return 0;
}