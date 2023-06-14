#include "GamePlayScene.h"
#include "Sprite.h"

void GamePlayScene::Initialize()
{
	Model::SetViewProjection(&debugCamera);
	debugCamera.Initialize();
}

void GamePlayScene::Update()
{
	debugCamera.Update();
}

void GamePlayScene::Draw()
{
}