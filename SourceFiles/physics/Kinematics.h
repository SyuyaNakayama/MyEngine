#pragma once
#include "Vector.h"

// �^���w
// �^���̖@��
struct Motion
{
	Vector2 p = 0; // �ʒu
	Vector2 v = 0; // ���x
	Vector2 a = 0; // �����x
	Vector2 f = 0; // ���̂ɓ�����
	float m = 1;  // ����

	void Initialize(Vector2 p, Vector2 v, Vector2 a = {}, Vector2 f = {}, float m = 1);
	void Update();
};