parallel_scc: parallel_scc.cpp 
	g++-11 -o parallel_scc -g parallel_scc.cpp -I. -fopenmp

graphgen: random_directed_graphgen.cpp 
	g++-11 -o graphgen -g random_directed_graphgen.cpp -I.

clean:
	rm -f parallel_scc graphgen *~