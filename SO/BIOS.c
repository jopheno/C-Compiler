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

void main(void) {
    output(10);

    input();

    init();
}

