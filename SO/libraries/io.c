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
        DMA_SO 131 eax
        DMA_STOREi eax &value
    }};
}

int cpfhd(int from_pos, int to_pos, int size) {
    int result;
    {{
        // ecx => resultado, ebx => size, eax => [ax = from_pos, aw = to_pos]
        DMA_LOADi aw &to_pos
        DMA_LOADi ax &from_pos
        DMA_LOADi ebx &size
        DMA_REQFHDD ecx ebx eax
        DMA_STOREi ecx &result
    }};

    return result;
}

int cpfram(int from_pos, int to_pos, int size) {
    int result;
    {{
        // ecx => resultado, ebx => size, eax => [ax = from_pos, aw = to_pos]
        DMA_LOADi ax &from_pos
        DMA_LOADi aw &to_pos
        DMA_LOADi ebx &size
        DMA_REQFRAM ecx ebx eax
        DMA_STOREi ecx &result
    }};

    return result;
}