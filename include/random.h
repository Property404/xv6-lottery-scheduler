#ifndef _RANDOM_H
#define _RANDOM_H
/*
 * Multiply with carry code by George Marsaglia
 *
 * Modified for XV6 by Dagan Martinez
 */
#include "types.h"

#define PHI 0x9e3779b9

static uint Q[4096], c = 362436;

static void srand(uint x)
{
	int i;

	Q[0] = x;
	Q[1] = x + PHI;
	Q[2] = x + PHI + PHI;

	for (i = 3; i < 4096; i++)
			Q[i] = Q[i - 3] ^ Q[i - 2] ^ PHI ^ i;
}

static uint rand(void)
{
	if(sizeof(unsigned long long) != 8){
		return 0;
	}
	unsigned long long t, a = 18782LL;
	static uint i = 4095;
	uint x, r = 0xfffffffe;
	i = (i + 1) & 4095;
	t = a * Q[i] + c;
	c = (t >> 32);
	x = t + c;
	if (x < c) {
	    x++;
	    c++;
	}
	return (Q[i] = r - x);
}
#endif
