PULP_APP = test
PULP_APP_FC_SRCS = test.c
PULP_CFLAGS = -O3 -g

# This will activate the DPI model called "uart_tb" and connect it
# to the chip interface called "uart0" so that we can see the bitstream
# dumped to a file (tx_uart.log in the build folder)
override PULP_TEMPLATE_ARGS += uart_tb(uart0)

include $(PULP_SDK_HOME)/install/rules/pulp_rt.mk
