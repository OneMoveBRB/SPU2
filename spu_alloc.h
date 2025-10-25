#ifndef SPU_ALLOC_H
#define SPU_ALLOC_H

#include "spu_settings.h"

SPU_Err_t SPU_Init(SPU* processor, const char* filename);
SPU_Err_t SPU_Destroy(SPU* processor);

#endif