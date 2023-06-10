#pragma once
#include "ViewProjection.h"
#include "Quaternion.h"

class DebugCamera : public ViewProjection
{
private:
	float distance = 50;
	Quaternion rotQ = 1.0f;

public:
	void Initialize(Vector3 targetPos = { 0,0,0 }, float distance = 50);
	void Update();
};