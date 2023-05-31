#pragma once

const float PI = 3.141592f;

class Angle
{
	float angle;

public:
	float operator-() { return -angle; }
	void operator++(int) { angle += PI / 180.0f; }
	void operator--(int) { angle -= PI / 180.0f; }
	void operator+=(float rad) { angle += rad; }
	void operator+=(int deg) { angle += deg * PI / 180.0f; }
	void operator-=(float rad) { angle -= rad; }
	void operator-=(int deg) { angle -= deg * PI / 180.0f; }
	operator float() { return angle; }
	Angle(float rad) { angle = rad; } // ラジアン代入
	Angle(int deg) { angle = (float)deg * PI / 180.0f; } // 度数代入
};

enum class Axis { X, Y, Z };

// 数をループさせる関数
int NumberLoop(int num, int max, int min = 0);