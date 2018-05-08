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



// This is entered by all cluster cores, some computation can then be done here
static void pe_entry(void *arg)
{
  printf("(%d, %d) Entered PE with arg %x\n", rt_cluster_id(), rt_core_id(), (int)arg);
}



// This function is entered on cluster side when the fabric controller
// is calling it remotly using rt_cluster_call.
// Only core 0 is entering it and can then use rt_team_fork to fork
// the execution on multiple cores.
static void cluster_entry(void *arg)
{
  printf("(%d, %d) Entered cluster with arg %x\n", rt_cluster_id(), rt_core_id(), (int)arg);

  // This will make all available cores entering pe_entry, including
  // core 0 which is calling this function.
  // Core 0 will return from rt_team_fork only when all cores have returned
  // from pe_entry (there is an implicit barrier).
  rt_team_fork(0, pe_entry, (void *)0x12345678);
}



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
