#include <stdio.h>
#include <rt/rt_api.h>
#include <stdint.h>

#define BUFFER_SIZE 256

RT_L2_DATA uint8_t tx_buffer[BUFFER_SIZE];


int main()
{
  printf("Entering test\n");

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

  // No event is specified so we are doing a blocking call, which means
  // we are blocked on this call until the whole buffer is sent
  rt_uart_write(uart, tx_buffer, BUFFER_SIZE, NULL);

  rt_uart_close(uart, NULL);

  return 0;
}
