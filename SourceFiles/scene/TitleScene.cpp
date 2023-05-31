#include "TitleScene.h"
#include "ImGuiManager.h"
#include "UIDrawer.h"
#include "WindowsAPI.h"
#include "SceneManager.h"

void SelectUI::Initialize()
{
	lerpRateRad = 0;
	offset = {};
	sprite->SetPosition(pos + offset);
}

void SelectUI::Update()
{
	lerpRateRad += 5;
	offset.x = (-std::cos(lerpRateRad) + 1.0f) * 50.0f;
	sprite->SetPosition(pos + offset);
}

void TitleScene::Initialize()
{
	Stage::SetStageNum(0);
	stage.Initialize();
	titleUI = UIDrawer::GetUI(0);
	titleUI->SetPosition({ WindowsAPI::WIN_SIZE.x / 2.0f,300.0f });
	for (size_t i = 0; i < 2; i++)
	{
		selectUIs[i].pos = { WindowsAPI::WIN_SIZE.x / 2.0f,600.0f + (float)i * 150.0f };
		selectUIs[i].sprite = UIDrawer::GetUI(i + 1);
		selectUIs[i].SetSpritePos();
	}
}

void TitleScene::Update()
{
	stage.Update();
	// チュートリアルかゲームプレイか選択する処理
	(this->*Select)();
}

void TitleScene::ToPlay()
{
	selectUIs[0].Update();
	if (input->IsTrigger(Key::S))
	{
		selectUIs[0].Initialize();
		Select = &TitleScene::ToTutorial;
	}
	if (input->IsTrigger(Key::Space)) { sceneManager_->ChangeScene(Scene::Select, false); }
}

void TitleScene::ToTutorial()
{
	selectUIs[1].Update();
	if (input->IsTrigger(Key::W))
	{
		selectUIs[1].Initialize();
		Select = &TitleScene::ToPlay;
	}
	if (input->IsTrigger(Key::Space)) { sceneManager_->ChangeScene(Scene::Tutorial); }
}

void TitleScene::Draw()
{
	Model::PreDraw();
	stage.Draw();
	Model::PostDraw();
}