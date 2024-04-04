#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include <bits/stdc++.h>

// #include <trajan_seq.h>


#define UNVISITED -1

char *file_path = nullptr;
int thread_count = 1;
int scc_count = 0;

/***************************************************************************
**************   TRAJAN SEQUENTIAL IMPLEMENTATION   ************************
****************************************************************************/

class TrajanSeq {
    int num_nodes = 0;
    int num_edges = 0;
    std::vector<std::vector<int>> graph;
    std::vector<std::vector<int>> sccs;

    int* ids;
    int* low;
    bool* onStack;
    std::stack<int> stack;
    int id = 0;

    int scc_count = 0;
public:
    void initValues();
    void readGraph(char* filePath);
    void dfs(int x);
    void findScc();
    void freeValues();
    std::vector<std::vector<int>> getGraph();
    std::vector<std::vector<int>> getSccs();
};

std::vector<std::vector<int>> TrajanSeq::getGraph(){
    return graph;
}

std::vector<std::vector<int>> TrajanSeq::getSccs(){
    return sccs;
}

void TrajanSeq::initValues(){
    ids = (int*) calloc(num_nodes, sizeof(int));
    low = (int*) calloc(num_nodes, sizeof(int));
    onStack = (bool*) calloc(num_nodes, sizeof(bool));

    for(int i=0; i<num_nodes; i++){
        ids[i] = UNVISITED;
    }
}

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

void TrajanSeq::readGraph(char* file_path){
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

void TrajanSeq::freeValues(){
    free(ids);
    free(low);
    free(onStack);

    ids = nullptr;
    low = nullptr;
    onStack = nullptr;
}

/***************************************************************************
*****************    FORWARD-BACKWARD ALGORITHM    *************************
****************************************************************************/

class FB {
    int num_nodes = 0;
    int num_edges = 0;
    int scc_count = 0;

    std::vector<std::vector<int>> graph;
    std::vector<std::vector<int>> forward;
    std::vector<std::vector<int>> backward;

    std::vector<std::vector<int>> sccs;
public:
    void initValues();
    void readGraph(char* file_path);
    void findScc();
    std::vector<std::vector<int>> getGraph();
    std::vector<std::vector<int>> getSccs();
};

void FB::initValues(){
    //nothing to do
    return;
}

void FB::readGraph(char* file_path){
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
    forward = std::vector<std::vector<int>>(num_nodes);
    backward = std::vector<std::vector<int>>(num_nodes);

    int u, v;
    while(fscanf(fptr, "%d %d", &u, &v) > 0 ){
		graph[u].push_back(v);
        forward[u].push_back(v);
        backward[v].push_back(u);
	}
}

void FB::findScc(){
    // remove nodes with only outgoing edges and only incoming edges;
    return;
}

std::vector<std::vector<int>> FB::getGraph(){
    return graph;
}

std::vector<std::vector<int>> FB::getSccs(){
    return sccs;
}

void printGraph(std::vector<std::vector<int>> graph){
    for(int i=0; i<graph.size(); i++){
        printf("node %d: ", i);
        for(int j=0; j<graph[i].size(); j++){
            printf("%d, ", graph[i][j]);
        }
        printf("\n");
    }
}

void printScc(std::vector<std::vector<int>> sccs){
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
    
    printf("file path: %s\nthread_count: %d\n\n", 
            file_path, 
            thread_count);

    //TODO: get argument for the method, and choose the class
    TrajanSeq method;
    method.readGraph(file_path);
    method.initValues();
    method.findScc();
    method.freeValues();

    auto sccs = method.getSccs();
    printScc(sccs);

    printf("scc count: %d\n", scc_count);

    return 0;
}