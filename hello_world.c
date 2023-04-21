#include <stdio.h>
#include <omp.h>

int main() {
    
    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        int ip = omp_get_num_threads();
        printf("Hello World from Thread %d!\n", id);
    }
    
    return 0;
}


