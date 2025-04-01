#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>

int main(int argc, char* argv[]) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int thread_count = omp_get_num_threads();
        
        #pragma omp critical
        {
            printf("Hello from MPI rank %d/%d, OpenMP thread %d/%d\n", 
                   rank, size, thread_id, thread_count);
        }
    }
    
    MPI_Finalize();
    return 0;
}