#include "bubble_sort_network.h"
#include "ist_algorithm.h"
#include "utils.h"
#include <mpi.h>
#include <omp.h>
#include <stdlib.h>
#include <stdio.h>

// Construct independent spanning trees using hybrid MPI+OpenMP approach
void construct_hybrid_ists(BubbleSortNetwork* network, IndependentSpanningTrees* ists) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    int n = network->dimension;
    int vertex_count = network->vertex_count;
    
    // Calculate the portion of vertices each process will handle
    int vertices_per_proc = vertex_count / size;
    int remainder = vertex_count % size;
    
    // Calculate start and end vertices for this process
    int start_vertex = rank * vertices_per_proc + (rank < remainder ? rank : remainder);
    int end_vertex = start_vertex + vertices_per_proc + (rank < remainder ? 1 : 0);
    
    // Process each vertex in the local range with OpenMP parallelism
    #pragma omp parallel
    {
        #pragma omp for collapse(2)
        for (int v = start_vertex; v < end_vertex; v++) {
            for (int t = 0; t < n - 1; t++) {
                Permutation* perm = index_to_permutation(v, n);
                
                // Skip the root (identity permutation)
                if (is_identity_permutation(perm)) {
                    free_permutation(perm);
                    continue;
                }
                
                // Determine parent in the tree
                Permutation* parent = Parent1(perm, t + 1, n); // t+1 because tree indices start at 1
                int parent_index = permutation_to_index(parent, n);
                
                #pragma omp critical
                {
                    ists->trees[t].parent[v] = parent_index;
                }
                
                free_permutation(parent);
                free_permutation(perm);
            }
        }
    }
    
    // Gather all results to all processes
    for (int t = 0; t < n - 1; t++) {
        MPI_Allgather(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL,
                     ists->trees[t].parent, vertex_count, MPI_INT, MPI_COMM_WORLD);
    }
}

// Function to handle the hybrid MPI+OpenMP process for IST construction
IndependentSpanningTrees* hybrid_construct_ists(BubbleSortNetwork* network) {
    int n = network->dimension;
    int vertex_count = network->vertex_count;
    
    // Allocate memory for the ISTs
    IndependentSpanningTrees* ists = (IndependentSpanningTrees*)malloc(sizeof(IndependentSpanningTrees));
    if (!ists) return NULL;
    
    ists->tree_count = n - 1;
    ists->trees = (SpanningTree*)malloc((n - 1) * sizeof(SpanningTree));
    if (!ists->trees) {
        free(ists);
        return NULL;
    }
    
    // Initialize each tree
    for (int t = 0; t < n - 1; t++) {
        ists->trees[t].vertex_count = vertex_count;
        ists->trees[t].parent = (int*)malloc(vertex_count * sizeof(int));
        if (!ists->trees[t].parent) {
            for (int j = 0; j < t; j++) {
                free(ists->trees[j].parent);
            }
            free(ists->trees);
            free(ists);
            return NULL;
        }
        
        // Initialize parent pointers to -1
        for (int v = 0; v < vertex_count; v++) {
            ists->trees[t].parent[v] = -1;
        }
    }
    
    // Construct the trees using hybrid parallelism
    construct_hybrid_ists(network, ists);
    
    return ists;
}