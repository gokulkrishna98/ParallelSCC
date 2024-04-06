#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include <bits/stdc++.h>


#define UNVISITED -1

char *file_path = nullptr;
int thread_count = 1;

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
    std::vector<std::vector<int>> graph;
    std::vector<std::vector<int>> forward;
    std::vector<std::vector<int>> backward;
    std::vector<std::vector<int>> sccs;

    int* vis = nullptr;
public:
    int num_nodes = 0;
    int num_edges = 0;
    int scc_count = 0;

    void initValues();
    void readGraph(char* file_path);
    void freeValues();

    std::vector<int> dfs(std::vector<std::vector<int>> &g, int x);
    void findScc(std::vector<int> graph);

    int in_degree(int x);
    int out_degree(int x);
    std::vector<std::vector<int>> getGraph();
    std::vector<std::vector<int>> getSccs();
};

void FB::initValues(){
    vis = (int*) calloc(num_nodes, sizeof(int));

    for(int i=0; i<num_nodes; i++){
        vis[i] = UNVISITED;
    }
}

void FB::freeValues(){
    free(vis);
    vis = nullptr;
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

int FB::in_degree(int x){
    int in_nodes = 0;
    for(int i=0; i<backward[x].size(); i++){
        if(vis[backward[x][i]] == UNVISITED)
            in_nodes++;
    }
    return in_nodes;
}

int FB::out_degree(int x){
    int out_nodes = 0;
    for(int i=0; i<forward[x].size(); i++){
        if(vis[forward[x][i]] == UNVISITED)
            out_nodes++;
    }
    return out_nodes;
}

std::vector<int> FB::dfs(std::vector<std::vector<int>> &g, int x){
    std::vector<int> ans;
    vis[x] = 1;
    ans.push_back(x);
    for(int i=0; i<g[x].size(); i++){
        if(vis[g[x][i]] == UNVISITED){
            auto t = dfs(g, g[x][i]);
            ans.insert(ans.end(), t.begin(), t.end());
        }
    }
    return ans;
}

void print_graph(std::vector<int> g, std::string name){
    printf("%s: ", name.c_str());
    for(auto it: g){
        printf("%d, ", it);
    }
    printf("\n");
    return;
}

void FB::findScc(std::vector<int> graph){
    if(graph.size() == 0){
        return;
    }

    // print_graph(graph, "graph");
    for(int i=0; i<graph.size(); i++){
        if(in_degree(graph[i]) == 0 || out_degree(graph[i]) == 0){
            // printf("debug trim: %d\n", graph[i]);
            vis[graph[i]] = 1;
            scc_count++;
            sccs.push_back({ graph[i] });
        }
    }

    std::vector<int> temp;
    for(auto it: graph){
        if(vis[it] == UNVISITED)
            temp.push_back(it);
    }
    graph = temp;

    // getting node after trim
    int x = -1;
    for(int i=0; i<graph.size(); i++){
        if(vis[graph[i]] == UNVISITED){
            x = graph[i];
            break;
        }   
    }

    if(x < 0){
        return;
    }

    // printf("x: %d\n", x);

    auto fw = dfs(forward, x);
    for(auto it: fw){
        vis[it] = UNVISITED;
    }
    auto bw = dfs(backward, x);
    for(auto it: bw){
        vis[it] = UNVISITED;
    }

    std::vector<int> scc, graph1, graph2, graph3, union_graph;
    
    sort(fw.begin(), fw.end());
    sort(bw.begin(), bw.end());

    // print_graph(fw, "fw");
    // print_graph(bw, "bw");

    std::set_intersection(fw.begin(), fw.end(), 
                          bw.begin(), bw.end(),
                          std::back_inserter(scc));

    for(auto it: scc){
        vis[it] = 1;
    }

    if(!scc.empty()){
        scc_count++;
        sccs.push_back(scc);
    }

    std::set_union(fw.begin(), fw.end(),
                    bw.begin(), bw.end(),
                    std::back_inserter(union_graph));

    std::set_difference(fw.begin(), fw.end(),
                        scc.begin(), scc.end(),
                        std::back_inserter(graph1));

    std::set_difference(bw.begin(), bw.end(),
                        scc.begin(), scc.end(),
                        std::back_inserter(graph2));

    std::set_difference(graph.begin(), graph.end(),
                        union_graph.begin(), union_graph.end(),
                        std::back_inserter(graph3));


    // print_graph(scc, "scc");
    // print_graph(graph1, "graph1");
    // print_graph(graph2, "graph2");
    // print_graph(graph3, "graph3");
    // printf("--------------------\n");

    findScc(graph1);
    findScc(graph2);
    findScc(graph3);

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

    // TODO: get argument for the method, and choose the class
    TrajanSeq method;
    method.readGraph(file_path);
    method.initValues();
    method.findScc();
    method.freeValues();

    // FB method;
    // method.readGraph(file_path);
    // method.initValues();
    
    // std::vector<int> graph(method.num_nodes);
    // for(int i=0; i<method.num_nodes; i++){
    //     graph[i] = i;
    // }

    // method.findScc(graph);
    auto sccs = method.getSccs();
    printScc(sccs);

    printf("scc count: %ld\n", sccs.size());

    return 0;
}