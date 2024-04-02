#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bits/stdc++.h>

char *file_path = nullptr;
int thread_count = 1;

int num_nodes = 0;
int num_edges = 0;
std::vector<std::vector<int>> graph;

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
        printf("%d -> size %ld\n", i, graph[i].size());
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

    printGraph();

    return 0;
}