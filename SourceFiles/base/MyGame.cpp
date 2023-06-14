#include "MyGame.h"
#include "Model.h"
#include "ImGuiManager.h"
#include "Audio.h"

void MyGame::Initialize()
{
	Framework::Initialize();
	sceneManager->ChangeScene(Scene::Play, false);
	Model::StaticInitialize();
	ParticleManager::Initialize();
	ImGuiManager::Initialize();
	Audio::StaticInitialize();
	postEffect = std::make_unique<PostEffect>();
	postEffect->Initialize();
}

void MyGame::Update()
{
	ImGuiManager::Begin();
	Framework::Update();
	Model::StaticUpdate();
	ParticleManager::Update();
	ImGuiManager::End();
}

void MyGame::Draw()
{
	postEffect->PreDrawScene();
	sceneManager->Draw();
	ParticleManager::Draw();
	postEffect->PostDrawScene();
	
	dxCommon->PreDraw();
	postEffect->Draw();
	ImGuiManager::Draw();
	dxCommon->PostDraw();
}

void MyGame::Finalize()
{
	ImGuiManager::Finalize();
	Audio::Finalize();
	Framework::Finalize();
}