#ifndef SPU_FUNCS_H
#define SPU_FUNCS_H

#include "spu_settings.h"

SPU_Err_t SPU_In(SPU* processor);
SPU_Err_t SPU_Push(SPU* processor, const int* value);
SPU_Err_t SPU_Pop(SPU* processor);
SPU_Err_t SPU_PushR(SPU* processor, const int* value);
SPU_Err_t SPU_PopR(SPU* processor, const int* value);
SPU_Err_t SPU_Add(SPU* processor);
SPU_Err_t SPU_Sub(SPU* processor);
SPU_Err_t SPU_Mul(SPU* processor);
SPU_Err_t SPU_Div(SPU* processor);
SPU_Err_t SPU_Sqrt(SPU* processor);
SPU_Err_t SPU_Out(SPU* processor);
SPU_Err_t SPU_Hlt(SPU* processor);
SPU_Err_t SPU_JumpCheck(SPU* processor, int* result, bool (*cmp)(int, int));
SPU_Err_t SPU_Call(SPU* processor, size_t* value);
SPU_Err_t SPU_Ret(SPU* processor, size_t* result);

#endif