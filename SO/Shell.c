
#include <stdio.c>

void main(void) {
    int count;
    count = 0;

    {{
        ALU_XOR eax eax eax
        SCHED_PC eax
        DMA_SO 131 eax
    }};

    input();

    while (count == count) {
        output(count);
        /*{{
            ALU_XOR eax eax eax
            SCHED_PC eax
            DMA_SO 131 eax
        }};*/

        count = count + 1;
    }
}
