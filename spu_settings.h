#ifndef SPU_SETTINGS_H
#define SPU_SETTINGS_H

#include "../stack/stack.h"
#include "../buffer/buffer.h"

#define MAX_REGS 4
#define UNINITIALIZED 0xDEAD

struct SPU {
    Stack_t* SPU_stack;
    Stack_t* SPU_retStack;
    Buffer_t SPU_code;
    size_t Instruction_Pointer;
    int regs[MAX_REGS];
};

enum SPU_Err_t {
    SPU_OK,
    SPU_IO_FAILED,
    SPU_UNDEFINED_REGISTER,
    SPU_NO_REGISTER_IN_STACK,
    SPU_NO_ELEMENT_IN_STACK,
    SPU_START,
    SPU_STOP
};

#endif
