/*
 * This example shows how to use SPIM driver with asynchronous
 * events.
 */

#include <stdio.h>
#include <rt/rt_api.h>
#include <stdint.h>

#define BUFFER_SIZE 32
#define NB_TRANSFER 16

static int nb_enqueued;
static int nb_done;
static rt_spim_t *spim;


// This callback is called everytime a transfer is finished
// Just reenqueued another transfer in case we didn't reach
// the number of transfers we want to enqueue
static void end_of_transfer(void *arg)
{
  if (nb_enqueued < NB_TRANSFER)
  {
    nb_enqueued++;
    rt_spim_send(
      spim, arg, BUFFER_SIZE*8, RT_SPIM_CS_AUTO,
      rt_event_get(NULL, end_of_transfer, arg)
    );
  }

  nb_done++;
}

int main()
{
  // As we'll use asnchronous events, we first need to allocate
  // them. We need as one event per pending transfer, so 2
  // in this case as we'll never have more that 2 transfers
  // enqueued at the same time.
  rt_event_alloc(NULL, 2);

  // First configure the SPI device
  rt_spim_conf_t conf;
  // Get default configuration
  rt_spim_conf_init(&conf);
  // Set maximum baudrate. Can actually be lower than
  // that depending on the best divider found
  conf.max_baudrate = 1000000;
  // SPI interface identifier as the Pulp chip can have
  // several interfaces
  conf.id = 0; 
  // Chip select
  conf.cs = 1;

  // Then open the device
  spim = rt_spim_open(NULL, &conf, NULL);
  if (spim == NULL) return -1;

  // Allocate the buffers
  // We will always try to have 2 buffers ready to get the best
  // out of the interface, this way the DMA can proceed with the
  // next one whie we enqueue a new one when one is finished.
  char *tx_buffer0 = rt_alloc(RT_ALLOC_PERIPH, BUFFER_SIZE);
  if (tx_buffer0 == NULL) return -1;
  char *tx_buffer1 = rt_alloc(RT_ALLOC_PERIPH, BUFFER_SIZE);
  if (tx_buffer1 == NULL) return -1;

  for (int i=0; i<BUFFER_SIZE; i++)
  {
    tx_buffer0[i] = i;
    tx_buffer1[i] = i;
  }

  // Now send the buffers.
  // Enqueue 2 at first and the rest will be enqueued from the
  // callback everytime one transfer is finished
  nb_enqueued = 2;
  nb_done = 0;
  rt_spim_send(
    spim, tx_buffer0, BUFFER_SIZE*8, RT_SPIM_CS_AUTO,
    rt_event_get(NULL, end_of_transfer, tx_buffer0)
  );
  rt_spim_send(
    spim, tx_buffer1, BUFFER_SIZE*8, RT_SPIM_CS_AUTO,
    rt_event_get(NULL, end_of_transfer, tx_buffer1)
  );

  while (nb_done != NB_TRANSFER)
  {
    rt_event_execute(NULL, 1);
  }

  rt_spim_close(spim, NULL);

  return 0;
}
