#include "GamePlayScene.h"
#include "SceneManager.h"

void GamePlayScene::Initialize()
{
	debugCamera.Initialize({ 120 }, 100.0f);
	stage.Initialize();
	//Model::SetViewProjection(&debugCamera.GetViewProjection());
	UIReset();
	UIDrawer::GetUI((size_t)UIType::Play::Reset + !input->IsConnectGamePad())->SetIsInvisible(true);
}

void GamePlayScene::Update()
{
	debugCamera.Update();
	stage.Update();
	if (input->IsTrigger(Key::R) || input->IsTrigger(JoyPad::X)) { sceneManager_->ChangeScene(Scene::Play); } // ƒŠƒgƒ‰ƒC
	if (input->IsTrigger(Mouse::Right)) { Model::SetViewProjection(&debugCamera.GetViewProjection()); }
}

void GamePlayScene::Draw()
{
	Model::PreDraw();
	stage.Draw();
	Model::PostDraw();
}