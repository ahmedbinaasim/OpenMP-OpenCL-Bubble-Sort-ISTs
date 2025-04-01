// #ifndef BUBBLE_SORT_NETWORK_H
// #define BUBBLE_SORT_NETWORK_H

// typedef struct {
//     int n;              // Dimension of permutation
//     int* elements;      // Array of elements [1...n]
// } Permutation;

// typedef struct {
//     int dimension;      // Dimension n of B_n
//     int vertex_count;   // Total number of vertices (n!)
//     int* adjacency;     // Adjacency list representation (CSR format)
//     int* offsets;       // Offsets for CSR format
// } BubbleSortNetwork;

// // Function prototypes
// BubbleSortNetwork* create_bubble_sort_network(int dimension);
// void free_bubble_sort_network(BubbleSortNetwork* network);
// Permutation* index_to_permutation(int index, int dimension);
// int permutation_to_index(Permutation* perm, int dimension);
// int is_identity_permutation(Permutation* perm);
// void free_permutation(Permutation* perm);

// #endif // BUBBLE_SORT_NETWORK_H


#ifndef BUBBLE_SORT_NETWORK_H
#define BUBBLE_SORT_NETWORK_H

typedef struct {
    int n;              // Dimension of permutation
    int* elements;      // Array of elements [1...n]
} Permutation;

typedef struct {
    int dimension;      // Dimension n of B_n
    int vertex_count;   // Total number of vertices (n!)
    int* adjacency;     // Adjacency list representation (CSR format)
    int* offsets;       // Offsets for CSR format
} BubbleSortNetwork;

// Function prototypes
BubbleSortNetwork* create_bubble_sort_network(int dimension);
void free_bubble_sort_network(BubbleSortNetwork* network);
Permutation* index_to_permutation(int index, int dimension);
int permutation_to_index(Permutation* perm, int dimension);
int is_identity_permutation(Permutation* perm);
int right_position(Permutation* perm);
int find_position(Permutation* perm, int value);
Permutation* copy_permutation(Permutation* perm);
void free_permutation(Permutation* perm);

#endif // BUBBLE_SORT_NETWORK_H