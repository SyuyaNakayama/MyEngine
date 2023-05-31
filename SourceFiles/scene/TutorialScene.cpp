#include "TutorialScene.h"
#include "SceneManager.h"
#include <imgui.h>
using namespace UIType;

void TutorialScene::Initialize()
{
	Stage::SetStageNum((int)Stage::StageNum::Tutorial);
	stage.Initialize();
	
	UIDrawer::GetUI((size_t)Play::Reset + !input->IsConnectGamePad())->SetIsInvisible(true);
	UIReset();

	uiBoxes[0].Initialize({ -33,9,-60 }, { 10,10,15 }, (size_t)Tutorial::Move + input->IsConnectGamePad());	// 移動
	uiBoxes[1].Initialize({ -33,9,-60 }, { 10,10,15 }, (size_t)Tutorial::CameraMove + input->IsConnectGamePad(), { 960,200 });	// 視点移動
	uiBoxes[2].Initialize({ -60,9,-60 }, { 14,10,13 }, (size_t)Tutorial::TutorialText1);	// 近くの燭台を灯そう
	uiBoxes[3].Initialize({ -60,9,-42 }, { 15,10,3 }, (size_t)Tutorial::Jump + input->IsConnectGamePad());	// ジャンプ
	uiBoxes[4].Initialize({ -60,9,-21 }, { 15,10,11 }, (size_t)Tutorial::TutorialText2);	// HPが減るよ
	uiBoxes[5].Initialize({ -60,9,0 }, { 15,10,8 }, (size_t)Tutorial::TutorialText3);		// 新たな燭台を灯そう
	uiBoxes[6].Initialize({ -60,9,21 }, { 15,10,11 }, (size_t)Tutorial::LightChange + input->IsConnectGamePad(), { 960,200 });	// 火を切り替える
	uiBoxes[7].Initialize({ -60,9,21 }, { 15,10,11 }, (size_t)Tutorial::TutorialText4);		// 火を切り替える
	uiBoxes[8].Initialize({ -48,9,60 }, { 9,10,15 }, (size_t)Tutorial::TutorialText5);		// 火の色によって変わるよ
	uiBoxes[9].Initialize({ -24,9,60 }, { 9,10,15 }, (size_t)Tutorial::TutorialText6);		// スイッチを移動しよう
	uiBoxes[10].Initialize({ 18,9,60 }, { 9,10,15 }, (size_t)Tutorial::TutorialText7);		// スイッチは一つだけじゃない
	uiBoxes[11].Initialize({ 60,9,54 }, { 15,10,9 }, (size_t)Tutorial::TutorialText8);		// 上へ行きたい
	uiBoxes[12].Initialize({ 42,31,60 }, { 9,10,15 }, (size_t)Tutorial::TutorialText9);		// 鍵を取得してみよう
	uiBoxes[13].Initialize({ 18,31,60 }, { 9,10,15 }, (size_t)Tutorial::TutorialText10);	// 鍵の欠片もあるよ
	uiBoxes[14].Initialize({ -6,31,60 }, { 9,10,15 }, (size_t)Tutorial::TutorialText11);	// 鍵を使ってみよう
	uiBoxes[15].Initialize({ -36,31,24 }, { 21,10,15 }, (size_t)Tutorial::TutorialText12);	// あのドアがゴールだ
}

void TutorialScene::Update()
{
	stage.Update();
	if (input->IsTrigger(Key::R) || input->IsTrigger(JoyPad::X)) { sceneManager_->ChangeScene(Scene::Tutorial); } // リトライ
	// UIの更新
	for (auto& uiSphere : uiBoxes) { uiSphere.Update(); }
}

void TutorialScene::Draw()
{
	Model::PreDraw();
	stage.Draw();
	Model::PostDraw();
}

void UIBox::Initialize(const Vector3& pos, const Vector3& rad, size_t uiIndex, const Vector3& uipos_)
{
	worldTransform.Initialize();
	worldTransform.translation = pos;
	worldTransform.scale = rad;
	worldTransform.Update();
	ui = UIDrawer::GetUI(uiIndex);
	collisionAttribute = CollisionAttribute::UI;
	collisionMask = CollisionMask::UI;
	uipos = uipos_;
}

void UIBox::Update()
{
	ui->SetIsInvisible(true);
}

void UIBox::OnCollision(BoxCollider* collider)
{
	ui->SetIsInvisible(false);
	ui->SetPosition(uipos);
}