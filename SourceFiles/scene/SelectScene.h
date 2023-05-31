#pragma once
#include "BaseScene.h"
#include "Stage.h"

class StagePlane
{
private:
	WorldTransform worldTransform;
	std::unique_ptr<Model> model;

public:
	void Initialize(const std::string& textureName, const Vector3& pos, const Vector3& scale);
	void Draw() { model->Draw(worldTransform); }
};

class SelectScene : public BaseScene
{
private:
	LightGroup* lightGroup;
	// ステージプレーン
	std::array<StagePlane, 4> stagePlanes;

public:
	void Initialize();
	void Update();
	void Draw();
};

