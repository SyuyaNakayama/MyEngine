#pragma once
#include "BaseScene.h"
#include "Model.h"
#include "ParticleManager.h"

class TestScene : public BaseScene
{
	std::unique_ptr<Model> models[2];
	WorldTransform worldTransforms[2];
	std::unique_ptr<Sprite> sprites[2];
	DiffuseParticle::AddProp particleProps[2];

	void Initialize();
	void Update();
	void Draw();
};