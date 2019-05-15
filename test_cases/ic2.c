/* Programa simples para verificar o funcionamento
da geração do código intermediário */

int sum(int x, int y) {
    return 4;
}

void main(void)
{	
    int x;
    int y;
    int z;

    x = 4;
    y = 3;

    z = sum(x, y);

    output(x+y);
}
