#ifndef __MATADDKERNEL_H__
#define __MATADDKERNEL_H__

#include <stdint.h>
#include "MatAddBasicKernels.h"
#define _L1_Memory_SIZE 43200
#define _L2_Memory_SIZE 0
extern char *L1_Memory; /* Size given for generation: 45000 bytes, used: 43200 bytes */
extern char *L2_Memory; /* Size used for generation: 0 bytes */
extern void MatADD(
		uint64_t In1,
		uint64_t  In2,
		uint64_t  Out);
#endif
