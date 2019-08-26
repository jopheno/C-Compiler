// Cálculo do N-ésimo número de Mersenne

int mersenne(int n){
	int i;
	int m;

	i = 1;
	m = 1;

	while(i <= n){
		m = m*2;
		i = i + 1;
	}

	return (m-1);
}

void main(void){
	int x;
	int result;

	x = input();
	result = mersenne(x);

	output(result);
}
