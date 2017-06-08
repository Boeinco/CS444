#include <stdio.h>
#include <linux/unistd.h>

#define __NR_get_slob_amt_claimed 223
#define __NR_get_slob_amt_free 222

int main(void)
{
	char* buffa, buffb, buffc, buffd, buffe;
	buffa = (char*) malloc(27739592);
	buffb = (char*) malloc(27739592);
	buffc = (char*) malloc(27739592);
	buffd = (char*) malloc(27739592);
	buffe = (char*) malloc(27739592);

	printf("Free memory: %d\n", syscall(__NR_get_slob_amt_free));
	printf("Claimed memory: %d\n", syscall(__NR_get_slob_amt_claimed));

	return 0;
}
