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
 * The executable implements a toy memory sniffer that searches for a pattern
 * defined by the host in specific memory locations that can be also, to some
 * extent, specified by the host.
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
  uint32_t spy_offset;
  uint32_t spy_shift;
  uint32_t spy_limit;
  uint8_t pattern[DDR_SNIFFER_MAX_PATTERN_SIZE];
  uint64_t output;
} st_wed;

static inline void wed_dump(st_wed *wed)
{
  printf("WED {\n");
  printf("\tspy_offset: 0x%08lx\n", wed->spy_offset);
  printf("\tspy_shift: 0x%08lx\n", wed->spy_shift);
  printf("\tspy_limit: 0x%08lx\n", wed->spy_limit);
  printf("\tpattern: %s\n", (wed->pattern[0]) ? (char *)(wed->pattern) : "empty");
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
  int id;
	st_wed wed = {
    .spy_offset = 0,
    .spy_shift = 0,
    .spy_limit = 0,
    .pattern = {0,},
    .output = 0ULL
  };
  const uint64_t base = 0x0000800000000000ULL;
  unsigned short s = (unsigned short)sizeof(buff);
  uint64_t address;
  uint64_t match = 0;

  printf("Greetings from PULP DDR sniffer!\n");
  printf("Running on cluster %d, core %d\n", rt_cluster_id(), rt_core_id());

  // Dump WED contents
  printf("Before loading\n");
  wed_dump(&wed);

	printf("wedptr: 0x%08lX_%08lX\n", (uint32_t)(wedptr >> 32), (uint32_t)(wedptr));
  // Use DMA to copy work element descriptor (WED) from host
	id = plp_dma_memcpy(htop(wedptr), (uint32_t)&wed, sizeof(wed), PLP_DMA_EXT2LOC);
	plp_dma_wait(id);
  wed.pattern[DDR_SNIFFER_MAX_PATTERN_SIZE - 1] = '\0';
  // Dump WED contents
  printf("After loading\n");
  wed_dump(&wed);

  // Start spying at this point
  address = base + wed.spy_offset;
  char *mstr = (char *)(wed.pattern);
  do {
    // Request to DMA engine - Read 4 KiB.
    id = plp_dma_memcpy(address, (uint32_t)buff, s, PLP_DMA_EXT2LOC);
    // Wait for DMA engine to finish
    plp_dma_wait(id);
    // Dump data obtained
    dump((uint8_t *)buff, s, NULL, 0);
    // Update address to the next spying point
    address += wed.spy_shift;
    // Compare pattern
    if (strncmp(mstr, (char *)(buff), bare_strlen(mstr)) == 0) {
      match = 1;
      goto found;
    }
  } while ((address - base) < wed.spy_limit);

found:
  // Inform the host about pattern match
  id = plp_dma_memcpy(htop(wed.output), (uint32_t)&match, sizeof(match), PLP_DMA_LOC2EXT);
	plp_dma_wait(id);

  return 0;
}

