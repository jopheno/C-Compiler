
// Include useful functions such as input and output
/*
    int procs = {
        [procId] = procInfoPos,
        [procId+1] = boolean,
    },

    int procInfo = {
        [0] = procId,
        [1] = beenUsed,
        [2] = isBlocked,
        [3] = dataSize,
        [4] = dataPos,
        [5] = cachedData,
        [6] = dataStackPointer,
        [7] = PC_pos,
        [8] = PrgStackAmount,
        [9] = allocatedIndex,
        [10-63] = prgContext, // 55 stacked calls
    }
*/

// For 10 processes
int curr_proc;
// 0 -> procInfoPos, 1 -> bool
int procs[20];
int procInfo[640];

// [procIOInfo]
// 3N + 0 -> bool (is there input), 3N + 1 -> input value, 3N + 2 -> output value
int procIOInfo[30];

// [dataHeader]
// 0 -> dataPos, 1 -> dataSize, 2 -> bool
// 3N -> dataPos, 3N+1 -> dataSize, 3N+2 -> bool

int dataHeader[30];
int data[4800];

#include <io.c>
#include <syscalls.c>

void syscall(void) {

    int procInfoPos;
    int type;
    int cachedData;
    int lastPos;
    int aux;

    {{
        ALU_XOR eax eax eax
        ALU_ADD eax eax kernel
        DMA_STOREi aw &lastPos
    }};

    //procInfoPos = procs[2*curr_proc];
    //cachedData = procInfo[procInfoPos+5];
    cachedData = getCachedData(curr_proc);

    // DMA_POP SYSCALL type:
    {{
        ALU_XOR eax eax eax
        DMA_POP eax
        DMA_STOREi eax &type
    }};

    // OUTPUT
    if (type == 1) {
        sys_output();
    }

    if (type == 2) {
        sys_input();
    }

    // addProc
    if (type == 5) {
        sys_add_proc();
        {{
            SCHED_RTimer
        }};
    }

    // rmProc
    if (type == 6) {
        sys_rm_proc();
        {{
            SCHED_RTimer
        }};
    }

    // Get proc output
    if (type == 7) {
        sys_proc_output();
    }

    // Insert proc input
    if (type == 8) {
        sys_proc_input();
    }

    lastPos = lastPos + 1;

    {{
        ALU_XOR eax eax eax
        DMA_LOADi aw &lastPos
        DMA_LOADi ax &cachedData
        PRG_GTP eax
    }};

}

void timer(void) {
    int i;
    int pid;
    int cachedData;
    int lastPos;
    int esp_value;
    int curr_psp;
    int psp_counter;
    int aux;

    {{
        ALU_XOR eax eax eax
        ALU_ADD eax eax kernel
        DMA_STOREi aw &lastPos
    }};

    {{
        DMA_STOREi esp &esp_value
    }};

    // Store current stack pointer
    setESP(curr_proc, esp_value);
    // Store current position
    setPC(curr_proc, lastPos);

    // Get Program stack items amount
    curr_psp = getCurrentPSP();
    //output(32);
    //i = input();
    //output(curr_psp);
    //i = input();

    if (curr_psp > 55) {
        {{
            ALU_XOR eax eax eax
            // ERROR E003
            // Unable to store ProgramStack
            // information, once it is greater
            // than 55 values.
            ALUi_ADD eax 57347
            DMA_SO 131 eax
            PRG_HALT;
        }};
    }

    // Store program stack items amount
    setPSP(curr_proc, curr_psp);

    // For each stack item add to procTable
    psp_counter = 0;
    if (curr_psp > 0) {
        while(curr_psp != 0) {
            //aux = popFromPS();
            {{
                PRG_POP eax
                PRG_NOP
                DMA_STOREi eax &aux
            }};
            addPSV(curr_proc, psp_counter, aux);
            curr_psp = curr_psp - 1;
            psp_counter = psp_counter + 1;
        }
    }

    // Get next process PID
    pid = getNextProcess(curr_proc);

    // Set it as current process
    curr_proc = pid;

    // Get cached data memory position
    cachedData = getCachedData(pid);

    // Get last program counter position
    lastPos = getPC(pid);

    // For each program stack item push again into stack
    curr_psp = getPSP(pid);
    if (curr_psp > 0) {
        while(curr_psp != 0) {

            curr_psp = curr_psp - 1;

            aux = getPSV(pid, curr_psp);
            //pushToPS(aux);
            {{
                DMA_LOADi eax &aux
                PRG_PUSH eax
                PRG_NOP
            }};
        }
    }

    // Set ESP to the last used

    esp_value = getESP(pid);

    {{
        ALU_XOR esp esp esp
        DMA_LOADi esp &esp_value
        
        //ALU_XOR eax eax eax
        //DMA_LOADi eax &esp_value
        //DMA_SP eax
    }};

    {{
        ALU_XOR eax eax eax
        DMA_LOADi aw &lastPos
        DMA_LOADi ax &cachedData
        SCHED_RTimer
        PRG_GTP eax
    }};
}

void unlock(void) {

    // Removes return information from this main function
    {{
        PRG_POP ecx
        ALU_XOR ecx ecx ecx
    }};

    output(36);

}

void main(void) {
    int i;
    int shellStartPos;
    int shellProcId;
    int shellProcInfoPos;
    int aux;
    int pc_pos;

    // Correct PRG Stack

    aux = getCurrentPSP();
    if (aux > 0) {
        while(aux != 0) {
            {{
                PRG_POP eax
                PRG_NOP
            }};
            aux = aux - 1;
        }
    }

    aux = getCurrentPSP();

    if (aux != 0) {
        {{
            ALU_XOR eax eax eax
            // ERROR E000
            ALUi_ADD eax 57344
            DMA_SO 131 eax
            PRG_HALT;
        }};
    }
    
    i = 0;
    while(i < 30) {
        dataHeader[i] = 0;
        procIOInfo[i] = 0;
        i = i + 1;
    }

    i = 0;
    while(i < 20) {
        procs[i] = 0;
        i = i + 1;
    }

    i = 0;
    while(i < 640) {
        procInfo[i] = 0;
        i = i + 1;
    }

    {{
        SCHED_SYSCalli syscall
        SCHED_Timeri timer
        SCHED_DMAInti unlock
        SCHED_RTimer
        SCHED_Start
    }};

    output(2781); // PRINTs Add
    shellStartPos = input();

    shellProcId = addProc(shellStartPos);

    aux = getESP(shellProcId);

    {{
        ALU_XOR esp esp esp
        DMA_LOADi esp &aux
        
        //ALU_XOR eax eax eax
        //DMA_LOADi eax &aux
        //DMA_SP eax
    }};

    aux = getCachedData(shellProcId);
    pc_pos = getPC(shellProcId);

    {{
        SCHED_RTimer
        ALU_XOR eax eax eax
        DMA_LOADi aw &pc_pos
        DMA_LOADi ax &aux
        PRG_GTP eax
    }};

}
