// #include "bubble_sort_network.h"
// #include "ist_algorithm.h"
// #include "utils.h"
// #include <stdio.h>
// #include <stdlib.h>

// int main(int argc, char* argv[]) {
//     // Check command line arguments
//     if (argc < 2) {
//         printf("Usage: %s <dimension>\n", argv[0]);
//         return 1;
//     }
    
//     int dimension = atoi(argv[1]);
//     if (dimension < 3) {
//         printf("Dimension must be at least 3\n");
//         return 1;
//     }
    
//     printf("Creating bubble-sort network B_%d...\n", dimension);
//     BubbleSortNetwork* network = create_bubble_sort_network(dimension);
//     if (!network) {
//         printf("Failed to create network\n");
//         return 1;
//     }
    
//     printf("Constructing %d independent spanning trees...\n", dimension - 1);
//     double start_time = measure_time();
//     IndependentSpanningTrees* ists = construct_sequential_ists(network);
//     double end_time = measure_time();
    
//     if (!ists) {
//         printf("Failed to construct ISTs\n");
//         free_bubble_sort_network(network);
//         return 1;
//     }
    
//     printf("Construction completed in %.6f seconds\n", end_time - start_time);
    
//     // Verify the spanning trees
//     printf("Verifying spanning trees...\n");
//     int valid = 1;
//     for (int t = 0; t < dimension - 1; t++) {
//         if (!verify_spanning_tree(&ists->trees[t], network)) {
//             printf("Tree %d is not a valid spanning tree\n", t + 1);
//             valid = 0;
//             break;
//         }
//     }
    
//     // Verify independence
//     if (valid) {
//         printf("Verifying independence...\n");
//         if (!verify_independence(ists, network)) {
//             printf("Trees are not independent\n");
//             valid = 0;
//         }
//     }
    
//     if (valid) {
//         printf("All trees are valid and independent\n");
//     }
    
//     // Clean up
//     free_ists(ists);
//     free_bubble_sort_network(network);
    
//     return 0;
// }

#include "bubble_sort_network.h"
#include "ist_algorithm.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    // Check command line arguments
    if (argc < 2) {
        printf("Usage: %s <dimension>\n", argv[0]);
        return 1;
    }
    
    int dimension = atoi(argv[1]);
    if (dimension < 3) {
        printf("Dimension must be at least 3\n");
        return 1;
    }
    
    printf("Creating bubble-sort network B_%d with %d vertices...\n", 
           dimension, factorial(dimension));
    
    double start_time = measure_time();
    BubbleSortNetwork* network = create_bubble_sort_network(dimension);
    double end_time = measure_time();
    
    if (!network) {
        printf("Failed to create network\n");
        return 1;
    }
    
    printf("Network created in %.6f seconds\n", end_time - start_time);
    
    // Test small examples
    if (dimension <= 4) {
        printf("\nTesting permutation conversion:\n");
        for (int i = 0; i < network->vertex_count; i++) {
            Permutation* perm = index_to_permutation(i, dimension);
            printf("  Index %d -> ", i);
            print_permutation(perm);
            int back = permutation_to_index(perm, dimension);
            printf(" -> Index %d\n", back);
            
            if (back != i) {
                printf("Error: Permutation conversion failed for index %d\n", i);
            }
            
            free_permutation(perm);
        }
    }
    
    printf("\nConstructing %d independent spanning trees...\n", dimension - 1);
    start_time = measure_time();
    IndependentSpanningTrees* ists = construct_sequential_ists(network);
    end_time = measure_time();
    
    if (!ists) {
        printf("Failed to construct ISTs\n");
        free_bubble_sort_network(network);
        return 1;
    }
    
    printf("ISTs constructed in %.6f seconds\n", end_time - start_time);
    
    // Print a small example tree if dimension is small
    if (dimension <= 3) {
        for (int t = 0; t < dimension - 1; t++) {
            printf("\nTree T_%d:\n", t+1);
            print_spanning_tree(&ists->trees[t], network);
        }
    }
    
    // Verify the spanning trees
    printf("\nVerifying spanning trees...\n");
    int valid = 1;
    for (int t = 0; t < dimension - 1; t++) {
        if (!verify_spanning_tree(&ists->trees[t], network)) {
            printf("Tree %d is not a valid spanning tree\n", t + 1);
            valid = 0;
            break;
        }
    }
    
    if (valid) {
        printf("All trees are valid spanning trees\n");
        
        // Verify independence
        printf("\nVerifying independence...\n");
        if (!verify_independence(ists, network)) {
            printf("Trees are not independent\n");
            valid = 0;
        } else {
            printf("All trees are independent\n");
        }
    }
    
    if (valid) {
        printf("\nSuccessfully constructed %d valid independent spanning trees\n", dimension - 1);
    }
    
    // Example path from a specific vertex to root in each tree
    if (dimension == 4) {
        // Example vertex: 4231 (as in Fig. 3 of the paper)
        int example_index = -1;
        for (int i = 0; i < network->vertex_count; i++) {
            Permutation* perm = index_to_permutation(i, dimension);
            if (perm->elements[0] == 4 && perm->elements[1] == 2 && 
                perm->elements[2] == 3 && perm->elements[3] == 1) {
                example_index = i;
                free_permutation(perm);
                break;
            }
            free_permutation(perm);
        }
        
        if (example_index != -1) {
            printf("\nExample paths from vertex 4231 to root in each tree:\n");
            for (int t = 0; t < dimension - 1; t++) {
                printf("Tree T_%d: ", t+1);
                
                // Trace path to root
                int current = example_index;
                while (current != 0) {  // 0 is the identity permutation
                    Permutation* perm = index_to_permutation(current, dimension);
                    print_permutation(perm);
                    printf(" -> ");
                    free_permutation(perm);
                    
                    current = ists->trees[t].parent[current];
                }
                
                // Print root
                Permutation* root = index_to_permutation(0, dimension);
                print_permutation(root);
                printf("\n");
                free_permutation(root);
            }
        }
    }
    
    // Clean up
    free_ists(ists);
    free_bubble_sort_network(network);
    
    return 0;
}