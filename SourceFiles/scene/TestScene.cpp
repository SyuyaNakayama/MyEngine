#include "TestScene.h"
#include "SpriteCommon.h"

void TestScene::Initialize()
{
	Model::SetViewProjection(&debugCamera);
	models[0] = Model::Create("candle", true);
	models[1] = Model::Create("switch_table", true);
	worldTransforms[0].Initialize();
	worldTransforms[1].Initialize();
	worldTransforms[1].translation.x = 4.0f;
	debugCamera.Initialize({ 2 }, 10);
	sprites[0] = Sprite::Create("floor.png");
	sprites[1] = Sprite::Create("wall.png");
	sprites[1]->position.x = 144.0f;
	particleProps[0] =
	{
		{0,0,-1},{0,0.02f,0},{0,0.0005f,0},
		0.025f,0.001f,0,30,0.4f
	};
}

void TestScene::Update()
{
	debugCamera.Update();
	for (size_t i = 0; i < 2; i++)
	{
		worldTransforms[i].Update();
		models[i]->Update();
		sprites[i]->Update();
		//ParticleManager::GetParticleGroup(i)->Add(particleProps[i]);
	}
	ParticleManager::GetParticleGroup(0)->Add(particleProps[0]);
}

void TestScene::Draw()
{
	SpriteCommon::PreDraw();
	sprites[0]->Draw();
	sprites[1]->Draw();
	SpriteCommon::PostDraw();

	Model::PreDraw();
	models[0]->Draw(worldTransforms[0]);
	models[1]->Draw(worldTransforms[1]);
	Model::PostDraw();
}