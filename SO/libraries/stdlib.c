void libSwitch(int pos, int type) {

    {{
        // Removes the return information that was inserted with this function call
        // PRG_POP ecx
        // ALU_XOR ecx ecx ecx

        // Load the variables needed to start SWITCH
        ALU_XOR eax eax eax
        DMA_LOADi aw &type
        DMA_LOADi ax &pos

        PRG_SWITCH eax
    }};

}

void libSwitchCallback(int pos, int type, int callback) {

    {{
        DMA_LOADi edx &callback
        DMA_PUSH edx
        // Removes the return information that was inserted with this function call
        //PRG_POP ecx
        //ALU_XOR ecx ecx ecx

        // Load the variables needed to start SWITCH
        ALU_XOR eax eax eax
        DMA_LOADi aw &type
        DMA_LOADi ax &pos

        PRG_SWITCH eax

        // No backup needed once a JMP will be executed
    }};

}
