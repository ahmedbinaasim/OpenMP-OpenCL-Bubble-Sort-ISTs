// #include "utils.h"
// #include <stdlib.h>
// #include <stdio.h>
// #include <time.h>
// #include <linux/time.h>

// // Calculate factorial of n
// int factorial(int n) {
//     int result = 1;
//     for (int i = 2; i <= n; i++) {
//         result *= i;
//     }
//     return result;
// }

// // Swap two integers
// void swap(int* a, int* b) {
//     int temp = *a;
//     *a = *b;
//     *b = temp;
// }

// // Measure time in seconds
// double measure_time() {
//     struct timespec ts;
//     clock_gettime(CLOCK_MONOTONIC, &ts);
//     return ts.tv_sec + ts.tv_nsec * 1e-9;
// }

// // Print a permutation
// void print_permutation(Permutation* perm) {
//     printf("(");
//     for (int i = 0; i < perm->n; i++) {
//         printf("%d", perm->elements[i]);
//         if (i < perm->n - 1) printf(" ");
//     }
//     printf(")\n");
// }

#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>  // For clock_gettime
#include <unistd.h> // For _POSIX_MONOTONIC_CLOCK

// Calculate factorial of n
int factorial(int n) {
    if (n <= 0) return 1;
    
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

// Swap two integers
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Measure time in seconds
double measure_time() {
    struct timespec ts;
#ifdef CLOCK_MONOTONIC
    clock_gettime(CLOCK_MONOTONIC, &ts);
#else
    clock_gettime(CLOCK_REALTIME, &ts);
#endif
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

// Print a permutation
void print_permutation(Permutation* perm) {
    printf("(");
    for (int i = 0; i < perm->n; i++) {
        printf("%d", perm->elements[i]);
        if (i < perm->n - 1) printf(" ");
    }
    printf(")");
}

// Verify that a spanning tree is valid
int verify_spanning_tree(SpanningTree* tree, BubbleSortNetwork* network) {
    // Check that all vertices (except root) have a parent
    for (int v = 0; v < network->vertex_count; v++) {
        // Skip the root (identity permutation)
        Permutation* perm = index_to_permutation(v, network->dimension);
        if (is_identity_permutation(perm)) {
            free_permutation(perm);
            continue;
        }
        
        // Check if parent is valid
        int parent = tree->parent[v];
        if (parent < 0 || parent >= network->vertex_count) {
            printf("Invalid parent for vertex %d: %d\n", v, parent);
            free_permutation(perm);
            return 0;
        }
        
        // Check if there's an edge from v to parent
        int is_neighbor = 0;
        for (int e = network->offsets[v]; e < network->offsets[v+1]; e++) {
            if (network->adjacency[e] == parent) {
                is_neighbor = 1;
                break;
            }
        }
        
        if (!is_neighbor) {
            printf("Parent of vertex %d is not a neighbor\n", v);
            free_permutation(perm);
            return 0;
        }
        
        free_permutation(perm);
    }
    
    // Check for cycles
    int* visited = (int*)calloc(network->vertex_count, sizeof(int));
    for (int v = 0; v < network->vertex_count; v++) {
        Permutation* perm = index_to_permutation(v, network->dimension);
        if (is_identity_permutation(perm)) {
            free_permutation(perm);
            continue;
        }
        
        // Trace path to root
        int current = v;
        while (current != 0) {  // 0 is the identity permutation
            if (visited[current]) {
                printf("Cycle detected at vertex %d\n", current);
                free(visited);
                free_permutation(perm);
                return 0;
            }
            
            visited[current] = 1;
            current = tree->parent[current];
            
            // Check for invalid parent
            if (current < 0 || current >= network->vertex_count) {
                printf("Path from vertex %d leads to invalid vertex %d\n", v, current);
                free(visited);
                free_permutation(perm);
                return 0;
            }
        }
        
        // Reset visited for next vertex
        for (int i = 0; i < network->vertex_count; i++) {
            visited[i] = 0;
        }
        
        free_permutation(perm);
    }
    
    free(visited);
    return 1;
}

// Verify that trees are independent
int verify_independence(IndependentSpanningTrees* ists, BubbleSortNetwork* network) {
    // Check each pair of trees
    for (int t1 = 0; t1 < ists->tree_count; t1++) {
        for (int t2 = t1 + 1; t2 < ists->tree_count; t2++) {
            // Check each vertex
            for (int v = 0; v < network->vertex_count; v++) {
                Permutation* perm = index_to_permutation(v, network->dimension);
                
                // Skip the root
                if (is_identity_permutation(perm)) {
                    free_permutation(perm);
                    continue;
                }
                
                // Trace paths to root in both trees
                int* path1 = (int*)malloc(network->vertex_count * sizeof(int));
                int* path2 = (int*)malloc(network->vertex_count * sizeof(int));
                int len1 = 0, len2 = 0;
                
                // Get path in first tree
                int current = v;
                while (current != 0) {  // 0 is the identity permutation
                    path1[len1++] = current;
                    current = ists->trees[t1].parent[current];
                }
                
                // Get path in second tree
                current = v;
                while (current != 0) {  // 0 is the identity permutation
                    path2[len2++] = current;
                    current = ists->trees[t2].parent[current];
                }
                
                // Check for common vertices (except for v and root)
                for (int i = 0; i < len1; i++) {
                    for (int j = 0; j < len2; j++) {
                        if (path1[i] == path2[j] && path1[i] != v) {
                            printf("Trees %d and %d share vertex %d in paths from %d to root\n", 
                                   t1+1, t2+1, path1[i], v);
                            free(path1);
                            free(path2);
                            free_permutation(perm);
                            return 0;
                        }
                    }
                }
                
                free(path1);
                free(path2);
                free_permutation(perm);
            }
        }
    }
    
    return 1;
}

// Print a spanning tree
void print_spanning_tree(SpanningTree* tree, BubbleSortNetwork* network) {
    printf("Spanning Tree:\n");
    for (int v = 0; v < network->vertex_count; v++) {
        Permutation* perm = index_to_permutation(v, network->dimension);
        
        // Skip the root
        if (is_identity_permutation(perm)) {
            free_permutation(perm);
            continue;
        }
        
        Permutation* parent_perm = index_to_permutation(tree->parent[v], network->dimension);
        
        printf("  ");
        print_permutation(perm);
        printf(" -> ");
        print_permutation(parent_perm);
        printf("\n");
        
        free_permutation(perm);
        free_permutation(parent_perm);
    }
}