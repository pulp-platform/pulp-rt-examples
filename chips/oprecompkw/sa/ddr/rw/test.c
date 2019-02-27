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
 * In this example a RISCV core writes a pattern (alternating 0xbebacafe,
 * 0xdeadbeef) into the external memory and then reads it back. For that
 * requests with size 4 KiB are made to the DMA engine.
 */


#include <stdio.h>

#include "rt/rt_api.h"

#include "utils.h"

/* 1024 elements of 4 bytes */
#define BUFF_SIZE 1024
static RT_L1_DATA uint32_t buff[BUFF_SIZE];

int main()
{
  int i;
  int id;
  uint64_t base   = 0x0000800000000000ULL;
  uint64_t offset = 0x0000000000000000ULL;
  uint64_t address = base + offset;
  unsigned short s = (unsigned short)sizeof(buff);

  printf("Greetings from ddr rw!\n");
  printf("Running on cluster %d, core %d\n", rt_cluster_id(), rt_core_id());

  for (i = 0; i < BUFF_SIZE; i++) {
    // Little endian
    buff[i] = (i % 2) ? 0xfecababe : 0xefbeadde;
  }

  // Write: PLP_DMA_LOC2EXT
  id = plp_dma_memcpy(address, (unsigned int)buff, s, PLP_DMA_LOC2EXT);

  // Destroy
  for (i = 0; i < BUFF_SIZE; i++) {
    buff[i] = 0xffffffff;
  }

  dump((uint8_t *)buff, s, NULL, 0);

  // Read back: PLP_DMA_EXT2LOC
  id = plp_dma_memcpy(address, (unsigned int)buff, s, PLP_DMA_EXT2LOC);

  plp_dma_wait(id);

  dump((uint8_t *)buff, s, NULL, 0);

  return 0;
}

