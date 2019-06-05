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

#define WIDTH    324
#define HEIGHT    244

#define BUFF_SIZE (WIDTH*HEIGHT)

static rt_camera_t *camera;

RT_L2_DATA unsigned char *buff[1];

int main()
{
  printf("Entering main controller\n");

  if (rt_event_alloc(NULL, 8)) return -1;

  rt_cam_conf_t cam_conf;

  rt_camera_conf_init(&cam_conf);

  cam_conf.id = 0;
  cam_conf.type = RT_CAM_TYPE_HIMAX;
  cam_conf.control_id = 1;

  buff[0] = rt_alloc(RT_ALLOC_PERIPH, WIDTH*HEIGHT);
  if (buff[0] == NULL) goto error;

  camera = rt_camera_open(NULL, &cam_conf, 0);
  if (camera == NULL) return -1;

  rt_cam_control(camera, CMD_INIT, 0);


  for (int i=0; i<10; i++)
  {
    rt_event_t *event1 = rt_event_get_blocking(NULL);

    rt_camera_capture (camera, buff[0], WIDTH*HEIGHT, event1);
    
    rt_cam_control(camera, CMD_START, 0);

    rt_event_wait(event1);
    rt_cam_control(camera, CMD_STOP, 0);
  }

  rt_camera_close(camera, 0);
  printf("Test success\n");


  return 0;

error:
  printf("Test failure\n");
  return -1;
}
