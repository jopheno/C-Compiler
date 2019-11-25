int getNextFPos(int start) {
    int i;
    int ret;

    i = start;
    ret = 2000;
    while(i<30) {
        if (dataHeader[i+2] == 1) {
            return dataHeader[i] + dataHeader[i+1];
        }
        i = i + 3;
    }

    return ret;
}

int getNxtDataPos(int space) {
    int i;
    int pos;
    int size;
    int boolean;

    int fpos; // First position
    int lpos; // First position + size
    int nfpos; // Next first position

    fpos = 0;
    lpos = 0;

    i = 0;
    while(i<30) {
        pos = dataHeader[i];
        size = dataHeader[i+1];
        boolean = dataHeader[i+2];

        nfpos = getNextFPos(i+3);

        if (boolean == 1) {
            fpos = pos;
            lpos = pos + size + 1;
        }

        if (nfpos - lpos > space) {
            return lpos;
        }


        i = i + 3;
    }

    return 2000; // When last pos is returned no position was available.

}

int alloc(int spos, int size) {
    int i;
    int nextpos;

    i = 0;
    while(i<30) {
        if (dataHeader[i+2] == 0) {
            nextpos = getNextFPos(i+3);
            if (nextpos > spos) {
                dataHeader[i] = spos;
                dataHeader[i+1] = size;
                dataHeader[i+2] = 1;
                return 1;
            }
        }
        i = i + 3;
    }

    return 0;
}

int getCachedData(int pid) {
    int shellProcInfoPos;
    shellProcInfoPos = procs[pid];
    return procInfo[shellProcInfoPos+5];
}

int getESP(int pid) {
    int shellProcInfoPos;
    shellProcInfoPos = procs[pid];
    return procInfo[shellProcInfoPos+6];
}

int getPC(int pid) {
    int shellProcInfoPos;
    shellProcInfoPos = procs[pid];
    return procInfo[shellProcInfoPos+7];
}

void setPC(int pid, int last_pos) {
    int shellProcInfoPos;
    shellProcInfoPos = procs[pid];
    procInfo[shellProcInfoPos+7] = last_pos;
}

int getProcessSize(int shellPos) {
    int shellSize;

    {{
        ALU_XOR eax eax eax
        ALUi_ADD aw &shellSize
        DMA_LOADi ax &shellPos
        ALU_XOR ebx ebx ebx
        ALUi_ADD ebx 1
        DMA_REQFHDD ecx ebx eax
        PRG_WAIT
    }};

    return shellSize;
}

int getNextProcess(int pid) {
    int i;
    int new_pid;
    int aux;

    new_pid = 500; // ERROR

    i = 0;
    while (i < 10) {
        aux = 2*i;
        aux = aux + 1;

        if (new_pid == 500) {
            if (procs[aux] == 1) {
                if (i == pid) {
                    new_pid = i;
                }
            }
        }

        i = i + 1;
    }

    if (new_pid == 500) {
        new_pid = pid;
    }

    return new_pid;

}

int addProc(int start_pos) {
    int size;
    int i;
    int aux;
    int proc_f;
    int procInfo_f;
    int allocPos;

    size = getProcessSize(start_pos);

    size = size + 300;

    proc_f = 500; // ERROR

    i = 0;
    while (i < 10) {
        aux = 2*i;
        aux = aux + 1;

        if (proc_f == 500) {
            if (procs[aux] == 0) {
                proc_f = i;
            }
        }

        i = i + 1;
    }

    if (proc_f == 500) {
        //ERROR CASE

        {{
            ALU_XOR eax eax eax
            // ERROR E001
            ALUi_ADD eax 57345
            DMA_SO 131 eax
            PRG_HALT;
        }};
    }

    // PROC INFO

    procInfo_f = 1000; // ERROR

    i = 0;
    while (i < 10) {

        if (procInfo_f == 1000) {
            aux = 64*i;
            aux = aux + 1;

            if (procInfo[aux] == 0) {
                procInfo_f = 64*i;
            }
        }

        i = i + 1;
    }

    if (procInfo_f == 1000) {
        //ERROR CASE

        {{
            ALU_XOR eax eax eax
            // ERROR E002
            ALUi_ADD eax 57346
            DMA_SO 131 eax
            PRG_HALT;
        }};
    }
    
    procs[proc_f] = procInfo_f; // procInfoPos
    procs[proc_f+1] = 1; // bool

    procInfo[procInfo_f] = proc_f; // procId
    procInfo[procInfo_f+1] = 1; // isBeenUsed
    procInfo[procInfo_f+2] = 0; // isBlocked
    procInfo[procInfo_f+3] = size; // dataSize
    procInfo[procInfo_f+4] = start_pos; // dataPos
    procInfo[procInfo_f+6] = size; // dataStackPointer
    procInfo[procInfo_f+7] = 0; // PC_pos
    procInfo[procInfo_f+8] = 0; // PrgStackAmount

    i = 9;
    while(i < 32) {
        procInfo[procInfo_f+i] = 0; // PrgStack
        i = i + 1;
    }
    
    allocPos = getNxtDataPos(size);

    //aux = &data;
    {{
        ALU_XOR eax eax eax
        ALUi_ADD eax &data
        DMA_STOREi eax &aux
    }};
    aux = aux + allocPos;
    cpfhd(start_pos, aux, size);
    
    // Wait until copy is made
    {{ 
        PRG_WAIT
    }};
    
    procInfo[procInfo_f+5] = aux; // cachedPos

    alloc(allocPos, size);

    return proc_f;

}

void sys_output(void) {
    int farg;

    // DMA_POP argument:
    {{
        DMA_POP eax
        DMA_STOREi eax &farg
    }};

    output(farg);
}

void sys_input(void) {
    {{
        ALU_XOR eax eax eax
        DMA_GIA eax
        ALU_XOR ebx ebx ebx
        ALU_SET ecx ebx eax
        PRG_JMPCi ecx sys_input
        DMA_RI edx 129
        DMA_PUSH edx
    }};
}

void sys_out(void) {
    int farg;

    // DMA_POP argument:
    {{
        DMA_POP eax
        DMA_STOREi eax &farg
    }};

    output(farg);
}

void sys_in(void) {
    {{
        ALU_XOR eax eax eax
        DMA_GIA eax
        ALU_XOR ebx ebx ebx
        ALU_SET ecx ebx eax
        PRG_JMPCi ecx sys_in
        DMA_RI edx 129
        DMA_PUSH edx
    }};
}

void sys_add_proc(void) {
    int start_pos;
    int procId;

    {{
        // POP process start position
        DMA_POP eax
        DMA_STOREi eax &start_pos
    }};

    procId = addProc(start_pos);

    {{
        ALU_XOR eax eax eax
        DMA_GIA eax
        ALU_XOR ebx ebx ebx
        ALU_SET ecx ebx eax
        PRG_JMPCi ecx sys_in
        DMA_RI edx 129
        DMA_PUSH edx
    }};
}