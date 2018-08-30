/*
 * This example shows how to use OpenMP runtime on cluster side.
 * The fabric controller is running classic C code from the main entry point
 * and can offload OpenMP kernel execution to the cluster.
 */

#include <stdio.h>
#include "rt/rt_api.h"
#include "cluster.h"

int main()
{
  printf("Hello !\n");

  rt_cluster_mount(1, 0, 0, NULL);

  rt_cluster_call(NULL, 0, cluster_entry, NULL, NULL, 0, 0, rt_nb_pe(), NULL);
  
  rt_cluster_mount(0, 0, 0, NULL);

  return 0;
}
