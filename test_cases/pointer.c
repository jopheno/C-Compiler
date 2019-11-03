/* Programa para testar ponteiros e injecao de instrucoes */

#include <stdio.c>

void maca(void) {

}

void banana(void) {
    output(3);
}

void main(void)
{
	int i;
    int vet[13];
	int amount;

    int a;
    a = banana;

    // É responsável por injetar um bloco de instruções assembly aqui !
    {{
        // Comentario aqui !
        DMA_LOADi eax &amount
        ALU_XOR eax eax eax
        ALUi_ADD eax 5
        DMA_STOREi eax &amount
    }};

	output(a); // Retorna posição do PC_Counter de banana :)

    i = input();

    output(amount);
}
