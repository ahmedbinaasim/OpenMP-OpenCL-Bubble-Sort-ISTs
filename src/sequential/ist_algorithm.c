#include "ist_algorithm.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>

// Determine the parent of vertex v in tree t
// This is the core algorithm from the paper
Permutation* Parent1(Permutation* v, int t, int n) {
    // Create a copy of the permutation to work with
    Permutation* parent = copy_permutation(v);
    
    // Case A: Last symbol is n
    if (parent->elements[n-1] == n) {
        // Case A.1: Tree index is not n-1
        if (t != n - 1) {
            // FindPosition function from the paper
            // Case A.1.1: t != 2 or Swap(v, t) != identity
            if (t != 2 || !is_swap_identity(parent, t)) {
                // Case A.1.1.1: Second-to-last symbol is t or n-1
                if (parent->elements[n-2] == t || parent->elements[n-2] == n-1) {
                    int j = right_position(parent);
                    int pos = find_position(parent, j);
                    swap(&parent->elements[pos-1], &parent->elements[pos]);
                }
                // Case A.1.1.2: Second-to-last symbol is not t or n-1
                else {
                    int pos = find_position(parent, t);
                    swap(&parent->elements[pos-1], &parent->elements[pos]);
                }
            }
            // Case A.1.2: t = 2 and Swap(v, t) = identity
            else {
                int pos = find_position(parent, t-1);
                swap(&parent->elements[pos-1], &parent->elements[pos]);
            }
        }
        // Case A.2: Tree index is n-1
        else {
            int pos = find_position(parent, parent->elements[n-2]);
            swap(&parent->elements[pos-1], &parent->elements[pos]);
        }
    }
    // Case B: Last symbol is n-1
    else if (parent->elements[n-1] == n-1) {
        // Case B.1: Second-to-last symbol is not n or Swap(v, n) is identity
        if (parent->elements[n-2] != n || is_swap_identity(parent, n)) {
            // Case B.1.1: Last symbol is equal to tree index
            if (parent->elements[n-1] == t) {
                int pos = find_position(parent, n);
                swap(&parent->elements[pos-1], &parent->elements[pos]);
            }
            // Case B.1.2: Last symbol is not equal to tree index
            else {
                int pos = find_position(parent, t);
                swap(&parent->elements[pos-1], &parent->elements[pos]);
            }
        }
        // Case B.2: Second-to-last symbol is n and Swap(v, n) is not identity
        else {
            // Case B.2.1: Tree index is not 1
            if (t != 1) {
                int pos = find_position(parent, t-1);
                swap(&parent->elements[pos-1], &parent->elements[pos]);
            }
            // Case B.2.2: Tree index is 1
            else {
                int pos = find_position(parent, n);
                swap(&parent->elements[pos-1], &parent->elements[pos]);
            }
        }
    }
    // Case C: Last symbol is j where j is in {1, 2, ..., n-2}
    else {
        // Case C.1: Last symbol is equal to tree index
        if (parent->elements[n-1] == t) {
            int pos = find_position(parent, n);
            swap(&parent->elements[pos-1], &parent->elements[pos]);
        }
        // Case C.2: Last symbol is not equal to tree index
        else {
            int pos = find_position(parent, t);
            swap(&parent->elements[pos-1], &parent->elements[pos]);
        }
    }
    
    return parent;
}

// Check if swapping the position of value t results in the identity permutation
int is_swap_identity(Permutation* perm, int t) {
    // Create a copy to work with
    Permutation* copy = copy_permutation(perm);
    
    // Find position of t and swap with next position
    int pos = find_position(copy, t);
    swap(&copy->elements[pos-1], &copy->elements[pos]);
    
    // Check if result is identity
    int result = is_identity_permutation(copy);
    
    free_permutation(copy);
    return result;
}

// Construct n-1 independent spanning trees sequentially
IndependentSpanningTrees* construct_sequential_ists(BubbleSortNetwork* network) {
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
    
    // Construct each tree
    for (int v = 0; v < vertex_count; v++) {
        Permutation* perm = index_to_permutation(v, n);
        
        // Skip the root (identity permutation)
        if (is_identity_permutation(perm)) {
            free_permutation(perm);
            continue;
        }
        
        // Determine parent in each tree
        for (int t = 0; t < n - 1; t++) {
            Permutation* parent = Parent1(perm, t + 1, n); // t+1 because tree indices start at 1
            int parent_index = permutation_to_index(parent, n);
            ists->trees[t].parent[v] = parent_index;
            free_permutation(parent);
        }
        
        free_permutation(perm);
    }
    
    return ists;
}

// Free memory for a spanning tree
void free_spanning_tree(SpanningTree* tree) {
    if (tree) {
        if (tree->parent) free(tree->parent);
    }
}

// Free memory for independent spanning trees
void free_ists(IndependentSpanningTrees* ists) {
    if (ists) {
        if (ists->trees) {
            for (int t = 0; t < ists->tree_count; t++) {
                free_spanning_tree(&ists->trees[t]);
            }
            free(ists->trees);
        }
        free(ists);
    }
}