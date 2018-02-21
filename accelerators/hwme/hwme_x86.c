/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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
 * Authors:  Francesco Conti <fconti@iis.ee.ethz.ch>
 */

#include <stdio.h>
#include <stdint.h>

#define USE_STIMULI
// #define DEBUG

#include "hwme_stimuli_a.h"
#include "hwme_stimuli_b.h"
#include "hwme_stimuli_c.h"
#include "hwme_stimuli_d.h"

int32_t a[128];
int32_t b[128];
int32_t c[128];
int32_t d[128];

int main() {

  volatile int errors = 0;
  int gold_sum = 0, check_sum = 0;
  int i,j;

#ifdef USE_STIMULI
  for(int i=0; i<512; i++) {
    ((uint8_t *) a)[i] = stim_a[i];
  }
  for(int i=0; i<512; i++) {
    ((uint8_t *) b)[i] = stim_b[i];
  }
  for(int i=0; i<512; i++) {
    ((uint8_t *) c)[i] = stim_c[i];
  }
  for(int i=0; i<512; i++) {
    ((uint8_t *) d)[i] = stim_d[i];
  }
#else
  for(int i=0; i<128; i++) {
    a[i] = i;
  }
  for(int i=0; i<128; i++) {
    b[i] = i;
  }
  for(int i=0; i<128; i++) {
    c[i] = i;
  }
  for(int i=0; i<128; i++) {
    d[i] = i;
  }
#endif

  for(int j=0; j<4; j++) {
    int64_t tmp = 0;
    tmp = c[j];
#ifdef DEBUG
    printf("-> %016x\n", c[j]);
#endif
    for(int i=0; i<32; i++) {
#ifdef DEBUG
      printf("%08x * %08x = %0llx -> %0llx\n", a[j*32+i], b[j*32+i], ((int64_t) a[j*32+i]) * ((int64_t) b[j*32+i]), tmp + ((int64_t) a[j*32+i]) * ((int64_t) b[j*32+i]));
#endif
      tmp += ((int64_t) a[j*32+i]) * ((int64_t) b[j*32+i]);
    }
    d[j] = 0x00000000ffffffff & tmp;
    printf("%d: %08x\n", j, d[j]);
  }

  return 0;
}
