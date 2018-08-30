#include "cluster.h"
#include "rt/rt_api.h"
#include "rt/rt_omp.h"


void cluster_entry(void *arg)
{
  // OpenMP runtime is not available by default and must be started
  rt_omp_start();

  printf("(%d, %d) Enter cluster entry\n", rt_cluster_id(), rt_core_id());

#pragma omp parallel
  {
    printf("(%d, %d) Enter parallel section\n", rt_cluster_id(), rt_core_id());
  }

#pragma omp parallel for schedule(static)
 for(int i =0; i<32;i++) 
  {
    printf("(%d, %d) Enter for iteration (iter=%d)\n", rt_cluster_id(), rt_core_id(), i);
  }

#pragma omp parallel
  {
    // Do some dummy computation to see the barrier effect
    volatile int i;
    for (i=0; i<omp_get_thread_num()*100; i++);

    #pragma omp barrier
  }

  int var = 0;

#pragma omp parallel shared(var)
  {
    for (int i=0; i<100; i++)
    {
      #pragma omp critical
      var++;
    }
  }
  printf("Var is %d\n", var);

  rt_omp_stop();
}
