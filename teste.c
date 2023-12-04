#include <stdio.h>
#include <omp.h>

int main() {
    #pragma omp parallel
    {
        #pragma omp single
        {
            int num = omp_get_num_threads();
            printf("%d\n", num);
        }
    }
}