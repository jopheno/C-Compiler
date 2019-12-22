
int vet[ 4 ];

#include <stdio.c>

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
	int aaa;
	i = 0;
	while (i < 4){
		vet[i] = in();
		i = i + 1;
	}
	sort(vet,0,4);
	i = 0;
	while (i < 4){
		out(vet[i]);
		aaa = in();
		i = i + 1;
	}

	while(aaa == aaa) {
		{{
			PRG_NOP
		}};
	}
}
