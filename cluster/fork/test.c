/*
 * This example shows how to use the performance API on fabric controller
 * side in order to get performance values out of the HW counters.
 * There are 2 ways to access counters, either directly to get the value 
 * directly out of the HW counter or indirectly where HW counter values are first
 * cumulated to the performance structure and then be read.
 * This example shows the direct way.
 */

#include "rt/rt_api.h"
#include "stdio.h"
#include "cluster.h"

int main()
{
  printf("Entering main controller\n");

  // Before being used, the cluster must be mounter, for example in case it must be
  // turned on.
  rt_cluster_mount(1, 0, 0, NULL);

  // This is the most basic call we can do to the cluster with all default
  // parameters (default stack size, max number of cores, etc) and is 
  // synchronous (last event parameter is NULL) which means we are blocked
  // until the call is finished
  rt_cluster_call(NULL, 0, cluster_entry, NULL, NULL, 0, 0, 0, NULL);

  // It must then be unmounted when it is not needed anymore so that it is turned off
  rt_cluster_mount(0, 0, 0, NULL);

  return 0;
}
