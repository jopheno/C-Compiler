
int vet[ 3 ];

int lowest ( int a[], int size )
{ int i; int low;
    i = 0;
    low = 999;
    while(i < size) {
        if (a[i] < low) {
            low = a[i];
        }
        i = i + 1;
    }
    return low;
}

int highest ( int a[], int size )
{ int i; int high;
    i = 0;
    high = 0;
    while(i < size) {
        if (a[i] > high) {
            high = a[i];
        }
        i = i + 1;
    }
    return high;
}

void main (void)
{ int low; int high;

    vet[0] = 10;
    vet[1] = 9;
    vet[2] = 8;

    low = lowest(vet, 3);
    high = highest(vet, 3);

    //printf("Lowest = %d && Higher = %d\n", low, high);
    output(low);
    input();
    output(high);

    return 0;
}
