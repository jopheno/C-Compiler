/* programa para ordena��o por sele��o de
   uma matriz com dez elementos. */

int vet[ 10 ];

int minloc ( int a[], int low, int high )
{	int i; int x; int k;
	k = low;
	x = a[low];
	i = low + 1;
	while (i < high){
		if (a[i] < x){
			x = a[i];
			k = i;
		}
		i = i + 1;
	}
	return k;
}

void sort( int a[], int low, int high)
{	int i; int k;
	i = low;
	while (i < high-1){
		int t;
		k = minloc(a,i,high);
		output(15);
		input();
		output(k);
		input();
		t = a[k];
		a[k] = a[i];
		a[i] = t;
		i = i + 1;
	}
}

void main(void)
{
	int i;
	i = 0;
	while (i < 10){
		vet[i] = input();
		i = i + 1;
	}
	sort(vet,0,10);
	i = 0;
	output(4095);
	while (i < 10){
		input();
		output(vet[i]);
		i = i + 1;
	}
}
