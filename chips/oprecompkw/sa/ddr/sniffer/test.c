/* vim: set ts=2 sw=2 expandtab: */
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
 * This example shows how a RISC-V core can use the DMA engine from within a
 * PULP cluster to access an external memory (e.g., DDRx-SDRAM, with via the
 * SoC interconnect through the DDRx-SDRAM memory controller.
 *
 * In this example the RISC-V core generates multiple read requests to the DMA
 * engine with size 4 KiB. The first request addresses the external memory
 * base address, then the address is incremented by 1MiB each time until it
 * reaches 256MiB from the base. The collected data is formatted and dumped to
 * the standard output.
 */

#include <stdio.h>

#include "rt/rt_api.h"

#include "utils.h"

/* 1024 elements of 4 bytes */
#define BUFF_SIZE 1024
static RT_L1_DATA uint32_t buff[BUFF_SIZE];

int main()
{
  int id;
  const uint64_t base   = 0x0000800000000000ULL;
  const uint64_t offset = 0x0000000000000000ULL;
  // Start spying at this point
  uint64_t address = base + offset;
  // Get 4KiB then jump ahead 1MiB to the next spying point
  const uint32_t spy_shift = (1 << 20);
  // Spy only the first 256 MiB
  const uint32_t spy_limit = (1 << 28);
  unsigned short s = (unsigned short)sizeof(buff);

  printf("Greetings from ddr sniffer!\n");
  printf("Running on cluster %d, core %d\n", rt_cluster_id(), rt_core_id());

  // All RISC-V cores of a cluster can generate requests to the DMA engine.
  // The DMA engine is connected to the cluster bus (AXI) within the PULP
  // cluster.
  // Each of the PULP cluster has its cluster bus connected to the SoC
  // interconnect bus (AXI) where a DDRx-SDRAM memory controller is connected
  // allowing access to external memory.

  do {
    // Request to DMA engine - Read 4 KiB.
    id = plp_dma_memcpy(address, (unsigned int)buff, s, PLP_DMA_EXT2LOC);
    // Wait for DMA engine to finish
    plp_dma_wait(id);
    // Dump data obtained
    dump((uint8_t *)buff, s, NULL, 0);
    // Update address to the next spying point
    address += spy_shift;
  } while ((address - base) < spy_limit);

  return 0;
}

