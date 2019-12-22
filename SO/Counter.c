
#include <stdio.c>

void main(void) {
    int count;
    int pid;
    count = 0;

    {{
        ALU_XOR eax eax eax
        SCHED_PC eax
        DMA_SO 131 eax
    }};

    /*{{
        ALU_XOR eax eax eax
        ALUi_ADD eax 3072
        DMA_PUSH eax
        
        ALU_XOR ebx ebx ebx
        ALUi_ADD ebx 5
        DMA_PUSH ebx

        PRG_SYSCALL
        PRG_NOP
    }};*/

    //pid = addProc(3072);

    while (count == count) {

        if (count == 2730) {
            count = input();
        }

        output(count);
        /*{{
            ALU_XOR eax eax eax
            SCHED_PC eax
            DMA_SO 131 eax
        }};*/

        count = count + 1;
    }
}
