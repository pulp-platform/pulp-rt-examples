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

#ifndef __UTILS_H_INCLUDED__
#define __UTILS_H_INCLUDED__

#include <stdio.h>

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
#endif /* __UTILS_H_INCLUDED__ */

