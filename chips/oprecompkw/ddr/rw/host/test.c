/* vim: set ts=2 sw=2 et: */
/*
 * Copyright (C) 2019 TU Kaiserslautern
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

/*
 * Author: Éder F. Zulian, TUK (zulian@eit.uni-kl.de)
 */

/*
 * In this example the host processor allocates an accelerator and offloads a
 * task to it. The task computation is performed by an executable binary which
 * is sent by the host to the bootloader running in the PULP.
 * The executable writes a pattern into the external memory and reads it back
 * using the DMA engine for that.
 *
 * This file contains the implementation of the host side.
 */

#include <stdio.h>
#include <stdlib.h>
#include <liboprecomp.h>

#include "utils.h"

#define _OFFSET    0x00000000

typedef struct {
  uint32_t _offset;
  volatile uint64_t *output;
} st_wed;

static inline void wed_dump(st_wed *wed)
{
  printf("WED {\n");
  printf("\t_offset: 0x%08x\n", wed->_offset);
  printf("\toutput: %p\n", wed->output);
  printf("}\n");
}

int main(int argc, char **argv)
{
  opc_error_t err;

  printf("\nGreetings from host!\n");

  // Open the PULP binary based on what has been passed on the command line.
  if (argc != 2) {
    fprintf(stderr, "usage: %s BINARY\n", argv[0]);
    return 1;
  }

  // Create new kernel - allocate memory for data structure that represents a
  // new computational kernel struct opc_kernel.
  opc_kernel_t knl = opc_kernel_new();

  // Load the kernel - fill up the just allocated data structure with ELF and
  // related info.
  err = opc_kernel_load_file(knl, argv[1]);
  if (err != OPC_OK) {
    opc_perror("opc_kernel_load_file", err);
    return 1;
  }

  st_wed wed = {
    ._offset = _OFFSET,
  };

  wed.output = calloc(1, sizeof(*(wed.output)));

  wed_dump(&wed);

  // Dump kernel info to stdout
  printf("Host dumping the kernel:\n");
  opc_kernel_dump_info(knl, stdout);

  // Create a new device - allocate a new struct opc_dev.
  opc_dev_t dev = opc_dev_new();
  // Open any accelerator available - fill up afu (Accelerator Function Unit)
  // pointer in struct opc_dev.
  err = opc_dev_open_any(dev);
  if (err != OPC_OK) {
    opc_perror("opc_dev_open_any", err);
    return 1;
  }

  // Allocate new WED (Work Element Descriptor) struct wed
  // Fill struct wed with binary sections to be passed to accelerator and work
  // element descriptor.
  // Call cxl_afu_attach() to trigger the accelerator
  // Currently it waits internally until job completion
  err = opc_dev_launch(dev, knl, (void*)&wed, NULL);
  if (err != OPC_OK) {
    opc_perror("opc_dev_launch", err);
    return 1;
  }

  // Currently does nothing since opc_dev_launch() blocks until job completion
  opc_dev_wait_all(dev);

  printf("(%s:%d) - %s: output: 0x%08lx\n", __FILE__, __LINE__, __func__, (uint64_t)(*wed.output));
  free((void*)wed.output);

  // Clean up.
  // Free resources, destroy mutexes, etc.
  opc_dev_free(dev);
  opc_kernel_free(knl);

  return 0;
}

