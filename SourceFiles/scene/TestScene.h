#pragma once
#include "BaseScene.h"
#include "Model.h"

class TestScene : public BaseScene
{
	std::unique_ptr<Model> model;
	WorldTransform worldTransform;

	void Initialize();
	void Update();
	void Draw();
};