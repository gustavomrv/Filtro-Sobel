#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;

    MPI_Init(&argc, &argv); // Inicializa o ambiente MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Obtém o rank do processo
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Obtém o número total de ranks

    printf("Hello! Eu sou o rank %d de um total de %d ranks.\n", rank, size);

    MPI_Finalize(); // Finaliza o ambiente MPI

    return 0;
}
