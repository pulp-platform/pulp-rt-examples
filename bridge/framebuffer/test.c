/* 
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

/*
 * This example shows how to dump an image to the external bridge framebuffer.
 */

#include <stdio.h>
#include "rt/rt_api.h"   
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define WIDTH  160
#define HEIGHT 120
#define BUFF_SIZE (WIDTH * HEIGHT)

static char buffer[BUFF_SIZE];

int main()
{
  // First wait until the external bridge is connected to the platform
  printf("Connecting to bridge\n");
  rt_bridge_connect(NULL);
  printf("Connection done\n");

  // Now open the bridge framebuffer so that we can flush images to it (through JTAG)
  uint64_t fb = rt_bridge_fb_open("fb", WIDTH, HEIGHT, RT_FB_FORMAT_GRAY, NULL);
  printf("Got framebuffer %x\n", fb);

  // Set all buffer to black
  memset(buffer, 0, BUFF_SIZE);

  // Now update a sliding window of the framebuffer
  int x = 0;
  int y = 0;
  while(1)
  {
    // This will always update a 16x16 rectangle which is sliding
    rt_bridge_fb_update(fb, (unsigned int)buffer, x, y, 16, 16, NULL);
    x++;
    if (x == WIDTH - 16)
    {
      x = 0;
      y += 16;
      if (y >= HEIGHT - 16)
        {
        y = 0;
      }
    }
  }

  rt_bridge_disconnect(NULL);

  return 0;
}
