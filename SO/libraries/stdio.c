int in() {
    int value;

    {{
        ALU_XOR eax eax eax
        DMA_GIA eax
        ALU_XOR ebx ebx ebx
        ALU_SET ecx ebx eax
        PRG_JMPCi ecx in
        DMA_RI edx 129
        DMA_STOREi edx &value
    }};

    return value;
}

void out(int value) {

    {{
        DMA_LOADi eax &value
        DMA_SO 131 eax
        DMA_STOREi eax &value
    }};

}

int input() {
    int flag;
    int value;
    int cond;
    int aaa;

    // Tries to obtain an input from OS, if flag == 1 it means there was an input available and it was retrieved.
    flag = 0;
    value = 0;
    cond = 1;

    while(cond == cond) {

        {{
            
            ALU_XOR ebx ebx ebx
            ALUi_ADD ebx 2
            DMA_PUSH ebx

            PRG_SYSCALL
            PRG_NOP

            ALU_XOR eax eax eax
            DMA_POP eax
            DMA_STOREi eax &flag

            ALU_XOR eax eax eax
            DMA_POP eax
            DMA_STOREi eax &value

        }};

        if (flag == 1) {
            {{
                ALU_XOR eax eax eax
                DMA_LOADi eax &value
                DMA_PUSH eax
                PRG_RET
            }};
        }

    }

    return value;
}

void output(int value) {
    int aux;

    {{
        DMA_LOADi eax &value
        DMA_PUSH eax
        
        ALU_XOR ebx ebx ebx
        ALUi_ADD ebx 1
        DMA_PUSH ebx

        PRG_SYSCALL
        PRG_NOP
    }};
}

int addProc(int startPos) {
    int ret;

    {{
        ALU_XOR eax eax eax
        DMA_LOADi eax &startPos
        DMA_PUSH eax
        
        ALU_XOR ebx ebx ebx
        ALUi_ADD ebx 5
        DMA_PUSH ebx

        PRG_SYSCALL
        PRG_NOP

        DMA_POP eax
        DMA_STOREi eax &ret
    }};

    return ret;
}

void rmProc(int pid) {

    {{
        ALU_XOR eax eax eax
        DMA_LOADi eax &pid
        DMA_PUSH eax
        
        ALU_XOR ebx ebx ebx
        ALUi_ADD ebx 6
        DMA_PUSH ebx

        PRG_SYSCALL
        PRG_NOP
    }};

}