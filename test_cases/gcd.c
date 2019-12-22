
/* Um programa para calcular o mdc
   segundo o algoritmo de Euclides. */

#include <stdio.c>

int gcd (int u, int v)
{       
	if (v == 0) return u ;
	else return gcd(v,u-u/v*v);
	/* u-u/v*v == u mod v */
}

void main(void)
{	int x; int y;

	{{
		ALU_XOR eax eax eax
		ALUi_ADD eax 1
		DMA_SO 129 eax
	}};

	out(32);
	x = in();
	out(x);
	y = in();
	out(y);
	out(gcd(x,y));
}
