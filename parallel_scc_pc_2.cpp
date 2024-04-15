#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include <bits/stdc++.h>


#define UNVISITED -1

char *file_path = nullptr;
int thread_count = 1;
std::string algo = "tarjan";
bool use_parallel_bfs = false;

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

    std::vector<int> vis;
public:
    int num_nodes = 0;
    int num_edges = 0;
    int scc_count = 0;

    void initValues();
    void readGraph(char* file_path);
    void freeValues();

    std::set<int> bfs(std::vector<std::vector<int>> &g, int x, std::vector<int> &v);
    std::set<int> parallel_bfs(std::vector<std::vector<int>> &g, int x, std::vector<int> &v);
    std::set<int> dfs(std::vector<std::vector<int>> &g, int x, std::vector<int> &v);
    void findScc(std::vector<int> &graph);

    int in_degree(int x);
    int out_degree(int x);
    std::vector<std::vector<int>> getGraph();
    std::vector<std::vector<int>> getSccs();
};

void FB::initValues(){
    vis = std::vector<int>(num_nodes, UNVISITED);
}

void FB::freeValues(){
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

    forward = std::vector<std::vector<int>>(num_nodes);
    backward = std::vector<std::vector<int>>(num_nodes);

    initValues();

    int u, v;
    while(fscanf(fptr, "%d %d", &u, &v) > 0 ){
        forward[u].push_back(v);
        backward[v].push_back(u);
	}
}

int FB::in_degree(int x){
    for(int i=0; i<backward[x].size(); i++){
        if(vis[backward[x][i]] == UNVISITED){
            return 1;
        }
    }
    return 0;
}

int FB::out_degree(int x){
    for(int i=0; i<forward[x].size(); i++){
        if(vis[forward[x][i]] == UNVISITED)
            return 1;
    }
    return 0;
}

std::set<int> FB::dfs(std::vector<std::vector<int>> &g, int x, std::vector<int> &v){
    std::set<int> ans;
    v[x] = 1;
    ans.insert(x);
    for(int i=0; i<g[x].size(); i++){
        if(v[g[x][i]] == UNVISITED){
            auto t = dfs(g, g[x][i], v);
            std::set_union(t.begin(), t.end(),
                            ans.begin(), ans.end(),
                            std::inserter(ans, ans.begin()));
        }
    }
    return ans;
}

std::set<int> FB::bfs(std::vector<std::vector<int>> &g, int x, std::vector<int> &v){
    std::queue<int> q;
    std::set<int> ans;
    v[x] = 1;
    q.push(x);
    ans.insert(x);
    while(!q.empty()){
        int node = q.front(); q.pop();
        for(int i=0; i<g[node].size(); i++){
            if(v[g[node][i]] == UNVISITED){
                v[g[node][i]] = 1;
                ans.insert(g[node][i]);
                q.push(g[node][i]);
            }
        }
    }
    return ans;
}

