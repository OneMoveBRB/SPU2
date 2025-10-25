#include "spu_launch.h"

#include <stdio.h>
#include <stdbool.h>

#include "spu_funcs.h"

#define JMP_CMP(symbol)                             \
    {[](int a, int b) -> bool {return a symbol b;}}

typedef bool (*CmpPointer)(int, int);
typedef SPU_Err_t (*Func_SPU)(SPU*);
typedef SPU_Err_t (*Func_SPU_INTptr)(SPU*, const int*);
typedef SPU_Err_t (*Func_SPU_Sizetptr)(SPU*, size_t*);
typedef SPU_Err_t (*Func_SPU_INT_CMP)(SPU*, int*, CmpPointer);

union FunctionPointer {
    Func_SPU          func_spu;
    Func_SPU_INTptr   func_spu_intptr;
    Func_SPU_Sizetptr func_spu_sizetptr;
    Func_SPU_INT_CMP  func_spu_int_cmp;
};

enum FunctionType {
    TYPE_SPU        = 1,
    TYPE_SPU_INTPTR = 2,
    TYPE_SPU_JUMP   = 3,
    TYPE_SPU_CALL   = 4,
    TYPE_SPU_RET    = 5
};

struct FunctionInfo {
    FunctionPointer func_ptr;
    CmpPointer      jmp_cmp;
    FunctionType    func_type;
};

static SPU_Err_t FunctionWrapper(SPU* processor, FunctionInfo* func);

FunctionInfo functions[] = {
    {.func_ptr           = NULL          ,    .jmp_cmp = NULL       ,    TYPE_SPU        },
    {{.func_spu          = SPU_In       },    .jmp_cmp = NULL       ,    TYPE_SPU        },
    {{.func_spu_intptr   = SPU_Push     },    .jmp_cmp = NULL       ,    TYPE_SPU_INTPTR },
    {{.func_spu          = SPU_Pop      },    .jmp_cmp = NULL       ,    TYPE_SPU        },
    {{.func_spu_intptr   = SPU_PushR    },    .jmp_cmp = NULL       ,    TYPE_SPU_INTPTR },
    {{.func_spu_intptr   = SPU_PopR     },    .jmp_cmp = NULL       ,    TYPE_SPU_INTPTR },
    {{.func_spu          = SPU_Add      },    .jmp_cmp = NULL       ,    TYPE_SPU        },
    {{.func_spu          = SPU_Sub      },    .jmp_cmp = NULL       ,    TYPE_SPU        },
    {{.func_spu          = SPU_Mul      },    .jmp_cmp = NULL       ,    TYPE_SPU        },
    {{.func_spu          = SPU_Div      },    .jmp_cmp = NULL       ,    TYPE_SPU        },
    {{.func_spu          = SPU_Sqrt     },    .jmp_cmp = NULL       ,    TYPE_SPU        },
    {{.func_spu          = SPU_Out      },    .jmp_cmp = NULL       ,    TYPE_SPU        },
    {{.func_spu          = SPU_Hlt      },    .jmp_cmp = NULL       ,    TYPE_SPU        },
    {{.func_spu_int_cmp  = SPU_JumpCheck},    .jmp_cmp = JMP_CMP(>) ,    TYPE_SPU_JUMP   },
    {{.func_spu_int_cmp  = SPU_JumpCheck},    .jmp_cmp = JMP_CMP(>=),    TYPE_SPU_JUMP   },
    {{.func_spu_int_cmp  = SPU_JumpCheck},    .jmp_cmp = JMP_CMP(<) ,    TYPE_SPU_JUMP   },
    {{.func_spu_int_cmp  = SPU_JumpCheck},    .jmp_cmp = JMP_CMP(<=),    TYPE_SPU_JUMP   },
    {{.func_spu_int_cmp  = SPU_JumpCheck},    .jmp_cmp = JMP_CMP(==),    TYPE_SPU_JUMP   },
    {{.func_spu_int_cmp  = SPU_JumpCheck},    .jmp_cmp = JMP_CMP(!=),    TYPE_SPU_JUMP   },
    {.func_ptr           = NULL          ,    .jmp_cmp = NULL       ,    TYPE_SPU_JUMP   },
    {{.func_spu_sizetptr = SPU_Call     },    .jmp_cmp = NULL       ,    TYPE_SPU_CALL   },
    {{.func_spu_sizetptr = SPU_Ret      },    .jmp_cmp = NULL       ,    TYPE_SPU_RET    }
};

SPU_Err_t SPU_Launch(SPU* processor) {
    size_t* IP   = &processor->Instruction_Pointer;
    size_t  size = processor->SPU_code.size;

    while ( *IP < size ) {
        FunctionInfo* func = &functions[processor->SPU_code.data[*IP]];

        if (FunctionWrapper(processor, func) == SPU_STOP)
            break;
    }

    return SPU_OK;
}

static SPU_Err_t FunctionWrapper(SPU* processor, FunctionInfo* func) {
    size_t* IP   = &processor->Instruction_Pointer;

    switch (func->func_type) {
        case TYPE_SPU_INTPTR: {

            int value = processor->SPU_code.data[++(*IP)];
            func->func_ptr.func_spu_intptr(processor, &value);
            ++(*IP);

            break;
            
        }
        case TYPE_SPU: {

            if (func->func_ptr.func_spu(processor) == SPU_STOP)
                return SPU_STOP;
            ++(*IP);

            break;

        }
        case TYPE_SPU_JUMP: {

            if (func->jmp_cmp == NULL) {
                *IP = (size_t)processor->SPU_code.data[*IP + 1];
            } else {
                int result = 0;
                func->func_ptr.func_spu_int_cmp(processor, &result, func->jmp_cmp);
                *IP = (result) ? (size_t)processor->SPU_code.data[*IP + 1] : *IP + 2;
            }

            break;

        }
        case TYPE_SPU_CALL: {

            size_t ret_pointer = *IP + 2;
            func->func_ptr.func_spu_sizetptr(processor, &ret_pointer);

            *IP = (size_t)processor->SPU_code.data[*IP + 1];

            break;

        }
        case TYPE_SPU_RET: {
            
            size_t ret_pointer = 0;
            func->func_ptr.func_spu_sizetptr(processor, &ret_pointer);

            *IP = ret_pointer;

            break;

        }        
        default:
            break;
    }
    
    return SPU_OK;
}
