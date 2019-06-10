
int a [2];
int vet [5];

void main(void) {
    int i;
    int aux;

    vet[0] = 2;
    vet[1] = 3;
    vet[2] = 4;
    vet[3] = 5;
    vet[4] = 6;

    aux = vet[3];
    vet[3] = vet[4];
    vet[4] = aux;

    // PRINT must be: 2, 3, 4, 6, 5

    while(i < 5) {
        input();
        output(vet[i]);
        i = i + 1;
    }
}