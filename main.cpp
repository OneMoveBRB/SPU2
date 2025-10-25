#include <stdio.h>

#include "spu_settings.h"
#include "spu_alloc.h"
#include "spu_launch.h"


int main() {
    const char* bytecode = "../asm/bytecode.bin";

    SPU processor;
    SPU_Init(&processor, bytecode);

    SPU_Launch(&processor);

    SPU_Destroy(&processor);
    return 0;
}
