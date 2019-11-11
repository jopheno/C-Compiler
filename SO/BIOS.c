/*
BIOS gravada na ROM do processador.

INIT_STATUS = {
    [10] = "OK",
    [A] = "Stack error",
    [B] = "HD error",
    [C] = "RAM error",
    [D] = "",
}

*/

int os_mem[1024];

#include <stdio.c>
#include <stdlib.c>

void main(void) {
    int out;
    int a;
    int b;
    a = 1;
    out = 10;

    // Removes return information from this main function
    {{
        PRG_POP ecx
        ALU_XOR ecx ecx ecx
    }};

    output(out);

    /*{{
        ALU_XOR clk clk clk
        ALUi_ADD clk 3
    }};*/

    input();

    b = cpfhd(0, &os_mem, 1024);
    output(b);
    input();
    output(11);
    input();
    libSwitch(0, 1); // First argument is the PC position, and the second argument will be the new source [0 -> ROM | 1 -> RAM]
    output(12);
    
}

