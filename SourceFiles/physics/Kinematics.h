#pragma once
#include "Vector.h"

// 運動学
// 運動の法則
struct Motion
{
	Vector2 p = 0; // 位置
	Vector2 v = 0; // 速度
	Vector2 a = 0; // 加速度
	Vector2 f = 0; // 物体に働く力
	float m = 1;  // 質量

	void Initialize(Vector2 p, Vector2 v, Vector2 a = {}, Vector2 f = {}, float m = 1);
	void Update();
};