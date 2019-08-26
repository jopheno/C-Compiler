// Algoritmo para calcular fatorial

int fatorial(int n){
	int i; int aux;
	i = 1;
	aux = 1;

	while(i <= n){
		aux = aux * i;
		i = i+1;
	}
	return aux;
}

void main(void){
	int x;
	int result;

	x = input();
	result = fatorial(x);
	output(result);
}
