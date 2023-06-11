#include "TestScene.h"
#include "SpriteCommon.h"

void TestScene::Initialize()
{
	Model::SetViewProjection(&debugCamera);
	sprites[0] = Sprite::Create("floor.png");
	sprites[1] = Sprite::Create("wall.png");
	for (size_t i = 0; i < 2; i++) 
	{
		sprites[i]->size *= 7.0f; 
		sprites[i]->color.a = 0.5f;
	}
	sprites[1]->position.x = sprites[0]->size.x;
	models[0] = Model::Create("candle", true);
	models[1] = Model::Create("cube");
	models[1]->GetMaterial()->SetSprite(Sprite::Create("wall.png"));
	for (auto& w : worldTransforms) { w.Initialize(); }
	worldTransforms[0].scale *= 2.0f;
	worldTransforms[0].translation -= {1.5f, 1.0f};
	worldTransforms[1].scale *= 2.0f;
	worldTransforms[1].translation -= {-1.5f, 1.0f};
	worldTransforms[2].translation.y = -2.0f;
	worldTransforms[2].scale = { 3,1,2 };
	debugCamera.Initialize({}, 10);
	for (size_t i = 0; i < 2; i++)
	{
		particleProps[i] =
		{
			worldTransforms[i].translation + Vector3(0,3.2f),
			{0,0.02f,0},{0,0.0005f,0},0.025f,0.001f,0,30,0.4f
		};
	}
}

void TestScene::Update()
{
	debugCamera.Update();
	for (auto& w : worldTransforms) { w.Update(); }
	for (size_t i = 0; i < 2; i++)
	{
		models[i]->Update();
		sprites[i]->Update();
		ParticleManager::GetParticleGroup(i)->Add(particleProps[i]);
	}
}

void TestScene::Draw()
{
	SpriteCommon::PreDraw();
	sprites[0]->Draw();
	sprites[1]->Draw();
	SpriteCommon::PostDraw();

	Model::PreDraw();
	models[0]->Draw(worldTransforms[0]);
	models[0]->Draw(worldTransforms[1]);
	models[1]->Draw(worldTransforms[2]);
	Model::PostDraw();
}