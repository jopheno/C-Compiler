int cpid;

#include <stdio.c>

void recCmd(int cmd) {
    int aux;
    aux = 0;
    // Change process
    if (cmd == 0) {
        out(205); // Prints Cd
        cpid = in();

        if (cpid > 11) {
            cpid = 11;
        }

        return;
    }

    // Add new process
    if (cmd == 1) {
        out(2781); // Prints Add
        aux = in();
        cpid = addProc(aux);
        return;
    }

    // Remove process
    if (cmd == 2) {
        out(3566); // Prints DEE
        aux = in();
        rmProc(aux);
        return;
    }
}

int proc_out(void) {
    int aux;

    {{
        ALU_XOR eax eax eax
        DMA_LOADi eax &cpid
        DMA_PUSH eax
        
        ALU_XOR ebx ebx ebx
        ALUi_ADD ebx 7
        DMA_PUSH ebx

        PRG_SYSCALL
        PRG_NOP
        DMA_POP eax
        DMA_STOREi eax &aux
    }};

    return aux;
}

void proc_in(int value) {
    {{
        ALU_XOR ecx ecx ecx
        DMA_LOADi ecx &value
        DMA_PUSH ecx

        ALU_XOR eax eax eax
        DMA_LOADi eax &cpid
        DMA_PUSH eax
        
        ALU_XOR ebx ebx ebx
        ALUi_ADD ebx 8
        DMA_PUSH ebx

        PRG_SYSCALL
        PRG_NOP
    }};

}

void main(void) {
    int loop;
    int cmd;
    int cmp;
    int aux;
    int ia;
    int count;

    count = 0;
    cmd = 0;
    ia = 0;
    loop = 1;
    cpid = 11; // Any available process to visualize, wait for command

    {{
        ALU_XOR eax eax eax
        SCHED_PC eax
        DMA_SO 131 eax
    }};

    cmd = in();


    //addProc(3072);

    cmp = 32767;

    while (loop == loop) {

        // Verify if there is any input to read
        {{
            ALU_XOR eax eax eax
            DMA_GIA eax
            DMA_STOREi eax &ia
        }};

        // Prints PID into last display
        {{
            DMA_LOADi eax &cpid
            DMA_SO 129 eax
            DMA_STOREi eax &cpid
        }};

        // There is input
        if (ia > 0) {
            // Read the input value
            cmd = in();

            // Verify if it is a command
            // cmp = 32768
            if (cmd > cmp) {
                // It's indeed a command, let's recognize it.
                cmd = cmd - cmp;
                cmd = cmd - 1;

                recCmd(cmd);
            } else {
                // Once its not a command, it is a new input
                proc_in(cmd);
            }
        }

        ia = 0;

        // Verify current PID
        if (cpid != 11) {
            // Get output information from OS to print
            aux = proc_out();
            out(aux);
            count = count + 1;
        } else {
            out(cmd);
        }
    }
}
