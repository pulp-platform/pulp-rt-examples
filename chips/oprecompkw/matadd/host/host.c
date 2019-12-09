// Copyright (c) 2017 OPRECOMP Project
// Fabian Schuiki <fschuiki@iis.ee.ethz.ch>

#include <stdio.h>
#include <stdlib.h>
#include <liboprecomp.h>


int main(int argc, char **argv) {
	opc_error_t err;

	// Open the PULP binary based on what has been passed on the command line.
	if (argc != 2) {
		fprintf(stderr, "usage: %s BINARY\n", argv[0]);
		return 1;
	}

	// Load the kernel.
	opc_kernel_t knl = opc_kernel_new();
	err = opc_kernel_load_file(knl, argv[1]);
	if (err != OPC_OK) {
		opc_perror("opc_kernel_load_file", err);
		return 1;
	}

	// Open a device and offload a job.
	opc_dev_t dev = opc_dev_new();
	err = opc_dev_open_any(dev);
	if (err != OPC_OK) {
		opc_perror("opc_dev_open_any", err);
		return 1;
	}

	err = opc_dev_launch(dev, knl, (void*)0xdeadbeeffacefeed, NULL);
	if (err != OPC_OK) {
		opc_perror("opc_dev_launch", err);
		return 1;
	}
	opc_dev_wait_all(dev);

	// Clean up.
	opc_dev_free(dev);
	opc_kernel_free(knl);
	return 0;
}
