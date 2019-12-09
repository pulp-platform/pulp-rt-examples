/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include "pmsis.h"
#include "stdio.h"
#include "MatAddBasicKernels.h"
#include "MatAddKernels.h"
//#include "test.h"
//#include "bsp/fs.h"
//#include "bsp/flash/hyperflash.h"

#define MAT_W      ( 100 )
#define MAT_H      ( 100 )
#define MAT_SIZE   ( MAT_W * MAT_H )

#define NB_ITER 2000

//#define USE_TLM_MEM 1

int main(uint64_t wed) {

  printf("Entering main\n");

  L1_Memory = rt_alloc(RT_ALLOC_CL_DATA, _L1_Memory_SIZE);

#ifdef USE_TLM_MEM
  uint64_t Int1 = 0x800000000000;
  uint64_t Int2 = 0x800000100000;
  uint64_t Out = 0x800000200000;
#else
  uint64_t Int1 = rt_alloc(RT_ALLOC_L2_CL_DATA, MAT_SIZE);
  uint64_t Int2 = rt_alloc(RT_ALLOC_L2_CL_DATA, MAT_SIZE);
  uint64_t Out = rt_alloc(RT_ALLOC_L2_CL_DATA, MAT_SIZE);

  if (!L1_Memory || !Int1 || !Int2 || !Out)
    return -1;

#endif

  for (int i=0; i<NB_ITER; i++)
  {
    MatADD(Int1, Int2, Out);
  }

  return 0;
}
