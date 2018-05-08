/*
 * This example shows how to execute code on the cluster.
 * The main entry point is entered on the fabric controller which can then
 * remotly call a function on cluster side.
 * The call in this test is synchronous to simplify but should be asynchronous
 * in real application to keep the fabric controller doing other stuff.
 */

#include "rt/rt_api.h"
#include "stdio.h"



// This function is entered on cluster side when the fabric controller
// is calling it remotly using rt_cluster_call.
// Only core 0 is entering it.
static void cluster_entry(void *arg)
{
  printf("Entered cluster with arg %x\n", (int)arg);
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
