// #ifndef UTILS_H
// #define UTILS_H

// #include "bubble_sort_network.h"
// #include "ist_algorithm.h"

// // Function prototypes for utility functions
// int factorial(int n);
// void swap(int* a, int* b);
// int verify_spanning_tree(SpanningTree* tree, BubbleSortNetwork* network);
// int verify_independence(IndependentSpanningTrees* ists, BubbleSortNetwork* network);
// void print_permutation(Permutation* perm);
// void print_spanning_tree(SpanningTree* tree, BubbleSortNetwork* network);
// double measure_time();
// int* partition_network_with_metis(BubbleSortNetwork* network, int num_parts);

// #endif // UTILS_H

#ifndef UTILS_H
#define UTILS_H

#include "bubble_sort_network.h"
#include "ist_algorithm.h"

// Function prototypes for utility functions
int factorial(int n);
void swap(int* a, int* b);
int verify_spanning_tree(SpanningTree* tree, BubbleSortNetwork* network);
int verify_independence(IndependentSpanningTrees* ists, BubbleSortNetwork* network);
void print_permutation(Permutation* perm);
void print_spanning_tree(SpanningTree* tree, BubbleSortNetwork* network);
double measure_time();

#endif // UTILS_H