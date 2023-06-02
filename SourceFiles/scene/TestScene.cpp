#include "TestScene.h"

void TestScene::Initialize()
{
	model = Model::Create("cube");
	worldTransform.Initialize();
	debugCamera.Initialize();
	Model::SetViewProjection(&debugCamera.GetViewProjection());
}

void TestScene::Update()
{
	debugCamera.Update();
	worldTransform.Update();
}

void TestScene::Draw()
{
	Model::PreDraw();
	model->Draw(worldTransform);
	Model::PostDraw();
}
