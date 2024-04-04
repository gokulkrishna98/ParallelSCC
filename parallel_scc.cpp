#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include <bits/stdc++.h>

#define UNVISITED -1

char *file_path = nullptr;
int thread_count = 1;

int num_nodes = 0;
int num_edges = 0;
std::vector<std::vector<int>> graph;
std::vector<std::vector<int>> sccs;

int scc_count = 0;
int* ids;
int* low;
bool* onStack;
std::stack<int> stack;
int id = 0;

void initValues(){
    ids = (int*) calloc(num_nodes, sizeof(int));
    low = (int*) calloc(num_nodes, sizeof(int));
    onStack = (bool*) calloc(num_nodes, sizeof(bool));

    for(int i=0; i<num_nodes; i++){
        ids[i] = UNVISITED;
    }
}

class TrajanSeq {
public:
    void dfs(int x);
    void findScc();
};

void TrajanSeq::dfs(int x){
    stack.push(x);
    onStack[x] = true;
    ids[x] = low[x] = id++;

    for(int i=0; i<graph[x].size(); i++){
        int to = graph[x][i];
        if(ids[to] == UNVISITED){
            dfs(to);
        }
        if(onStack[to]){
            low[x] = std::min(low[x], low[to]);
        }
    }

    if(ids[x] == low[x]){
        std::vector<int> scc;
        while(stack.top() != x){
            int node = stack.top(); stack.pop();
            onStack[node] = false;
            scc.push_back(node);
        }
        int node = stack.top(); stack.pop();
        scc.push_back(node);
        onStack[node] = false;

        sccs.push_back(scc);
        scc_count++;
    }
    return;
}

void TrajanSeq::findScc(){
    for(int i=0; i<num_nodes; i++){
        if(ids[i] == UNVISITED){
            dfs(i);
        }
    }
}

void readGraph(){
    if(file_path == nullptr){
        printf("File path is NULL\n");
        exit(EXIT_FAILURE);
    }

    FILE *fptr = fopen(file_path, "r");
    if(fptr == nullptr){
        fprintf(stderr, "error in reading graph file\n");
        exit(EXIT_FAILURE);
    }

    if(fscanf(fptr, "#Nodes: %d Edges: %d", &num_nodes, &num_edges) <= 0){
		printf("Could not read the number of nodes \n");	
		exit(0);
	}

    graph = std::vector<std::vector<int>>(num_nodes);
    int u, v;
    while(fscanf(fptr, "%d %d", &u, &v) > 0 ){
		graph[u].push_back(v);
	}
}

void printGraph(){
    for(int i=0; i<num_nodes; i++){
        printf("node %d: ", i);
        for(int j=0; j<graph[i].size(); j++){
            printf("%d, ", graph[i][j]);
        }
        printf("\n");
    }
}

void printScc(){
    for(int i=0; i<sccs.size(); i++){
        printf("scc %d: ", i);
        for(int j=0; j<sccs[i].size(); j++){
            printf("%d ", sccs[i][j]);
        }
        printf("\n");
    }
}


int main(int argc, char** argv){
    int opt;
    while((opt = getopt(argc, argv, "f:t:")) != - 1){
        switch (opt){
            case 'f': {
                file_path = optarg;
                break;
            }

            case 't': {
                sscanf(optarg, "%d", &thread_count);
                break;
            }

            case '?' : {
                if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n",
                                optopt);
                else
                    fprintf (stderr, "Unknown option character"
                            " `\\x%x'.\n",
                            optopt);
                return 1;
            }
            default:
                abort();
        }
    }
    
    printf("data read:\n file path: %s\n thread_count: %d\n", 
            file_path, 
            thread_count);

    readGraph();
    // printGraph();
    initValues();

    //TODO: get argument for the method, and choose the class
    TrajanSeq method;

    method.findScc();
    printScc();

    printf("scc count: %d\n", scc_count);

    return 0;
}