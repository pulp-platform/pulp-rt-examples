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
 * This example shows the indirect way.
 */



#include "rt/rt_api.h"
#include "stdio.h"

static int cores_events;

// This tructure will hold the configuration and also the results in the
// cumulative mode
RT_L1_DATA static rt_perf_t perf[ARCHI_CLUSTER_NB_PE];


// This benchmark is cumulating over several sequence of codes in the perf
// structure so that after several execution, we have the values for all
// counters
static void do_bench_1_pe(void *arg)
{
  rt_perf_t *perf = &((rt_perf_t *)arg)[rt_core_id()];

  // Activate specified events
  rt_perf_conf(perf, cores_events);

  // Reset HW counters now and start and stop counters so that we benchmark
  // only around the printf
  rt_perf_reset(perf);
  rt_perf_start(perf);

  printf("Hello !\n");
  // Put some dummy computation to see the difference between the cores
  volatile int i;
  for (i=0; i<rt_core_id()*100; i++);

  rt_perf_stop(perf);
  rt_perf_save(perf);
}

// Make all the cores execute the benchmark so that we can read performance
// counters from all cores
static void do_bench_1(rt_perf_t *perf, int events)
{
  cores_events = events;

  rt_team_fork(0, do_bench_1_pe, (void *)perf);
}


static void display_events(void *arg)
{
  printf("(%d, %d) Instructions: %d\n", rt_cluster_id(), rt_core_id(), rt_perf_read(RT_PERF_INSTR));
}


static void cluster_entry()
{
  // It must be initiliazed at least once, this will set all values in the
  // structure to zero.
  for (int i=0; i<rt_nb_pe(); i++)
  {
    rt_perf_init(&perf[i]);
  }


  // In this mode, we can launch several times the same code with different
  // events to get everything saved in the perf structure
  do_bench_1(perf, (1<<RT_PERF_CYCLES) | (1<<RT_PERF_INSTR));
  do_bench_1(perf, (1<<RT_PERF_ACTIVE_CYCLES));
  do_bench_1(perf, (1<<RT_PERF_LD_STALL));
  do_bench_1(perf, (1<<RT_PERF_JR_STALL));
  do_bench_1(perf, (1<<RT_PERF_IMISS));
  do_bench_1(perf, (1<<RT_PERF_LD));
  do_bench_1(perf, (1<<RT_PERF_ST));
  do_bench_1(perf, (1<<RT_PERF_JUMP));
  do_bench_1(perf, (1<<RT_PERF_BRANCH));
  do_bench_1(perf, (1<<RT_PERF_BTAKEN));
  do_bench_1(perf, (1<<RT_PERF_RVC));
  do_bench_1(perf, (1<<RT_PERF_LD_EXT));
  do_bench_1(perf, (1<<RT_PERF_ST_EXT));
  do_bench_1(perf, (1<<RT_PERF_LD_EXT_CYC));
  do_bench_1(perf, (1<<RT_PERF_ST_EXT_CYC));
  do_bench_1(perf, (1<<RT_PERF_TCDM_CONT));
  //do_bench_1(perf, (1<<RT_PERF_CSR_HAZARD));

  // All cores have filled their own performance structure, thus just display an average
  printf("Total cycles: %d\n", rt_perf_get_average(perf, RT_PERF_CYCLES, rt_nb_pe()));
  printf("Instructions: %d\n", rt_perf_get_average(perf, RT_PERF_INSTR, rt_nb_pe()));
  printf("Active cycles: %d\n", rt_perf_get_average(perf, RT_PERF_ACTIVE_CYCLES, rt_nb_pe()));
  printf("Load data hazards: %d\n", rt_perf_get_average(perf, RT_PERF_LD_STALL, rt_nb_pe()));
  printf("Jump stalls: %d\n", rt_perf_get_average(perf, RT_PERF_JR_STALL, rt_nb_pe()));
  printf("Instruction cache misses: %d\n", rt_perf_get_average(perf, RT_PERF_IMISS, rt_nb_pe()));
  printf("Load accesses: %d\n", rt_perf_get_average(perf, RT_PERF_LD, rt_nb_pe()));
  printf("Store accesses: %d\n", rt_perf_get_average(perf, RT_PERF_ST, rt_nb_pe()));
  printf("Jumps: %d\n", rt_perf_get_average(perf, RT_PERF_JUMP, rt_nb_pe()));
  printf("Branches: %d\n", rt_perf_get_average(perf, RT_PERF_BRANCH, rt_nb_pe()));
  printf("Branches taken: %d\n", rt_perf_get_average(perf, RT_PERF_BTAKEN, rt_nb_pe()));
  printf("Compressed instructions: %d\n", rt_perf_get_average(perf, RT_PERF_RVC, rt_nb_pe()));
  printf("External load accesses: %d\n", rt_perf_get_average(perf, RT_PERF_LD_EXT, rt_nb_pe()));
  printf("External store accesses: %d\n", rt_perf_get_average(perf, RT_PERF_ST_EXT, rt_nb_pe()));
  printf("External load stall cycles: %d\n", rt_perf_get_average(perf, RT_PERF_LD_EXT_CYC, rt_nb_pe()));
  printf("External store stall cycles: %d\n", rt_perf_get_average(perf, RT_PERF_ST_EXT_CYC, rt_nb_pe()));
  printf("TCDM contention cycles: %d\n", rt_perf_get_average(perf, RT_PERF_TCDM_CONT, rt_nb_pe()));
  //printf("CSR hazards: %d\n", rt_perf_get_average(perf, RT_PERF_CSR_HAZARD, rt_nb_pe()));
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