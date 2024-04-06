parallel_scc: parallel_scc.cpp 
	g++-11 -o parallel_scc -g parallel_scc.cpp -I. -fopenmp
clean:
	rm -f parallel_scc *~