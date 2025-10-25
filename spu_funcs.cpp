#include "spu_funcs.h"

#include <stdio.h>
#include <math.h>
#include <assert.h>

SPU_Err_t SPU_In(SPU* processor) {
    assert( processor != NULL );

    return SPU_START;
}

SPU_Err_t SPU_Push(SPU* processor, const int* value) {
    assert( processor != NULL );

    StackPush(processor->SPU_stack, value);

    return SPU_OK;
}

SPU_Err_t SPU_Pop(SPU* processor) {
    assert( processor != NULL );

    StackPop(processor->SPU_stack, NULL);

    return SPU_OK;
}

SPU_Err_t SPU_PushR(SPU* processor, const int* value) {
    assert( processor != NULL );
    assert( value != NULL );

    if (processor->regs[*value] == UNINITIALIZED) {
        fprintf(stderr, "You're trying to push undefined register\nRegister[%d] was not found at line:%zu\n", 
                        *value, processor->Instruction_Pointer);
        return SPU_UNDEFINED_REGISTER;
    }

    StackPush(processor->SPU_stack, processor->regs + *value);

    return SPU_OK;
}

SPU_Err_t SPU_PopR(SPU* processor, const int* value) {
    assert( processor != NULL );
    assert( value != NULL );

    if ( StackPop(processor->SPU_stack, processor->regs + *value) == STACK_UNDERFLOW ) {
        fprintf(stderr, "Stack is empty. Register was not taken from the stack at line:%zu\n",
                        processor->Instruction_Pointer);
        return SPU_NO_REGISTER_IN_STACK;
    }

    return SPU_OK;
}

SPU_Err_t SPU_Add(SPU* processor) {
    assert( processor != NULL );

    int prev_element = 0, last_element = 0;
    
    if ( StackPop(processor->SPU_stack, &last_element) == STACK_UNDERFLOW ) {
        fprintf(stderr, "Stack is empty. Add failed at line:%zu\n",
                        processor->Instruction_Pointer);
        return SPU_NO_ELEMENT_IN_STACK;
    }
    
    if ( StackPop(processor->SPU_stack, &prev_element) == STACK_UNDERFLOW ) {
        fprintf(stderr, "Stack is empty. Add failed at line:%zu\n",
                        processor->Instruction_Pointer);
        return SPU_NO_ELEMENT_IN_STACK;
    }

    int sum = prev_element + last_element;
    StackPush(processor->SPU_stack, &sum);

    return SPU_OK;
}

SPU_Err_t SPU_Sub(SPU* processor) {
    assert( processor != NULL );

    int prev_element = 0, last_element = 0;
    
    if ( StackPop(processor->SPU_stack, &last_element) == STACK_UNDERFLOW ) {
        fprintf(stderr, "Stack is empty. Add failed at line:%zu\n",
                        processor->Instruction_Pointer);
        return SPU_NO_ELEMENT_IN_STACK;
    }
    
    if ( StackPop(processor->SPU_stack, &prev_element) == STACK_UNDERFLOW ) {
        fprintf(stderr, "Stack is empty. Add failed at line:%zu\n",
                        processor->Instruction_Pointer);
        return SPU_NO_ELEMENT_IN_STACK;
    }

    int sub = prev_element - last_element;
    StackPush(processor->SPU_stack, &sub);

    return SPU_OK;
}

SPU_Err_t SPU_Mul(SPU* processor) {
    assert( processor != NULL );

    int prev_element = 0, last_element = 0;
    
    if ( StackPop(processor->SPU_stack, &last_element) == STACK_UNDERFLOW ) {
        fprintf(stderr, "Stack is empty. Add failed at line:%zu\n",
                        processor->Instruction_Pointer);
        return SPU_NO_ELEMENT_IN_STACK;
    }
    
    if ( StackPop(processor->SPU_stack, &prev_element) == STACK_UNDERFLOW ) {
        fprintf(stderr, "Stack is empty. Add failed at line:%zu\n",
                        processor->Instruction_Pointer);
        return SPU_NO_ELEMENT_IN_STACK;
    }

    int mul = prev_element * last_element;
    StackPush(processor->SPU_stack, &mul);

    return SPU_OK;
}

SPU_Err_t SPU_Div(SPU* processor) {
    assert( processor != NULL );

    int prev_element = 0, last_element = 0;
    
    if ( StackPop(processor->SPU_stack, &last_element) == STACK_UNDERFLOW ) {
        fprintf(stderr, "Stack is empty. Add failed at line:%zu\n",
                        processor->Instruction_Pointer);
        return SPU_NO_ELEMENT_IN_STACK;
    }
    
    if ( StackPop(processor->SPU_stack, &prev_element) == STACK_UNDERFLOW ) {
        fprintf(stderr, "Stack is empty. Add failed at line:%zu\n",
                        processor->Instruction_Pointer);
        return SPU_NO_ELEMENT_IN_STACK;
    }

    // double double_div = prev_element * 1.0 / last_element;
    // int int_div = (int)(double_div * 100);
    int int_div = prev_element / last_element;
    StackPush(processor->SPU_stack, &int_div);

    return SPU_OK;
}

SPU_Err_t SPU_Sqrt(SPU* processor) {
    assert( processor != NULL );

    int last_element = 0;
    
    if ( StackPop(processor->SPU_stack, &last_element) == STACK_UNDERFLOW ) {
        fprintf(stderr, "Stack is empty. Add failed at line:%zu\n",
                        processor->Instruction_Pointer);
        return SPU_NO_ELEMENT_IN_STACK;
    }

    double double_div = sqrt((double)last_element);
    // int int_div = (int)(double_div * 100);
    int int_div = (int)double_div;
    StackPush(processor->SPU_stack, &int_div);

    return SPU_OK;
}

SPU_Err_t SPU_Out(SPU* processor) {
    assert( processor != NULL );

    static int out_counter = 1;

    FILE* fp = fopen("SPU_Code_Out.txt", "a");
    if (fp == NULL) {
        fprintf(stderr, "Error opening SPU_Code_Out.txt: fp = NULL\n");
        return SPU_IO_FAILED;
    }

    fprintf(fp, "############# OUT %d #############\n", out_counter++);
    fprintf(fp, "SPU_Code capacity: %zu\n", processor->SPU_stack->meta.capacity);
    fprintf(fp, "SPU_Code size: %zu\nSPU_Code data: ", processor->SPU_stack->meta.size);
    for (size_t i = 0; i < processor->SPU_stack->meta.size; i++) {
        fprintf(fp, "%d ", *(int*)((char*)processor->SPU_stack->data + i*sizeof(4)));
    } fprintf(fp, "\n\n");

    fclose(fp);

    return SPU_OK;
}

SPU_Err_t SPU_Hlt(SPU* processor) {
    assert( processor != NULL );

    return SPU_STOP;
}

SPU_Err_t SPU_JumpCheck(SPU* processor, int* result, bool (*cmp)(int, int)) {
    assert( processor != NULL );
    assert( cmp != NULL );

    int a = 0, b = 0;
    StackPop(processor->SPU_stack, &a);
    StackPop(processor->SPU_stack, &b);

    *result = cmp(a, b) ? 1 : 0;

    return SPU_OK;
}

SPU_Err_t SPU_Call(SPU* processor, size_t* value) {
    assert( processor != NULL );
    assert( value != NULL );

    StackPush(processor->SPU_retStack, value);

    return SPU_OK;
}

SPU_Err_t SPU_Ret(SPU* processor, size_t* result) {
    assert( processor != NULL );

    StackPop(processor->SPU_retStack, result);

    return SPU_OK;
}
