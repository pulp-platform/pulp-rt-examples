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



// This benchmark is a single shot so we can read the value directly out of the
// HW counter using the function rt_perf_read
static void do_bench_0(rt_perf_t *perf, int events)
{
  // Activate specified events
  rt_perf_conf(perf, events);

  // Reset HW counters now and start and stop counters so that we benchmark
  // only around the printf
  rt_perf_reset(perf);
  rt_perf_start(perf);

  printf("Hello !\n");

  rt_perf_stop(perf);
}




int main()
{
  // This tructure will hold the configuration and also the results in the
  // cumulative mode
  rt_perf_t perf;

  // It must be initiliazed at least once, this will set all values in the
  // structure to zero.
  rt_perf_init(&perf);

  // To be compatible with all platforms, we can count only 1 event at the
  // same time (the silicon as only 1 HW counter), but the total number of cyles
  // is reported by a timer, we can activate it at the same time.
  do_bench_0(&perf, (1<<RT_PERF_CYCLES) | (1<<RT_PERF_INSTR));
  printf("Total cycles: %d\n", rt_perf_read(RT_PERF_CYCLES));
  printf("Instructions: %d\n", rt_perf_read(RT_PERF_INSTR));


  return 0;
}
