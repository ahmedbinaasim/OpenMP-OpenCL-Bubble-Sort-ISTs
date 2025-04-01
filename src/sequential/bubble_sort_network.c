// #include "bubble_sort_network.h"
// #include <stdlib.h>

// // Create a bubble-sort network of dimension n
// BubbleSortNetwork* create_bubble_sort_network(int dimension) {
//     BubbleSortNetwork* network = (BubbleSortNetwork*)malloc(sizeof(BubbleSortNetwork));
//     if (!network) return NULL;
    
//     network->dimension = dimension;
//     network->vertex_count = factorial(dimension);
    
//     // TODO: Initialize adjacency list and offsets
    
//     return network;
// }

// // Convert index to permutation
// Permutation* index_to_permutation(int index, int dimension) {
//     Permutation* perm = (Permutation*)malloc(sizeof(Permutation));
//     if (!perm) return NULL;
    
//     perm->n = dimension;
//     perm->elements = (int*)malloc(dimension * sizeof(int));
//     if (!perm->elements) {
//         free(perm);
//         return NULL;
//     }
    
//     // TODO: Implement index to permutation conversion
    
//     return perm;
// }

// // Convert permutation to index
// int permutation_to_index(Permutation* perm, int dimension) {
//     // TODO: Implement permutation to index conversion
//     return 0;
// }

// // Check if the permutation is the identity
// int is_identity_permutation(Permutation* perm) {
//     for (int i = 0; i < perm->n; i++) {
//         if (perm->elements[i] != i + 1) return 0;
//     }
//     return 1;
// }

// // Free memory for a permutation
// void free_permutation(Permutation* perm) {
//     if (perm) {
//         if (perm->elements) free(perm->elements);
//         free(perm);
//     }
// }

// // Free memory for a bubble-sort network
// void free_bubble_sort_network(BubbleSortNetwork* network) {
//     if (network) {
//         if (network->adjacency) free(network->adjacency);
//         if (network->offsets) free(network->offsets);
//         free(network);
//     }
// }

#include "bubble_sort_network.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Create a bubble-sort network of dimension n
BubbleSortNetwork* create_bubble_sort_network(int dimension) {
    BubbleSortNetwork* network = (BubbleSortNetwork*)malloc(sizeof(BubbleSortNetwork));
    if (!network) return NULL;
    
    network->dimension = dimension;
    network->vertex_count = factorial(dimension);
    
    // Create adjacency list representation
    // For each vertex, we have (dimension-1) neighbors (one for each possible adjacent swap)
    int edge_count = network->vertex_count * (dimension - 1);
    network->adjacency = (int*)malloc(edge_count * sizeof(int));
    network->offsets = (int*)malloc((network->vertex_count + 1) * sizeof(int));
    
    if (!network->adjacency || !network->offsets) {
        free_bubble_sort_network(network);
        return NULL;
    }
    
    // Initialize adjacency list
    int edge_index = 0;
    for (int v = 0; v < network->vertex_count; v++) {
        network->offsets[v] = edge_index;
        
        // Get permutation for this vertex
        Permutation* perm = index_to_permutation(v, dimension);
        
        // Add edges for each possible adjacent swap
        for (int i = 0; i < dimension - 1; i++) {
            // Create a new permutation by swapping positions i and i+1
            Permutation* neighbor = copy_permutation(perm);
            swap(&neighbor->elements[i], &neighbor->elements[i+1]);
            
            // Convert back to index and add to adjacency list
            int neighbor_index = permutation_to_index(neighbor, dimension);
            network->adjacency[edge_index++] = neighbor_index;
            
            free_permutation(neighbor);
        }
        
        free_permutation(perm);
    }
    
    network->offsets[network->vertex_count] = edge_index;
    
    return network;
}

// Create a copy of a permutation
Permutation* copy_permutation(Permutation* perm) {
    Permutation* copy = (Permutation*)malloc(sizeof(Permutation));
    if (!copy) return NULL;
    
    copy->n = perm->n;
    copy->elements = (int*)malloc(perm->n * sizeof(int));
    if (!copy->elements) {
        free(copy);
        return NULL;
    }
    
    memcpy(copy->elements, perm->elements, perm->n * sizeof(int));
    return copy;
}

// Convert index to permutation using factorial number system
Permutation* index_to_permutation(int index, int dimension) {
    Permutation* perm = (Permutation*)malloc(sizeof(Permutation));
    if (!perm) return NULL;
    
    perm->n = dimension;
    perm->elements = (int*)malloc(dimension * sizeof(int));
    if (!perm->elements) {
        free(perm);
        return NULL;
    }
    
    // Initialize with all elements
    int* available = (int*)malloc(dimension * sizeof(int));
    for (int i = 0; i < dimension; i++) {
        available[i] = i + 1;
    }
    
    // Convert index to factorial representation
    int factor = factorial(dimension - 1);
    for (int i = 0; i < dimension - 1; i++) {
        int digit = index / factor;
        index %= factor;
        
        // Select the digit-th available element
        perm->elements[i] = available[digit];
        
        // Remove the selected element
        for (int j = digit; j < dimension - i - 1; j++) {
            available[j] = available[j + 1];
        }
        
        // Update factor for next position
        if (i < dimension - 2) {
            factor /= (dimension - i - 1);
        }
    }
    
    // Set the last element
    perm->elements[dimension - 1] = available[0];
    
    free(available);
    return perm;
}

// Convert permutation to index using factorial number system
int permutation_to_index(Permutation* perm, int dimension) {
    int index = 0;
    
    // Create a copy of the elements to work with
    int* elements_copy = (int*)malloc(dimension * sizeof(int));
    memcpy(elements_copy, perm->elements, dimension * sizeof(int));
    
    // Convert permutation to index using factorial number system
    for (int i = 0; i < dimension - 1; i++) {
        int value = elements_copy[i];
        
        // Count how many smaller values remain
        int smaller_count = 0;
        for (int j = i + 1; j < dimension; j++) {
            if (elements_copy[j] < value) {
                smaller_count++;
            }
        }
        
        // Adjust values after removing the current element
        for (int j = i + 1; j < dimension; j++) {
            if (elements_copy[j] > value) {
                elements_copy[j]--;
            }
        }
        
        // Add contribution to the index
        index += smaller_count * factorial(dimension - i - 1);
    }
    
    free(elements_copy);
    return index;
}

// Check if the permutation is the identity
int is_identity_permutation(Permutation* perm) {
    for (int i = 0; i < perm->n; i++) {
        if (perm->elements[i] != i + 1) return 0;
    }
    return 1;
}

// Position of the first symbol from the right which is not in the right position
int right_position(Permutation* perm) {
    for (int i = perm->n - 1; i >= 0; i--) {
        if (perm->elements[i] != i + 1) {
            return i + 1;
        }
    }
    return 0;  // All elements are in the right position
}

// Get the position of a value in the permutation
int find_position(Permutation* perm, int value) {
    for (int i = 0; i < perm->n; i++) {
        if (perm->elements[i] == value) {
            return i + 1;
        }
    }
    return -1;  // Value not found (should not happen)
}

// Free memory for a permutation
void free_permutation(Permutation* perm) {
    if (perm) {
        if (perm->elements) free(perm->elements);
        free(perm);
    }
}

// Free memory for a bubble-sort network
void free_bubble_sort_network(BubbleSortNetwork* network) {
    if (network) {
        if (network->adjacency) free(network->adjacency);
        if (network->offsets) free(network->offsets);
        free(network);
    }
}