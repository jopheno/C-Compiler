// This test is used to certify that it's not possible to create variable with
// temporary internal variables name, using them could cause a lot of inconsistencies

void main(void)
{
	int a;
    int b;
    int i;
    int j;
    int t2;

    a = input();
    b = input();

    output(a+b);

}

