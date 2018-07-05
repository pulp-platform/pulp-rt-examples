/*
 * This code resides in a separate file to be able to compile FC and cluster codes
 * separately in cae FC and cluster cores have different ISAs
 */

#include "rt/rt_api.h"
#include "stdio.h"
#include "cluster.h"


// This is entered by all cluster cores, some computation can then be done here
static void pe_entry(void *arg)
{
  printf("(%d, %d) Entered PE with arg %x\n", rt_cluster_id(), rt_core_id(), (int)arg);
}



// This function is entered on cluster side when the fabric controller
// is calling it remotly using rt_cluster_call.
// Only core 0 is entering it and can then use rt_team_fork to fork
// the execution on multiple cores.
void cluster_entry(void *arg)
{
  printf("(%d, %d) Entered cluster with arg %x\n", rt_cluster_id(), rt_core_id(), (int)arg);

  // This will make all available cores entering pe_entry, including
  // core 0 which is calling this function.
  // Core 0 will return from rt_team_fork only when all cores have returned
  // from pe_entry (there is an implicit barrier).
  rt_team_fork(0, pe_entry, (void *)0x12345678);
}
