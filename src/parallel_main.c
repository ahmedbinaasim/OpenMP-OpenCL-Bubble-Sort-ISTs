// #include "bubble_sort_network.h"
// #include "ist_algorithm.h"
// #include "utils.h"
// #include <stdio.h>
// #include <stdlib.h>
// #include <mpi.h>

// int main(int argc, char* argv[]) {
//     int rank, size;
    
//     // Initialize MPI
//     MPI_Init(&argc, &argv);
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//     MPI_Comm_size(MPI_COMM_WORLD, &size);
    
//     // Check command line arguments
//     if (argc < 2) {
//         if (rank == 0) {
//             printf("Usage: %s <dimension>\n", argv[0]);
//         }
//         MPI_Finalize();
//         return 1;
//     }
    
//     int dimension = atoi(argv[1]);
//     if (dimension < 3) {
//         if (rank == 0) {
//             printf("Dimension must be at least 3\n");
//         }
//         MPI_Finalize();
//         return 1;
//     }
    
//     if (rank == 0) {
//         printf("Creating bubble-sort network B_%d...\n", dimension);
//     }
    
//     BubbleSortNetwork* network = create_bubble_sort_network(dimension);
//     if (!network) {
//         if (rank == 0) {
//             printf("Failed to create network\n");
//         }
//         MPI_Finalize();
//         return 1;
//     }
    
//     if (rank == 0) {
//         printf("Constructing %d independent spanning trees in parallel with %d processes...\n", 
//                dimension - 1, size);
//     }
    
//     double start_time = MPI_Wtime();
    
//     // Construct ISTs in parallel
//     construct_parallel_ists_mpi(network);
    
//     double end_time = MPI_Wtime();
    
//     if (rank == 0) {
//         printf("Construction completed in %.6f seconds\n", end_time - start_time);
//     }
    
//     // Clean up
//     free_bubble_sort_network(network);
    
//     MPI_Finalize();
//     return 0;
// }

#include "bubble_sort_network.h"
#include "ist_algorithm.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Function declarations for parallel implementation
IndependentSpanningTrees* mpi_construct_ists(BubbleSortNetwork* network);

int main(int argc, char* argv[]) {
    int rank, size;
    
    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Check command line arguments
    if (argc < 2) {
        if (rank == 0) {
            printf("Usage: %s <dimension>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }
    
    int dimension = atoi(argv[1]);
    if (dimension < 3) {
        if (rank == 0) {
            printf("Dimension must be at least 3\n");
        }
        MPI_Finalize();
        return 1;
    }
    
    // Only rank 0 prints the initial information
    if (rank == 0) {
        printf("Creating bubble-sort network B_%d with %d vertices...\n", 
               dimension, factorial(dimension));
        printf("Using %d MPI processes\n", size);
    }
    
    // Create the bubble-sort network
    double start_time = MPI_Wtime();
    BubbleSortNetwork* network = create_bubble_sort_network(dimension);
    double end_time = MPI_Wtime();
    
    if (!network) {
        if (rank == 0) {
            printf("Failed to create network\n");
        }
        MPI_Finalize();
        return 1;
    }
    
    if (rank == 0) {
        printf("Network created in %.6f seconds\n", end_time - start_time);
        printf("\nConstructing %d independent spanning trees in parallel...\n", dimension - 1);
    }
    
    // Construct independent spanning trees in parallel
    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();
    IndependentSpanningTrees* ists = mpi_construct_ists(network);
    MPI_Barrier(MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    
    if (!ists) {
        if (rank == 0) {
            printf("Failed to construct ISTs\n");
        }
        free_bubble_sort_network(network);
        MPI_Finalize();
        return 1;
    }
    
    if (rank == 0) {
        printf("ISTs constructed in %.6f seconds\n", end_time - start_time);
        
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
    }
    
    // Clean up
    free_ists(ists);
    free_bubble_sort_network(network);
    
    MPI_Finalize();
    return 0;
}