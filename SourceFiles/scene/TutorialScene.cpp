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

	uiBoxes[0].Initialize({ -33,9,-60 }, { 10,10,15 }, (size_t)Tutorial::Move + input->IsConnectGamePad());	// �ړ�
	uiBoxes[1].Initialize({ -33,9,-60 }, { 10,10,15 }, (size_t)Tutorial::CameraMove + input->IsConnectGamePad(), { 960,200 });	// ���_�ړ�
	uiBoxes[2].Initialize({ -60,9,-60 }, { 14,10,13 }, (size_t)Tutorial::TutorialText1);	// �߂��̐C��𓔂���
	uiBoxes[3].Initialize({ -60,9,-42 }, { 15,10,3 }, (size_t)Tutorial::Jump + input->IsConnectGamePad());	// �W�����v
	uiBoxes[4].Initialize({ -60,9,-21 }, { 15,10,11 }, (size_t)Tutorial::TutorialText2);	// HP�������
	uiBoxes[5].Initialize({ -60,9,0 }, { 15,10,8 }, (size_t)Tutorial::TutorialText3);		// �V���ȐC��𓔂���
	uiBoxes[6].Initialize({ -60,9,21 }, { 15,10,11 }, (size_t)Tutorial::LightChange + input->IsConnectGamePad(), { 960,200 });	// �΂�؂�ւ���
	uiBoxes[7].Initialize({ -60,9,21 }, { 15,10,11 }, (size_t)Tutorial::TutorialText4);		// �΂�؂�ւ���
	uiBoxes[8].Initialize({ -48,9,60 }, { 9,10,15 }, (size_t)Tutorial::TutorialText5);		// �΂̐F�ɂ���ĕς���
	uiBoxes[9].Initialize({ -24,9,60 }, { 9,10,15 }, (size_t)Tutorial::TutorialText6);		// �X�C�b�`���ړ����悤
	uiBoxes[10].Initialize({ 18,9,60 }, { 9,10,15 }, (size_t)Tutorial::TutorialText7);		// �X�C�b�`�͈��������Ȃ�
	uiBoxes[11].Initialize({ 60,9,54 }, { 15,10,9 }, (size_t)Tutorial::TutorialText8);		// ��֍s������
	uiBoxes[12].Initialize({ 42,31,60 }, { 9,10,15 }, (size_t)Tutorial::TutorialText9);		// �����擾���Ă݂悤
	uiBoxes[13].Initialize({ 18,31,60 }, { 9,10,15 }, (size_t)Tutorial::TutorialText10);	// ���̌��Ђ������
	uiBoxes[14].Initialize({ -6,31,60 }, { 9,10,15 }, (size_t)Tutorial::TutorialText11);	// �����g���Ă݂悤
	uiBoxes[15].Initialize({ -36,31,24 }, { 21,10,15 }, (size_t)Tutorial::TutorialText12);	// ���̃h�A���S�[����
}

void TutorialScene::Update()
{
	stage.Update();
	if (input->IsTrigger(Key::R) || input->IsTrigger(JoyPad::X)) { sceneManager_->ChangeScene(Scene::Tutorial); } // ���g���C
	// UI�̍X�V
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