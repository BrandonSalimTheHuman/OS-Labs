#include <stdio.h>

int main() {
	int result;
	asm("movl $10, %%eax; movl $20, %%ebx; addl %%ebx, %%eax;" : "=a" (result));
	printf("Result: %d\n", result);
	return 0;
}
