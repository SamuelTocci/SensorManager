#include <stdio.h>

void swap_pointers(int **pntr_a, int **pntr_b){ //pointer to pointer because pointer does
						// not actually edit the global value for the adress
	int *pntr_dummy = *pntr_a;
	*pntr_a = *pntr_b;
	*pntr_b = pntr_dummy;
}


void main(void){
	int a = 1;
	int b = 2;

	int *p = &a;
	int *q = &b;

	printf("address of p = %p and q = %p \n", p, q);

	swap_pointers( &p, &q); //adress of p and q
	printf("address of p = %p and q = %p \n", p, q);
}
