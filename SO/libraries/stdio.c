int input() {
    int value;

    {{
        ALU_XOR eax eax eax
        DMA_GIA eax
        ALU_XOR ebx ebx ebx
        ALU_SET ecx ebx eax
        PRG_JMPCi ecx input
        DMA_RI edx 129
        DMA_STOREi edx &value
    }};

    return value;
}

void output(int value) {
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