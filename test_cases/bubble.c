
int vet[ 255 ];

void sort( int a[], int low, int high)
{	int i; int j; int k; int aux;
	i = high-1;
	while (i > 0){

		j = 0;
		while (j < i){
			if(a[j] > a[j+1]){
				aux = a[j];
				a[j] = a[j+1];
				a[j+1] = aux;
			}
			j = j+1;
		}

		i = i - 1;
	}
}

void main(void)
{
	int i;
	i = 0;
	while (i < 4){
		vet[i] = input();
		i = i + 1;
	}
	sort(vet,0,4);
	i = 0;
	while (i < 4){
		output(vet[i]);
		i = i + 1;
	}
}
