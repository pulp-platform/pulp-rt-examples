#include <stdio.h>
#include <rt/rt_api.h>
#include <stdint.h>

#define BUFFER_SIZE 256

RT_L2_DATA uint8_t tx_buffer[BUFFER_SIZE];

RT_L2_DATA uint8_t rx_buffer[BUFFER_SIZE];


int main()
{
  printf("Entered test\n");

  // As we are going to use event to do asynchronous calls, we must allocate
  // a few ones as the runtime is not allocating them
  rt_event_alloc(NULL, 4);

  rt_uart_conf_t conf;


  rt_uart_conf_init(&conf);
  
  // This will tell the driver to use the uart 0 interface (in case there are several)
  conf.itf = 0;

  rt_uart_t *uart = rt_uart_open(NULL, &conf, NULL);
  if (uart == NULL) return -1;


  for (int i=0; i<BUFFER_SIZE; i++)
  {
    tx_buffer[i] = i;
  }

  rt_event_t *event = rt_event_get_blocking(NULL);

  // We first enqueue a read copy so that we can receive what we will send.
  // This must be asynchronous as the transfer cannot finish before
  // we enqueue the write.
  // The event used here is a blocking event, which means we must explicitely wait for it
  // to know when the transfer is finished.
  rt_uart_read(uart, rx_buffer, BUFFER_SIZE, event);

  // This one is not using any event and is thus blocking.
  rt_uart_write(uart, tx_buffer, BUFFER_SIZE, NULL);

  // This will block execution until we have received the whole buffer.
  rt_event_wait(event);

  int error = 0;
  for (int i=0; i<BUFFER_SIZE; i++)
  {
    //printf("%d: @%p: %x @%p: %x\n", i, &rx_buffer[i], rx_buffer[i], &tx_buffer[i], tx_buffer[i]);
    if (rx_buffer[i] != tx_buffer[i])
    {
      printf("Error at index %d, expected 0x%x, got 0x%x\n", i, tx_buffer[i], rx_buffer[i]);
      error++;
    }
  }

  if (error)
  {
    printf("Got %d errors\n", error);
  }

  return error;
}
