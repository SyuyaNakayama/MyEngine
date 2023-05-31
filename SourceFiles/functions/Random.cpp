#include "Random.h"

std::random_device RandomDevice::rnd;
std::mt19937 RandomDevice::rnddev(rnd());

Random_Int::Random_Int(int min, int max)
{
	random = std::make_unique<std::uniform_int_distribution<int>>(min, max);
}

Random_Float::Random_Float(float min, float max)
{
	random = std::make_unique<std::uniform_real_distribution<float>>(min, max);
}