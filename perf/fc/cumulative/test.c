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
 * This example shows how to use the performance API on fabric controller
 * side in order to get performance values out of the HW counters.
 * There are 2 ways to access counters, either directly to get the value 
 * directly out of the HW counter or indirectly where HW counter values are first
 * cumulated to the performance structure and then be read.
 * This example shows the indirect way.
 */




#include "rt/rt_api.h"
#include "stdio.h"





// This benchmark is cumulating over several sequence of codes in the perf
// structure so that after several execution, we have the values for all
// counters
static void do_bench_1(rt_perf_t *perf, int events)
{
  // Activate specified events
  rt_perf_conf(perf, events);

  // Reset HW counters now and start and stop counters so that we benchmark
  // only around the printf
  rt_perf_reset(perf);
  rt_perf_start(perf);

  printf("Hello !\n");

  rt_perf_stop(perf);
  rt_perf_save(perf);
}


int main()
{
  // This tructure will hold the configuration and also the results in the
  // cumulative mode
  rt_perf_t perf;

  // It must be initiliazed at least once, this will set all values in the
  // structure to zero.
  rt_perf_init(&perf);


  // In this mode, we can launch several times the same code with different
  // events to get everything saved in the perf structure
  do_bench_1(&perf, (1<<RT_PERF_CYCLES) | (1<<RT_PERF_INSTR));
  do_bench_1(&perf, (1<<RT_PERF_ACTIVE_CYCLES));
  do_bench_1(&perf, (1<<RT_PERF_LD_STALL));
  do_bench_1(&perf, (1<<RT_PERF_JR_STALL));
  do_bench_1(&perf, (1<<RT_PERF_IMISS));
  do_bench_1(&perf, (1<<RT_PERF_LD));
  do_bench_1(&perf, (1<<RT_PERF_ST));
  do_bench_1(&perf, (1<<RT_PERF_JUMP));
  do_bench_1(&perf, (1<<RT_PERF_BRANCH));
  do_bench_1(&perf, (1<<RT_PERF_BTAKEN));
  do_bench_1(&perf, (1<<RT_PERF_RVC));
  do_bench_1(&perf, (1<<RT_PERF_LD_EXT));
  do_bench_1(&perf, (1<<RT_PERF_ST_EXT));
  do_bench_1(&perf, (1<<RT_PERF_LD_EXT_CYC));
  do_bench_1(&perf, (1<<RT_PERF_ST_EXT_CYC));
  do_bench_1(&perf, (1<<RT_PERF_TCDM_CONT));
  //do_bench_1(&perf, (1<<RT_PERF_CSR_HAZARD));

  printf("Total cycles: %d\n", rt_perf_get(&perf, RT_PERF_CYCLES));
  printf("Instructions: %d\n", rt_perf_get(&perf, RT_PERF_INSTR));
  printf("Active cycles: %d\n", rt_perf_get(&perf, RT_PERF_ACTIVE_CYCLES));
  printf("Load data hazards: %d\n", rt_perf_get(&perf, RT_PERF_LD_STALL));
  printf("Jump stalls: %d\n", rt_perf_get(&perf, RT_PERF_JR_STALL));
  printf("Instruction cache misses: %d\n", rt_perf_get(&perf, RT_PERF_IMISS));
  printf("Load accesses: %d\n", rt_perf_get(&perf, RT_PERF_LD));
  printf("Store accesses: %d\n", rt_perf_get(&perf, RT_PERF_ST));
  printf("Jumps: %d\n", rt_perf_get(&perf, RT_PERF_JUMP));
  printf("Branches: %d\n", rt_perf_get(&perf, RT_PERF_BRANCH));
  printf("Branches taken: %d\n", rt_perf_get(&perf, RT_PERF_BTAKEN));
  printf("Compressed instructions: %d\n", rt_perf_get(&perf, RT_PERF_RVC));
  printf("External load accesses: %d\n", rt_perf_get(&perf, RT_PERF_LD_EXT));
  printf("External store accesses: %d\n", rt_perf_get(&perf, RT_PERF_ST_EXT));
  printf("External load stall cycles: %d\n", rt_perf_get(&perf, RT_PERF_LD_EXT_CYC));
  printf("External store stall cycles: %d\n", rt_perf_get(&perf, RT_PERF_ST_EXT_CYC));
  printf("TCDM contention cycles: %d\n", rt_perf_get(&perf, RT_PERF_TCDM_CONT));
  //printf("CSR hazards: %d\n", rt_perf_get(&perf, RT_PERF_CSR_HAZARD));


  return 0;
}
