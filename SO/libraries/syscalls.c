int getNextFPos(int start) {
    int i;
    int ret;

    i = start;
    ret = 4800;
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

    return 4800; // When last pos is returned no position was available.

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
                return i;
            }
        }
        i = i + 3;
    }

    return 50;
}

int getCurrentPSP() {
    int psp;
    {{
        PRG_GSA eax
        PRG_NOP
        DMA_STOREi eax &psp
    }};

    // Need to disconsider this function call
    psp = psp - 1;

    return psp;
}

int popFromPS() {
    // This function will not work, once it will
    // POP the position that called this function
    int pop_value;
    {{
        PRG_POP eax
        PRG_NOP
        DMA_STOREi eax &pop_value
    }};

    return pop_value;
}

void pushToPS(int push_value) {
    // This function will not work, once it will
    // PUSH a position above the returned value
    // from this function and the return will
    // return to the position pushed
    {{
        DMA_LOADi eax &push_value
        PRG_PUSH eax
        PRG_NOP
    }};
}

int getPSP(int pid) {
    int shellProcInfoPos;
    shellProcInfoPos = procs[2*pid];
    return procInfo[shellProcInfoPos+8];
}

int isBlocked(int pid) {
    int shellProcInfoPos;
    shellProcInfoPos = procs[2*pid];
    return procInfo[shellProcInfoPos+2];
}

void setBlocked(int pid) {
    int shellProcInfoPos;
    shellProcInfoPos = procs[2*pid];
    procInfo[shellProcInfoPos+2] = 1;
}

void setPSP(int pid, int psp_value) {
    int shellProcInfoPos;
    shellProcInfoPos = procs[2*pid];
    procInfo[shellProcInfoPos+8] = psp_value;
}

void addPSV(int pid, int d, int ps_value) {
    int shellProcInfoPos;
    shellProcInfoPos = procs[2*pid];
    shellProcInfoPos = shellProcInfoPos + d;
    procInfo[shellProcInfoPos+10] = ps_value;
}

int getPSV(int pid, int d) {
    int shellProcInfoPos;
    shellProcInfoPos = procs[2*pid];
    shellProcInfoPos = shellProcInfoPos + d;

    return procInfo[shellProcInfoPos+10];
}

int setAllocIndex(int pid, int index) {
    int shellProcInfoPos;
    shellProcInfoPos = procs[2*pid];
    procInfo[shellProcInfoPos+9] = index;
}

int getAllocIndex(int pid) {
    int shellProcInfoPos;
    shellProcInfoPos = procs[2*pid];
    return procInfo[shellProcInfoPos+9];
}

int getCachedData(int pid) {
    int shellProcInfoPos;
    shellProcInfoPos = procs[2*pid];
    return procInfo[shellProcInfoPos+5];
}

void disableProc(int pid) {
    setBlocked(pid);
}

void removeProc(int pid) {
    int shellProcInfoPos;
    int allocIndex;
    int cachedPos;
    int size;
    int count;
    int aux;

    count = 0;

    aux = 2*pid;
    shellProcInfoPos = procs[aux];
    while(count < 32) {
        aux = shellProcInfoPos+count;
        procInfo[aux] = 0;
        count = count + 1;
    }
    aux = 2*pid;
    aux = aux + 1;
    procs[aux] = 0;

    allocIndex = getAllocIndex(pid);
    dataHeader[allocIndex + 2] = 0;
    size = dataHeader[allocIndex + 1];
    cachedPos = getCachedData(pid);

    /*while(count < size) {
        aux = cachedPos + count;

        {{
            DMA_LOADi eax &aux
            ALU_XOR ebx ebx ebx
            DMA_STORE ebx eax
        }};

        count = count + 1;
    }*/
}

int getESP(int pid) {
    int shellProcInfoPos;
    shellProcInfoPos = procs[2*pid];
    return procInfo[shellProcInfoPos+6];
}

void setESP(int pid, int esp_value) {
    int shellProcInfoPos;
    shellProcInfoPos = procs[2*pid];
    procInfo[shellProcInfoPos+6] = esp_value;
}

int getPC(int pid) {
    int shellProcInfoPos;
    shellProcInfoPos = procs[2*pid];
    return procInfo[shellProcInfoPos+7];
}

