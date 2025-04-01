// // #ifndef IST_ALGORITHM_H
// // #define IST_ALGORITHM_H

// // #include "bubble_sort_network.h"

// // typedef struct {
// //     int vertex_count;   // Number of vertices
// //     int* parent;        // Parent pointers for each vertex
// // } SpanningTree;

// // typedef struct {
// //     int tree_count;     // Number of trees (n-1)
// //     SpanningTree* trees; // Array of spanning trees
// // } IndependentSpanningTrees;

// // // Function prototypes
// // Permutation* Parent1(Permutation* v, int t, int n);
// // IndependentSpanningTrees* construct_sequential_ists(BubbleSortNetwork* network);
// // void construct_parallel_ists_mpi(BubbleSortNetwork* network);
// // void construct_hybrid_ists(BubbleSortNetwork* network);
// // void free_spanning_tree(SpanningTree* tree);
// // void free_ists(IndependentSpanningTrees* ists);

// // #endif // IST_ALGORITHM_H

// #ifndef IST_ALGORITHM_H
// #define IST_ALGORITHM_H

// #include "bubble_sort_network.h"

// typedef struct {
//     int vertex_count;   // Number of vertices
//     int* parent;        // Parent pointers for each vertex
// } SpanningTree;

// typedef struct {
//     int tree_count;     // Number of trees (n-1)
//     SpanningTree* trees; // Array of spanning trees
// } IndependentSpanningTrees;

// // Function prototypes
// Permutation* Parent1(Permutation* v, int t, int n);
// int is_swap_identity(Permutation* perm, int t);
// IndependentSpanningTrees* construct_sequential_ists(BubbleSortNetwork* network);
// void free_spanning_tree(SpanningTree* tree);
// void free_ists(IndependentSpanningTrees* ists);

// #endif // IST_ALGORITHM_H

#ifndef IST_ALGORITHM_H
#define IST_ALGORITHM_H

#include "bubble_sort_network.h"

typedef struct {
    int vertex_count;   // Number of vertices
    int* parent;        // Parent pointers for each vertex
} SpanningTree;

typedef struct {
    int tree_count;     // Number of trees (n-1)
    SpanningTree* trees; // Array of spanning trees
} IndependentSpanningTrees;

// Function prototypes for sequential implementation
Permutation* Parent1(Permutation* v, int t, int n);
int is_swap_identity(Permutation* perm, int t);
IndependentSpanningTrees* construct_sequential_ists(BubbleSortNetwork* network);

// Function prototypes for parallel implementation
void construct_parallel_ists_mpi(BubbleSortNetwork* network, IndependentSpanningTrees* ists);
IndependentSpanningTrees* mpi_construct_ists(BubbleSortNetwork* network);

// Function prototypes for hybrid implementation
void construct_hybrid_ists(BubbleSortNetwork* network, IndependentSpanningTrees* ists);
IndependentSpanningTrees* hybrid_construct_ists(BubbleSortNetwork* network);

// Memory management functions
void free_spanning_tree(SpanningTree* tree);
void free_ists(IndependentSpanningTrees* ists);

#endif // IST_ALGORITHM_H