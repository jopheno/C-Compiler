// Algoritmo para verificar se vetor é palíndromo

int vet[ 4 ];

int palindromo(int a[], int low, int high){
	int l; int h;

	l = low;
	h = high-1;

	while(l < h){
		if(a[l] != a[h]){
			return 2;
		}
		l = l+1;
		h = h-1;
	}
	return 1;
}

void main(void){
	
	int i;
	int result;

	i = 0;
	while (i < 4){
		vet[i] = input();
		i = i + 1;
	}

	result = palindromo(vet,0,4);
	output(result);

}
