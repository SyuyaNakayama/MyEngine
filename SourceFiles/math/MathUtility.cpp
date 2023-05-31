#include "MathUtility.h"

int NumberLoop(int num, int max, int min)
{
	if (num > max) { return num % (max + 1) + min; }
	return num;
}