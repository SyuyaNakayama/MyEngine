#pragma once
#include <random>
#include <memory>

class RandomDevice
{
protected:
	static std::random_device rnd;
	static std::mt19937 rnddev;
};

class Random_Int : private RandomDevice
{
private:
	std::unique_ptr<std::uniform_int_distribution<int>> random;

public:
	Random_Int(int min, int max);
	int operator()() { return (*random)(rnddev); }
};

class Random_Float : private RandomDevice
{
private:
	std::unique_ptr<std::uniform_real_distribution<float>> random;

public:
	Random_Float(float min, float max);
	float operator()() { return (*random)(rnddev); }
};