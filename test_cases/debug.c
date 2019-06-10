
int a [2];
int vet [5];

void proc(int v[], int size) {
    int aux;

    v[0] = 9;
    v[1] = 8;

    aux = v[0];
    v[0] = v[1];
    v[1] = aux;

    // PRINT must be: 8, 9

    input();
    output(255);
    input();
    output(v[0]);
    input();
    output(v[1]);
}

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

    proc(a, 2);
}