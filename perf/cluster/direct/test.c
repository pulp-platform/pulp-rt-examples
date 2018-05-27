/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */


/*
 * This example shows how to use the performance API on cluster
 * side in order to get performance values out of the HW counters.
 * There are 2 ways to access counters, either directly to get the value 
 * directly out of the HW counter or indirectly where HW counter values are first
 * cumulated to the performance structure and then be read.
 * This example shows the direct way.
 */





#include "rt/rt_api.h"
#include "stdio.h"

static int cores_events;

// This benchmark is a single shot so we can read the value directly out of the
// HW counter using the function rt_perf_read
static void do_bench_0_pe(void *arg)
{
  rt_perf_t *perf = &((rt_perf_t *)arg)[rt_core_id()];

  // Activate specified events
  rt_perf_conf(perf, cores_events);

  // Reset HW counters now and start and stop counters so that we benchmark
  // only around the printf
  rt_perf_reset(perf);
  rt_perf_start(perf);

  printf("(%d, %d) Hello !\n", rt_cluster_id(), rt_core_id());
  // Put some dummy computation to see the difference between the cores
  volatile int i;
  for (i=0; i<rt_core_id()*100; i++);

  rt_perf_stop(perf);
}


// Make all the cores execute the benchmark so that we can read performance
// counters from all cores
static void do_bench_0(rt_perf_t *perf, int events)
{
  cores_events = events;
  rt_team_fork(0, do_bench_0_pe, (void *)perf);
}



static void display_events(void *arg)
{
  printf("(%d, %d) Instructions: %d\n", rt_cluster_id(), rt_core_id(), rt_perf_read(RT_PERF_INSTR));
}


static void cluster_entry()
{
  // This tructure will hold the configuration and also the results in the
  // cumulative mode
  rt_perf_t perf[rt_nb_pe()];

  // It must be initiliazed at least once, this will set all values in the
  // structure to zero.
  for (int i=0; i<rt_nb_pe(); i++)
  {
    rt_perf_init(&perf[i]);
  }


  // To be compatible with all platforms, we can count only 1 event at the
  // same time (the silicon as only 1 HW counter), but the total number of cyles
  // is reported by a timer, we can activate it at the same time.
  do_bench_0(perf, (1<<RT_PERF_CYCLES) | (1<<RT_PERF_INSTR));

  printf("(%d, %d) Total cycles: %d\n", rt_cluster_id(), rt_core_id(), rt_perf_read(RT_PERF_CYCLES));
  rt_team_fork(0, display_events, NULL);
}



int main()
{
  // This is executed by the fabric controller, enqueue a call to the cluster side
  // to read performance counters there
  rt_cluster_mount(1, 0, 0, NULL);

  if (rt_cluster_call(NULL, 0, cluster_entry, NULL, NULL, 0, 0, 0, NULL)) return -1;

  rt_cluster_mount(0, 0, 0, NULL);

  return 0;
}