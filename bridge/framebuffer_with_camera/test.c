/* 
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

/*
 * This example shows how to capture an image from the camera and flush it
 * to the external bridge framebuffer.
 */


#include "rt/rt_api.h"
#include "rt/data/rt_data_camera.h"

// This strange resolution comes from the himax camera
#define WIDTH     324
#define HEIGHT    244

#ifdef USE_RAW
#define FB_FORMAT RT_FB_FORMAT_RAW
#else
#define FB_FORMAT RT_FB_FORMAT_GRAY
#endif

// The following structure is used to describe an image
typedef struct buffer_s buffer_t;

typedef struct buffer_s {
  void *image;
  buffer_t *next;
} buffer_t;

// First available image buffer
static buffer_t *first_buffer = NULL;

// 0 if the no copy to the framebuffer is pending which means we can enqueue one
static int fb_pending = 0;

// Number of frame dropped since the last frame was flushed to the framebuffer
static int dropped = 0;

// Camera descriptor
static rt_camera_t *camera;

// Framebuffer descriptor
static uint64_t fb;



static void end_of_frame(void *arg);


// This can be called to enqueue a capture to the camera.
// This will allocate an image buffer and use it for the capture
static void enqueue_capture()
{
  buffer_t *buffer = first_buffer;
  first_buffer = buffer->next;

  rt_camera_capture (camera, buffer->image, WIDTH*HEIGHT, rt_event_get(NULL, end_of_frame, (void *)buffer));
}



// This is called when an image has been fully flushed to the framebuffer.
// This just makes the image buffer available for a new capture and make
// the framebuffer flush available.
static void end_of_fb_update(void *arg)
{
  buffer_t *buffer = (buffer_t *)arg;
  buffer->next = first_buffer;
  first_buffer = buffer;
  fb_pending = 0;
}



// This is called when an image capture is finished.
static void end_of_frame(void *arg)
{
  buffer_t *buffer = (void *)arg;

  // If the no copy to the framebuffer is pending, we can enqueue one
  // otherwise just drop the frame.
  if (!fb_pending)
  {
    enqueue_capture();
    fb_pending = 1;
    rt_bridge_fb_update(fb, (unsigned int)buffer->image, 0, 0, WIDTH, HEIGHT, rt_event_get(NULL, end_of_fb_update, (void *)buffer));
    printf("Enqueued frame to framebuffer, dropped %d frames before\n", dropped);
    dropped = 0;
  }
  else
  {
    buffer->next = first_buffer;
    first_buffer = buffer;
    dropped++;
    enqueue_capture();
  }
}



int main()
{
  printf("Entering main controller\n");

  // First wait until the external bridge is connected to the platform
  printf("Connecting to bridge\n");
  rt_bridge_connect(1, NULL);
  printf("Connection done\n");

  // Allocate 3 buffers, there will be 2 for the camera double-buffering and one
  // for flushing an image to the framebuffer.
  for (int i=0; i<3; i++)
  {
    // Buffer structure is manipulated by the FC
    buffer_t *buffer = rt_alloc(RT_ALLOC_FC_DATA, sizeof(buffer_t));
    if (buffer == NULL) return -1;

    // While buffer image is manipulated by the periph DMA
    buffer->image = rt_alloc(RT_ALLOC_PERIPH, WIDTH*HEIGHT);
    if (buffer->image == NULL) return -1;

    // Enqueue each buffer to the free list, they will be dynamically assigned
    // depending on the needs
    buffer->next = first_buffer;
    first_buffer = buffer;
  }

  // Now open the bridge framebuffer so that we can flush images to it (through JTAG)
  fb = rt_bridge_fb_open("Camera", WIDTH, HEIGHT, FB_FORMAT, NULL);
  if (fb == 0) return -1;

  // We'll need one event per buffer
  if (rt_event_alloc(NULL, 3)) return -1;

  // Configure Himax camera on interface 0
  rt_cam_conf_t cam_conf;
  rt_camera_conf_init(&cam_conf);
  cam_conf.id = 0;
  cam_conf.control_id = 1;
  cam_conf.type = RT_CAM_TYPE_HIMAX;

  // Open the camera
  camera = rt_camera_open(NULL, &cam_conf, 0);
  if (camera == NULL) return -1;

  rt_cam_control(camera, CMD_INIT, 0);
  //rt_time_wait_us(1000000); //Wait camera calibration

  // Start it
  rt_cam_control(camera, CMD_START, 0);

  // Enqueue 2 captures as we'll need 1 buffer filled and one ready to not lose
  // any data
  enqueue_capture();
  enqueue_capture();

  // The main loop is not doing anything, everything will be done through event callbacks
  while(1)
  {
    rt_event_execute(NULL, 1);
  }

  rt_camera_close(camera, 0);

  return 0;
}
