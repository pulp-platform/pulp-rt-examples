/* 
 * Copyright (C) 2017 ETH Zurich, University of Bologna and GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "rt/rt_api.h"
#include "stdio.h"

static RT_L1_DATA rt_task_t fc_tasks[2];

static void cl_task_entry(rt_task_t *task, int id)
{
  printf("Entered CL task %p / %ld,%ld with ID %d\n", task, task->args[0], task->args[1], id);
}

static void fc_task_entry(rt_task_t *_task, int id)
{
  rt_task_t cl_tasks[2];

  printf("Entered FC task %p / %ld with ID %d, triggering 2 other tasks\n", _task, _task->args[0], id);

  for (int i=0; i<2; i++)
  {
    rt_task_t *task = &cl_tasks[i];
    rt_task_init(task, cl_task_entry);
    rt_task_cores(task, 2);
    task->args[0] = _task->args[0];
    task->args[1] = i;
    rt_task_cl_push(task);
  }

  for (int i=0; i<2; i++)
  {
    rt_task_t *task = &cl_tasks[i];
    rt_task_cl_wait(task);
  }
}

int main()
{
  rt_task_conf_t conf;
  rt_task_cluster_t cluster;

  printf("Starting test\n");

  if (rt_event_alloc(NULL, 2)) return -1;

  rt_cluster_mount(1, 0, 0, NULL);

  rt_task_conf_init(&conf);

  if (rt_task_cluster_init(&cluster, &conf, NULL))
    return -1;

  rt_event_t *events[2];

  for (int i=0; i<2; i++)
  {
    rt_task_t *task = &fc_tasks[i];
    events[i] = rt_event_get_blocking(NULL);
    rt_task_init(task, fc_task_entry);
    task->args[0] = i;
    rt_task_fc_push(&cluster, task, events[i]);
  }

  for (int i=0; i<2; i++)
  {
    rt_event_wait(events[i]);
  }

  return 0;
}
