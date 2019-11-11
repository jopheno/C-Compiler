int os_mem[512]; // SKIP the size of OS instructions

// Include useful functions such as input and output
#include <stdio.c>

void syscall(void) {
    
    // Removes return information from this main function
    {{
        PRG_POP ecx
        ALU_XOR ecx ecx ecx
    }};

    output(34);

}

void timer(void) {

    {{
        SCHED_RTimer
    }};

    input();

    output(35);

    {{
        PRG_HALT
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
    int a;
    output(16);
    a = input();

    {{
        SCHED_SYSCalli syscall
        SCHED_Timeri timer
        SCHED_DMAInti unlock
        SCHED_RTimer
        SCHED_Start
    }};

    output(32);

    while(a == a) {
        a = a + 1;
        output(a);
    }
    
    output(2);

}