std::set<int> FB::parallel_bfs(std::vector<std::vector<int>> &g, int x, std::vector<int> &v){
    std::queue<int> q;
    std::set<int> ans;
    v[x] = 1;
    q.push(x);
    ans.insert(x);

    while(!q.empty()){
        int node = q.front(); q.pop();
        #pragma omp parallel for num_threads(thread_count)
        for(int i=0; i<g[node].size(); i++){
            if(v[g[node][i]] == UNVISITED){
                v[g[node][i]] = 1;
                #pragma omp critical
                {
                    ans.insert(g[node][i]);
                    q.push(g[node][i]);
                }
            }
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

void FB::findScc(std::vector<int> &graph){
    int index = sccs.size();

    // --------------- TRIM -----------------------------------------
    #pragma omp parallel for num_threads(thread_count)
    for(int i=0; i<graph.size(); i++){
        if(vis[graph[i]] == UNVISITED &&
           (in_degree(graph[i]) == 0 || out_degree(graph[i]) == 0)){
            #pragma omp critical
            {
                vis[graph[i]] = 1;
                sccs.push_back({graph[i]});
            }
        }
    }
    // graph - trim
    std::vector<int> temp;
    for(auto it: graph){
        if(vis[it] == UNVISITED)
            temp.push_back(it);
    }
    graph = temp;
    if(graph.empty()) return;

    // ----------------------------- FW-BW reach -------------------
    // getting node after trim
    int x = graph[0];
    std::vector<int> fvis = vis;
    std::vector<int> bvis = vis;

    std::set<int> fw, bw;
    // these two can be done parallely
    if(use_parallel_bfs){
        fw = parallel_bfs(forward, x, fvis);
        bw = parallel_bfs(backward, x, bvis);
    }else{        
        fw = bfs(forward, x, fvis);
        bw = bfs(backward, x, bvis);
    }

    // --------------------------- PARTITIONING --------------------
    std::vector<int> scc, graph1, graph2, graph3, union_graph;
    std::set_intersection(fw.begin(), fw.end(), 
                          bw.begin(), bw.end(),
                          std::back_inserter(scc));
    
    // CRITICAL SECTION
    for(auto it: scc){
        vis[it] = 1;
    }
    if(!scc.empty()){
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

    // try to implement working queue (producer-consumer method)
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

/***************************************************************************
*****************    FORWARD-BACKWARD Producer Consumer ALGORITHM    *************************
****************************************************************************/

std::queue<std::vector<int>> jobQueue;
int number_sccs=0; 

class FBpc {
    std::vector<std::vector<int>> graph;
    std::vector<std::vector<int>> forward;
    std::vector<std::vector<int>> backward;
    std::vector<std::vector<int>> sccs;

    std::vector<int> vis;
public:
    int num_nodes = 0;
    int num_edges = 0;
    int scc_count = 0;

    void initValues();
    void readGraph(char* file_path);
    void freeValues();

    std::set<int> bfs(std::vector<std::vector<int>> &g, int x, std::vector<int> &v);
    std::set<int> parallel_bfs(std::vector<std::vector<int>> &g, int x, std::vector<int> &v);
    std::set<int> dfs(std::vector<std::vector<int>> &g, int x, std::vector<int> &v);
    void findScc(std::vector<int> graph);

    int in_degree(int x);
    int out_degree(int x);
    std::vector<std::vector<int>> getGraph();
    std::vector<std::vector<int>> getSccs();
};

void FBpc::initValues(){
    vis = std::vector<int>(num_nodes, UNVISITED);
}

void FBpc::freeValues(){
    return;
}

void FBpc::readGraph(char* file_path){
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

    forward = std::vector<std::vector<int>>(num_nodes);
    backward = std::vector<std::vector<int>>(num_nodes);

    initValues();

    int u, v;
    while(fscanf(fptr, "%d %d", &u, &v) > 0 ){
        forward[u].push_back(v);
        backward[v].push_back(u);
	}
}

int FBpc::in_degree(int x){
    for(int i=0; i<backward[x].size(); i++){
        if(vis[backward[x][i]] == UNVISITED){
            return 1;
        }
    }
    return 0;
}

int FBpc::out_degree(int x){
    for(int i=0; i<forward[x].size(); i++){
        if(vis[forward[x][i]] == UNVISITED)
            return 1;
    }
    return 0;
}

std::set<int> FBpc::dfs(std::vector<std::vector<int>> &g, int x, std::vector<int> &v){
    std::set<int> ans;
    v[x] = 1;
    ans.insert(x);
    for(int i=0; i<g[x].size(); i++){
        if(v[g[x][i]] == UNVISITED){
            auto t = dfs(g, g[x][i], v);
            std::set_union(t.begin(), t.end(),
                            ans.begin(), ans.end(),
                            std::inserter(ans, ans.begin()));
        }
    }
    return ans;
}

std::set<int> FBpc::bfs(std::vector<std::vector<int>> &g, int x, std::vector<int> &v){
    std::queue<int> q;
    std::set<int> ans;
    v[x] = 1;
    q.push(x);
    ans.insert(x);
    while(!q.empty()){
        int node = q.front(); q.pop();
        for(int i=0; i<g[node].size(); i++){
            if(v[g[node][i]] == UNVISITED){
                v[g[node][i]] = 1;
                ans.insert(g[node][i]);
                q.push(g[node][i]);
            }
        }
    }
    return ans;
}

std::set<int> FBpc::parallel_bfs(std::vector<std::vector<int>> &g, int x, std::vector<int> &v){
    std::queue<int> q;
    std::set<int> ans;
    v[x] = 1;
    q.push(x);
    ans.insert(x);

    while(!q.empty()){
        int node = q.front(); q.pop();
        #pragma omp parallel for num_threads(thread_count)
        for(int i=0; i<g[node].size(); i++){
            if(v[g[node][i]] == UNVISITED){
                v[g[node][i]] = 1;
                #pragma omp critical
                {
                    ans.insert(g[node][i]);
                    q.push(g[node][i]);
                }
            }
        }
    }

    return ans;
}



void FBpc::findScc(std::vector<int> graph){
    int index = sccs.size();
    // int index = number_sccs;

    // --------------- TRIM -----------------------------------------
    // #pragma omp parallel for num_threads(thread_count)   // COMEBACK
    for(int i=0; i<graph.size(); i++){
        if(vis[graph[i]] == UNVISITED &&
           (in_degree(graph[i]) == 0 || out_degree(graph[i]) == 0)){
            // vis[graph[i]] = 1;
            #pragma omp critical
            {
                vis[graph[i]] = 1;
                sccs.push_back({graph[i]});
                number_sccs+=1;
            }
        }
    }
    // graph - trim
    std::vector<int> temp;
    for(auto it: graph){
        if (vis[it] == UNVISITED)
            temp.push_back(it);
    }
    graph = temp;
    if(graph.empty()) 
        return;

    // ----------------------------- FW-BW reach -------------------
    // getting node after trim
    int x = graph[0];
    std::vector<int> fvis = vis;
    std::vector<int> bvis = vis;

    std::set<int> fw, bw;
    // these two can be done parallely
    if(use_parallel_bfs){
        fw = parallel_bfs(forward, x, fvis);
        bw = parallel_bfs(backward, x, bvis);
    }else{        
        fw = bfs(forward, x, fvis);
        bw = bfs(backward, x, bvis);
    }

    // --------------------------- PARTITIONING --------------------
    std::vector<int> scc, graph1, graph2, graph3, union_graph;
    std::set_intersection(fw.begin(), fw.end(), 
                          bw.begin(), bw.end(),
                          std::back_inserter(scc));
    
    // #pragma omp critical {

    // }
    // // CRITICAL SECTION
    // for(auto it: scc){
    //     vis[it] = 1;
    // }
    // if(!scc.empty()){
    //     sccs.push_back(scc);
    // }

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

    // try to implement working queue (producer-consumer method)
    // for (auto it: scc){
    //         vis[it] = 1;
    // }
    
    #pragma omp critical 
    {  
        for (auto it: scc){
            vis[it] = 1;
    }
        if(!scc.empty()){
            sccs.push_back(scc);
        }
        if(!graph1.empty())
            jobQueue.push(graph1);
        if(!graph2.empty())
            jobQueue.push(graph2);
        if(!graph3.empty())
            jobQueue.push(graph3);
        
        number_sccs+= scc.size();
    } 
    
    // findScc(graph1);
    // findScc(graph2);
    // findScc(graph3);   
    return;
}

std::vector<std::vector<int>> FBpc::getGraph(){
    return graph;
}

std::vector<std::vector<int>> FBpc::getSccs(){
    return sccs;
}


int main(int argc, char** argv){
    int opt;
    while((opt = getopt(argc, argv, "f:t:m:b")) != - 1){
        switch (opt){
            case 'f': {
                file_path = optarg;
                break;
            }

            case 't': {
                sscanf(optarg, "%d", &thread_count);
                break;
            }

            case 'm': {
                algo = std::string(optarg);
                break;
            }

            case 'b': {
                use_parallel_bfs = true;
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

    if(algo == "tarjan"){
        TrajanSeq method;
        method.readGraph(file_path);
        method.initValues();
        method.findScc();
        method.freeValues();
        auto sccs = method.getSccs();
        printf("scc count: %ld\n", sccs.size());
    }else if(algo == "fb"){
        FB method;
        method.readGraph(file_path);
    
        std::vector<int> graph(method.num_nodes);
        for(int i=0; i<method.num_nodes; i++){
            graph[i] = i;
        }
        method.findScc(graph);
        auto sccs = method.getSccs();
        printf("scc count: %ld\n", sccs.size());
    }
    else if(algo == "fbpc"){
        FBpc method;
        method.readGraph(file_path);
    
        std::vector<int> graph(method.num_nodes);
        for(int i=0; i<method.num_nodes; i++){
            graph[i] = i;
        }

        int graph_size = method.num_nodes;

        jobQueue.push(graph);

        while(number_sccs < graph_size) {

            int num_threads_to_use = thread_count;
            
            // if(jobQueue.size() >= thread_count)
            //     num_threads_to_use = thread_count;
            // else
            //     num_threads_to_use = jobQueue.size();
            
            #pragma omp parallel for num_threads(num_threads_to_use)
            for(int i=0; i<num_threads_to_use; i++){
                
                std::vector<int> frontVector;
                #pragma omp critical
                {   
                    if(!jobQueue.empty()){
                        frontVector = jobQueue.front();
                        jobQueue.pop(); 
                    }
                }
                method.findScc(frontVector);
            }
            // printf("scc nodes count: %d\n", number_sccs);
        }
        // method.findScc(graph);
        auto sccs = method.getSccs();
        printf("scc count: %ld\n", sccs.size());
        printf("num nodes: %d\n", graph_size);
        // printf("scc count: %ld\n", number_sccs);
    }

    // auto sccs = method.getSccs();
    // printScc(sccs);

    // printf("scc count: %ld\n", sccs.size());

    return 0;
}