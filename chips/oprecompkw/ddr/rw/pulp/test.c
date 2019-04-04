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
 * This file contains the implementation of the PULP side.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "rt/rt_api.h"

#include "utils.h"

/* 1024 elements of 4 bytes */
#define BUFF_SIZE 1024
static RT_L1_DATA uint32_t buff[BUFF_SIZE];

typedef struct {
  uint32_t _offset;
  uint64_t output;
} st_wed;

static inline void wed_dump(st_wed *wed)
{
  printf("WED {\n");
  printf("\t_offset: 0x%08lx\n", wed->_offset);
  printf("\toutput: 0x%08lx_%08lx\n", (uint32_t)(wed->output >> 32), (uint32_t)(wed->output));
  printf("}\n");
}

// All RISC-V cores of a cluster can generate requests to the DMA engine.
// The DMA engine is connected to the cluster bus (AXI) within the PULP
// cluster.
// Each of the PULP cluster has its cluster bus connected to the SoC
// interconnect bus (AXI) where a DDRx-SDRAM memory controller is connected
// allowing access to external memory.

int main(uint64_t wedptr)
{
  int i;
  int id;
  st_wed wed = {
    ._offset = 0,
    .output = 0ULL
  };
  const uint64_t base = 0x0000800000000000ULL;
  unsigned short s = (unsigned short)sizeof(buff);
  uint64_t address;
  uint64_t output = 0xbabeface;

  printf("Greetings from PULP DDR R/W!\n");
  printf("Running on cluster %d, core %d\n", rt_cluster_id(), rt_core_id());

  // Initialize buffer with pattern alternating 0xbebacafe and 0xdeadbeef
  for (i = 0; i < BUFF_SIZE; i++) {
    // Little endian
    buff[i] = (i % 2) ? 0xfecababe : 0xefbeadde;
  }

  // Dump WED contents
  printf("Before loading\n");
  wed_dump(&wed);

  printf("wedptr: 0x%08lX_%08lX\n", (uint32_t)(wedptr >> 32), (uint32_t)(wedptr));
  // Use DMA to copy work element descriptor (WED) from host
  id = plp_dma_memcpy(htop(wedptr), (uint32_t)&wed, sizeof(wed), PLP_DMA_EXT2LOC);
  plp_dma_wait(id);

  // Dump WED contents
  printf("After loading\n");
  wed_dump(&wed);

  // Write and read at this point
  address = base + wed._offset;

  // Write into external memory: PLP_DMA_LOC2EXT
  id = plp_dma_memcpy(address, (uint32_t)buff, s, PLP_DMA_LOC2EXT);
  plp_dma_wait(id);

  // Overwrite local buffer before reading back from external memory
  for (i = 0; i < BUFF_SIZE; i++) {
    buff[i] = 0xffffffff;
  }

  // Dump buffer contents
  dump((uint8_t *)buff, s, NULL, 0);

  // Read back: PLP_DMA_EXT2LOC
  id = plp_dma_memcpy(address, (unsigned int)buff, s, PLP_DMA_EXT2LOC);
  plp_dma_wait(id);

  // Dump buffer contents
  dump((uint8_t *)buff, s, NULL, 0);

  id = plp_dma_memcpy(htop(wed.output), (uint32_t)&output, sizeof(output), PLP_DMA_LOC2EXT);
  plp_dma_wait(id);

  return 0;
}