void setPC(int pid, int last_pos) {
    int shellProcInfoPos;
    shellProcInfoPos = procs[2*pid];
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
    int count;

    new_pid = 500; // ERROR

    count = 0;
    i = pid+1;

    while (count < 10) {

        if (i > 9) {
            i = 0;
        }
    
        aux = 2*i;
        aux = aux + 1;

        if (new_pid == 500) {
            if (procs[aux] == 1) {
                if (isBlocked(i) == 0) {
                    if (i != pid) {
                        new_pid = i;
                    }
                }
            }
        }

        i = i + 1;
        count = count + 1;
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
    int aaa;
    int allocIndex;

    size = getProcessSize(start_pos);

    size = size + 300;

    proc_f = 500; // ERROR

    i = 0;
    while (i < 10) {
        aux = 2*i;
        aux = aux + 1;

        if (proc_f == 500) {
            if (procs[aux] == 0) {
                procs[aux] = 1;
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

    // Reset IO information from process
    procIOInfo[3*proc_f] = 0;
    procIOInfo[3*proc_f+1] = 0;
    procIOInfo[3*proc_f+2] = 0;
    
    procs[2*proc_f] = procInfo_f; // procInfoPos
    //procs[proc_f+1] = 1; // bool

    procInfo[procInfo_f] = proc_f; // procId
    procInfo[procInfo_f+1] = 1; // isBeenUsed
    procInfo[procInfo_f+2] = 0; // isBlocked
    procInfo[procInfo_f+3] = size; // dataSize
    procInfo[procInfo_f+4] = start_pos; // dataPos
    procInfo[procInfo_f+7] = 0; // PC_pos
    procInfo[procInfo_f+8] = 0; // PrgStackAmount

    i = 10;
    while(i < 31) {
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

    aux = aux + size;
    procInfo[procInfo_f+6] = aux; // dataStackPointer

    allocIndex = alloc(allocPos, size);
    procInfo[procInfo_f+9] = allocIndex; // allocated index

    return proc_f;

}

/*void sys_output(void) {
    int farg;

    {{
        DMA_POP eax
        DMA_STOREi eax &farg
    }};

    output(farg);
}*/

void sys_output(void) {
    int farg;
    int aux;

    // DMA_POP argument:
    {{
        DMA_POP eax
        DMA_STOREi eax &farg
    }};

    //output(farg);

    aux = 3*curr_proc;
    aux = aux + 2;

    procIOInfo[aux] = farg;
}

/*void sys_input(void) {
    {{
        ALU_XOR eax eax eax
        DMA_GIA eax
        ALU_XOR ebx ebx ebx
        ALU_SET ecx ebx eax
        PRG_JMPCi ecx sys_input
        DMA_RI edx 129
        DMA_PUSH edx
    }};
}*/

void sys_input(void) {
    int aux;
    int aaa;
    int flag;
    int value;

    flag = 0;
    value = 0;
    aux = 3*curr_proc;
    aaa = procIOInfo[aux];

    if (aaa == 1) {
        procIOInfo[aux] = 0;
        aux = aux + 1;
        value = procIOInfo[aux];

        // VALUE
        {{
            ALU_XOR eax eax eax
            DMA_LOADi eax &value
            DMA_PUSH eax
        }};

        // FLAG
        {{
            ALU_XOR eax eax eax
            ALUi_ADD eax 1
            DMA_PUSH eax
        }};

        //aaa = 0;
    } else {
        // VALUE
        {{
            ALU_XOR eax eax eax
            ALUi_ADD eax 0
            DMA_PUSH eax
        }};

        // FLAG
        {{
            ALU_XOR eax eax eax
            ALUi_ADD eax 0
            DMA_PUSH eax
        }};
    }
}

/*
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
}*/

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
        DMA_LOADi eax &procId
        DMA_PUSH eax
    }};

    //return procId;

}

void sys_rm_proc(void) {
    int pid;
    int allocIndex;

    {{
        // POP process start position
        DMA_POP eax
        DMA_STOREi eax &pid
    }};

    //removeProc(pid);
    disableProc(pid);

}

void sys_proc_output(void) {
    int pid;
    int aux;
    int value;

    // DMA_POP argument:
    {{
        DMA_POP eax
        DMA_STOREi eax &pid
    }};

    aux = 3*pid;
    aux = aux + 2;

    value = procIOInfo[aux];

    {{
        DMA_LOADi eax &value
        DMA_PUSH eax
    }};
}

void sys_proc_input(void) {
    int pid;
    int inp;
    int aux;
    int aaa;

    // DMA_POP argument:
    {{
        ALU_XOR eax eax eax
        DMA_POP eax
        DMA_STOREi eax &pid
    }};

    // DMA_POP argument:
    {{
        ALU_XOR eax eax eax
        DMA_POP eax
        DMA_STOREi eax &inp
    }};

    aux = 3*pid;
    aaa = procIOInfo[aux];

    if (aaa == 0) {
        procIOInfo[aux] = 1;
        aux = aux + 1;
        procIOInfo[aux] = inp;
    }
}