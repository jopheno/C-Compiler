
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
        [9-31] = prgContext, // 23 stacked calls
    }
*/

// For 10 processes
int curr_proc;
// 0 -> procInfoPos, 1 -> bool
int procs[20];
int procInfo[640];

// [dataHeader]
// 0 -> dataPos, 1 -> dataSize, 2 -> bool
// 3N -> dataPos, 3N+1 -> dataSize, 3N+2 -> bool

int dataHeader[30];
int data[2000];

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

    procInfoPos = procs[curr_proc];
    cachedData = procInfo[procInfoPos+5];

    // DMA_POP SYSCALL type:
    {{
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

    if (type == 5) {
        sys_add_proc();
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
    int aux;

    {{
        SCHED_RTimer
        ALU_XOR eax eax eax
        ALU_ADD eax eax kernel
        DMA_STOREi aw &lastPos
    }};

    pid = getNextProcess(curr_proc);

    cachedData = getCachedData(pid);
    setPC(pid, lastPos);

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
    
    i = 0;
    while(i < 30) {
        dataHeader[i] = 0;
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
