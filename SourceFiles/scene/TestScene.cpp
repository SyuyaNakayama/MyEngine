#include "TestScene.h"

void TestScene::Initialize()
{
	models[0] = Model::Create("cube");
	models[1] = Model::Create("cube");
	worldTransforms[0].Initialize();
	worldTransforms[1].Initialize();
	worldTransforms[1].translation.x = 4.0f;
	debugCamera.Initialize();
	Model::SetViewProjection(&debugCamera);
	std::unique_ptr<Sprite> modelSprite = Sprite::Create("floor.png");
	modelSprite->textureSize *= 2.0f ;
	Material* material[2] = { models[0]->GetMaterial() ,models[1]->GetMaterial() };
	material[0]->SetSprite(move(modelSprite));
	material[1]->SetSprite(Sprite::Create("floor.png"));
}

void TestScene::Update()
{
	debugCamera.Update();
	worldTransforms[0].Update();
	worldTransforms[1].Update();
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