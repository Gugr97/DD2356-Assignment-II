#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void generate_random(double *input, size_t size)
{
  for (size_t i = 0; i < size; i++) {
    input[i] = rand() / (double)(RAND_MAX);
  }
}

double serial_sum(double *x, size_t size)
{
  double sum_val = 0.0;

  for (size_t i = 0; i < size; i++) {
    sum_val += x[i];
  }

  return sum_val;
}

double omp_sum(double *x, size_t size)
{
  double sum_val = 0.0;

  #pragma omp parallel for
  for (size_t i = 0; i < size; i++) {
    sum_val += x[i];
  }

  return sum_val;
}

double omp_critical_sum(double *x, size_t size)
{
  double sum_val = 0.0;

  #pragma omp parallel for
  for (size_t i = 0; i < size; i++) {
    #pragma omp critical 
    {
    sum_val += x[i];
    }
  }

  return sum_val;
}

double omp_local_sum(double *x, size_t size)
{  
  int MAX_THREADS = omp_get_max_threads();

  double sum_vals[MAX_THREADS], sum_val;

  #pragma omp parallel shared(sum_vals)
  {
  int id = omp_get_thread_num();
  sum_vals[id] = 0.0;
  #pragma omp for
  for (size_t i = 0; i < size; i++) {
    sum_vals[id] += x[i];
    }
  }
  
  sum_val = serial_sum(sum_vals, MAX_THREADS);

  return sum_val;
}

double opt_local_sum(double *x, size_t size)
{  
  int MAX_THREADS = omp_get_max_threads();

  typedef struct { double val; char pad[128]; } tvals;

  tvals sum_vals[MAX_THREADS];

  #pragma omp parallel shared(sum_vals)
  {
  int id = omp_get_thread_num();
  sum_vals[id].val = 0.0;
  #pragma omp for
  for (size_t i = 0; i < size; i++) {
    sum_vals[id].val += x[i];
    }
  }
  
  double sum_val = 0.0;

  for (int j = 0; j < MAX_THREADS; j++) {
    sum_val += sum_vals[j].val;
    }

  return sum_val;
}

int main() {
    double start_time, end_time, elapsed_time;

    size_t size = 10000000;
    double *list = (double*)malloc(sizeof(double) * size);

    generate_random(list, size);

    start_time = omp_get_wtime();

    double sum = opt_local_sum(list, size);

    end_time = omp_get_wtime();
    elapsed_time = end_time - start_time;

    printf("Sum: %f\n", sum);

    printf("Elapsed time: %f seconds\n", elapsed_time);

    return 0;
}