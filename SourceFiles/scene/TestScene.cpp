#include "TestScene.h"

void TestScene::Initialize()
{
	models[0] = Model::Create("cube");
	models[1] = Model::Create("cube");
	worldTransforms[0].Initialize();
	worldTransforms[1].Initialize();
	worldTransforms[1].translation.x = 4.0f;
	debugCamera.Initialize();
	Model::SetViewProjection(&debugCamera.GetViewProjection());
	models[0]->GetMatrial()->SetSprite(Sprite::Create("stages/floor.png"));
	models[1]->GetMatrial()->SetSprite(Sprite::Create("stages/floor.png"));
}

void TestScene::Update()
{
	debugCamera.Update();
	worldTransforms[0].Update();
	worldTransforms[1].Update();
	Sprite* modelSprite = models[0]->GetMatrial()->GetSprite();
	modelSprite->SetTextureLeftTop(modelSprite->GetTextureLeftTop() + Vector2(1, 0));
	models[0]->Update();
	models[1]->Update();
}

void TestScene::Draw()
{
	Model::PreDraw();
	models[0]->Draw(worldTransforms[0]);
	models[1]->Draw(worldTransforms[1]);
	Model::PostDraw();
}