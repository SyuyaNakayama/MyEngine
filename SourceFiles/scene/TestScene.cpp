#include "TestScene.h"

void TestScene::Initialize()
{
	models[0] = Model::Create("cube");
	worldTransforms[0].Initialize();
	worldTransforms[1].Initialize();
	worldTransforms[1].translation.x = 4.0f;
	debugCamera.Initialize();
	Model::SetViewProjection(&debugCamera.GetViewProjection());
}

void TestScene::Update()
{
	debugCamera.Update();
	worldTransforms[0].Update();
	worldTransforms[1].Update();
}

void TestScene::Draw()
{
	Model::PreDraw();
	models[0]->Draw(worldTransforms[0]);
	models[0]->Draw(worldTransforms[1]);
	Model::PostDraw();
}