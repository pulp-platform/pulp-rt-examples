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





#include "rt/rt_api.h"
#include "hal/gvsoc/gvsoc.h"
#include "stdio.h"

RT_L1_DATA static int trace;

static void pe_entry(void *arg)
{
  printf("(%d, %d) Hello !\n", rt_cluster_id(), rt_core_id());
  volatile int i;
  for (i=0; i<rt_core_id()*100; i++);

}



static void cluster_entry()
{
  trace = gv_vcd_open_trace("/user/kernel/active");
  gv_vcd_dump_trace(trace, 1);
  rt_team_fork(0, pe_entry, NULL);
  gv_vcd_dump_trace(trace, 0);
}



int main()
{
  rt_cluster_mount(1, 0, 0, NULL);

  if (rt_cluster_call(NULL, 0, cluster_entry, NULL, NULL, 0, 0, 0, NULL)) return -1;

  rt_cluster_mount(0, 0, 0, NULL);

  return 0;
}