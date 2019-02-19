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

#include <stdio.h>

#include "rt/rt_api.h"

/* 1024 elements of 4 bytes */
#define BUFF_SIZE 1024
static RT_L1_DATA uint32_t buff[BUFF_SIZE];

#define DEBUG_BUFF_DUMP
#ifdef DEBUG_BUFF_DUMP
static inline void dump(uint8_t *data, size_t data_len, char *msg, uint8_t byte_split)
{
  size_t i;

  if (msg != NULL) {
    printf("%s\n", msg);
  }

  for (i = 0; i < data_len; i++) {
    if (i % 32 == 0) {
      if (i != 0) {
        puts("");
      }
      printf("%04x-%04x: ", i, i + 32);
    }
    if (byte_split) {
      printf("%02x ", data[i]);
    } else {
      printf("%02x", data[i]);
      if ((i + 1) % 4 == 0) {
        printf(" ");
      }
    }
  }
  puts("");
}
#else
static inline void dump(uint8_t *data, size_t data_len, char *msg, uint8_t byte_split)
{
}
#endif /* DEBUG_BUFF_DUMP */

int main()
{
  int i;
  int id;
  uint64_t base   = 0x0000800000000000ULL;
  uint64_t offset = 0x0000000000000000ULL;
  uint64_t address = base + offset;
  unsigned short s = (unsigned short)sizeof(buff);

  printf("Greetings from ddr_dma!\n");

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

