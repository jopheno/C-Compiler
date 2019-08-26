// Cálculo do Fibonacci

int calcfib(int n){

	int x;
	int y;
	int aux;
	int i;

	x = 0;
	y = 1;
	aux = 0;
	i = 2;

	if(n == 0){
		return 0;
	}

	if(n == 1){
		return 1;
	}

	while(i <= n){
		aux = x + y;
		x = y;
		y = aux;
		i = i + 1;
	}
	return y;
}

void main(void) {

    int num;
	int result;
	num = input();

	result = calcfib(num);

	output(result);
}
