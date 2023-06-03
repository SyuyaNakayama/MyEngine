#pragma once
#include "BaseScene.h"
#include "Model.h"

class TestScene : public BaseScene
{
	std::unique_ptr<Model> models[2];
	WorldTransform worldTransforms[2];

	void Initialize();
	void Update();
	void Draw();
};