#include "spu_alloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


SPU_Err_t SPU_Init(SPU* processor, const char* filename) {
    assert( processor != NULL );
    assert( filename != NULL );

    StackInit(&processor->SPU_stack, sizeof(int), FIRST_SIZE, "SPU_stack");
    StackInit(&processor->SPU_retStack, sizeof(size_t), FIRST_SIZE, "SPU_retStack");

    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Opening %s FAILED\n", filename);
        return SPU_IO_FAILED;
    }

    if (fread(&processor->Instruction_Pointer, sizeof(size_t), 1, fp) != 1) {
        fprintf(stderr, "Reading StartIP in %s FAILED\n", filename);
        return SPU_IO_FAILED;
    }

    size_t capacity = 0;
    if (fread(&capacity, sizeof(size_t), 1, fp) != 1) {
        fprintf(stderr, "Reading Buffer Size in %s FAILED\n", filename);
        return SPU_IO_FAILED;
    }

    BufferInit(&processor->SPU_code, capacity);

    size_t num_of_read_elements = fread(processor->SPU_code.data, sizeof(int), 
                                        processor->SPU_code.capacity, fp);
    if (num_of_read_elements != processor->SPU_code.capacity) {
        fprintf(stderr, "FAILED: Read %zu elements out of %zu in Buffer in %s\n", 
                        num_of_read_elements, processor->SPU_code.capacity, filename);
        return SPU_IO_FAILED;
    }

    processor->SPU_code.size = num_of_read_elements;

    for (int i = 0; i < MAX_REGS; i++) {
        processor->regs[i] = UNINITIALIZED;
    }

    return SPU_OK;
}

SPU_Err_t SPU_Destroy(SPU* processor) {
    assert( processor != NULL );

    StackDestroy(processor->SPU_stack);
    StackDestroy(processor->SPU_retStack);

    BufferDestroy(&processor->SPU_code);

    return SPU_OK;
}