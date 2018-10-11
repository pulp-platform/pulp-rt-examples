/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna and
 * GreenWaves Technologies
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

#include "rt/rt_api.h"

#define ELEM_SIZE 2
#define NB_ELEM 128
#define BUFF_SIZE (NB_ELEM*ELEM_SIZE)

static uint8_t buff[BUFF_SIZE];

int main()
{
  printf("Entering main controller\n");

  if (rt_event_alloc(NULL, 1)) return -1;

  rt_i2s_conf_t i2s_conf;
  rt_i2s_conf_init(&i2s_conf);

  i2s_conf.frequency = SAMPLING_FREQ;
  i2s_conf.dual = 0;
  i2s_conf.width = 16;
  i2s_conf.id = 0;

  rt_i2s_t *i2s = rt_i2s_open(NULL, &i2s_conf, NULL);
  if (i2s == NULL) return -1;

  rt_event_t *event = rt_event_get_blocking(NULL);

  rt_i2s_capture(i2s, buff, BUFF_SIZE, event);

  rt_i2s_start(i2s);

  rt_event_wait(event);

  rt_i2s_stop(i2s);

  rt_i2s_close(i2s, NULL);

  for (int i=0; i<NB_ELEM; i++)
  {
    printf("%d\n", ((int16_t *)buff)[i]);    
  }

  return 0;
}
