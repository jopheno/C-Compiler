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

void main(void) {
    int out;
    int a;
    int b;
    a = 1;
    out = 10;
    output(out);

    {{
        ALU_XOR clk clk clk
        ALUi_ADD clk 2
    }};

    input();

    b = cpfhd(0, &os_mem, 1024);
    output(b);
    input();
    output(os_mem[0]);
    input();
    while(a == a) {
        output(255);
        a = input();
        output(a);
        input();
        output(os_mem[a]);
        input();
    }
    
}

