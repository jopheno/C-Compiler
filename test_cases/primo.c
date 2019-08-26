// Numero primo

int getprimo(int x){
	int aux;
	int i;
	int lim;

	lim = x/2;
	i = 2;
	while(i < lim){
		aux = x-((x/i)*i);

		if(aux == 0){
			return (0-1);
		}

		i = i+1;
	}

	return 1;
}

void main(void){
	int x;
	int result;

	x = input();
	result = getprimo(x);
	output(result);
}