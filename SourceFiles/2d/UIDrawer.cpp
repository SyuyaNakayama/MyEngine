#include "UIDrawer.h"
#include "SpriteCommon.h"
#include "SceneManager.h"

std::array<UIDrawer::SceneUI, (size_t)Scene::SceneNum - 1> UIDrawer::sceneUIs;
Scene UIDrawer::scene = Scene::Null;

void UIDrawer::SceneUI::LoadUI(const std::string& fileName, float sizeRate, Vector2 anchorPoint, bool isInvisible)
{
	std::unique_ptr<Sprite> sprite = Sprite::Create("ui/" + fileName);
	sprite->SetSize(sprite->GetSize() * sizeRate);
	sprite->SetAnchorPoint(anchorPoint);
	sprite->SetIsInvisible(isInvisible);
	uis.push_back(std::move(sprite));
}

void UIDrawer::SceneUI::Load()
{
	float objUISizeRate = 1.0f / 8.0f;

	SpriteAnimation* spriteAnime = nullptr;
	switch (scene)
	{
	case Scene::Title:
		LoadUI("title/tutorial.png", 2.0f, { 0.5f,0.5f });	// �^�C�g��
		LoadUI("title/select.png", 1.0f, { 0.5f,0.5f });	// �Q�[���v���C
		LoadUI("title/tutorial.png", 0.9f, { 0.5f,0.5f });	// �`���[�g���A��
		break;
	case Scene::Select:
		// ����
		LoadUI("keyboard/light.png", 0.8f, { 0.5f,0.5f }, true);
		LoadUI("controller/light.png", 0.8f, { 0.5f,0.5f }, true);
		break;
	case Scene::Tutorial:
	case Scene::Play:
		// �v���C���[��HP�Q�[�W�̔w�i
		LoadUI("white1x1.png");
		LoadUI("white1x1.png");
		// �v���C���[��HP�Q�[�W�̘g
		LoadUI("hpUI.png", 1.0f, { 0.0f,0.0f }, true);
		// �C���UI
		for (size_t i = 0; i < 8; i++) { LoadUI("CandleUI.png", objUISizeRate, {}, true); }
		// ��
		LoadUI("key/key.png", objUISizeRate, {}, true);
		// ���p�[�c
		LoadUI("key/parts1.png", objUISizeRate * 2.0f, {}, true);
		LoadUI("key/parts2.png", objUISizeRate * 2.0f, {}, true);
		LoadUI("key/parts3.png", objUISizeRate * 2.0f, {}, true);
		LoadUI("key/parts4.png", objUISizeRate * 2.0f, {}, true);
		LoadUI("key/parts4.png", objUISizeRate * 2.0f, {}, true);
		LoadUI("key/parts5.png", objUISizeRate * 2.0f, {}, true);
		// ����
		LoadUI("keyboard/light.png", 0.8f, { 0.5f,0.5f }, true);
		LoadUI("controller/light.png", 0.8f, { 0.5f,0.5f }, true);
		// ���o�[�؂�ւ�
		LoadUI("keyboard/lever.png", 0.8f, { 0.5f,0.0f }, true);
		LoadUI("controller/lever.png", 0.8f, { 0.5f,0.0f }, true);
		// �����J����
		LoadUI("keyboard/keyOpen.png", 0.8f, { 0.5f,0.0f }, true);
		LoadUI("controller/keyOpen.png", 0.8f, { 0.5f,0.0f }, true);
		// ���Z�b�gUI
		LoadUI("keyboard/reset.png", 0.8f, { -4.2f, -0.5f });
		LoadUI("controller/reset.png", 0.8f, { -4.2f, -0.15f });
		if (scene == Scene::Play) { break; }
		
		// ��������`���[�g���A���̐���UI
		// �ړ�
		LoadUI("keyboard/move.png", 1.0f, { 0.5f,0.5f }, true);
		LoadUI("controller/move.png", 1.0f, { 0.5f,0.5f }, true);
		// ���_�ړ�
		LoadUI("keyboard/cameraMove.png", 1.0f, { 0.5f,0.5f }, true);
		LoadUI("controller/cameraMove.png", 1.0f, { 0.5f,0.5f }, true);
		// �΂�ς���
		LoadUI("keyboard/lightChange.png", 1.0f, { 0.5f,0.5f }, true);
		LoadUI("controller/lightChange.png", 1.0f, { 0.5f,0.5f }, true);
		// �W�����v
		LoadUI("keyboard/jump.png", 1.0f, { 0.5f,0.0f }, true);
		LoadUI("controller/jump.png", 1.0f, { 0.5f,0.0f }, true);
		//�`���[�g���A���e�L�X�g
		LoadUI("tutorial/tutorial1.png", 1.0f, { 0.5f,0.5f }, true);	// �߂��̐C��𓔂���
		LoadUI("tutorial/tutorial2.png", 1.0f, { 0.5f,0.5f }, true);	// HP�������
		LoadUI("tutorial/tutorial3.png", 1.0f, { 0.5f,0.5f }, true);	// �V���ȐC��𓔂���
		LoadUI("tutorial/tutorial4.png", 1.0f, { 0.5f,0.5f }, true);	// �΂�؂�ւ���
		LoadUI("tutorial/tutorial5.png", 1.0f, { 0.5f,0.5f }, true);	// �΂̐F�ɂ���ĕς���
		LoadUI("tutorial/tutorial6.png", 1.0f, { 0.5f,0.5f }, true);	// �X�C�b�`���ړ����悤
		LoadUI("tutorial/tutorial7.png", 1.0f, { 0.5f,0.5f }, true);	// �X�C�b�`���͈��������Ȃ�
		LoadUI("tutorial/tutorial8.png", 1.0f, { 0.5f,0.5f }, true);	// ��֍s������
		LoadUI("tutorial/tutorial9.png", 1.0f, { 0.5f,0.5f }, true);	// �����擾���Ă݂悤
		LoadUI("tutorial/tutorial10.png", 1.0f, { 0.5f,0.5f }, true);	// ���̌��Ђ������
		LoadUI("tutorial/tutorial11.png", 1.0f, { 0.5f,0.5f }, true);	// �����g���Ă݂悤
		LoadUI("tutorial/tutorial12.png", 1.0f, { 0.5f,0.5f }, true);	// ���̃h�A���S�[����
	}
}

void UIDrawer::SceneUI::Update()
{
	for (auto& ui : uis) { ui->Update(); }
	for (auto& animationUI : animationUIs) { animationUI.Update(); }
}

void UIDrawer::SceneUI::Draw()
{
	for (auto& ui : uis) { ui->Draw(); }
	for (auto& animationUI : animationUIs) { animationUI.Draw(); }
}

void UIDrawer::LoadAll()
{
	sceneUIs[(size_t)Scene::Title - 1].SetScene(Scene::Title);
	sceneUIs[(size_t)Scene::Select - 1].SetScene(Scene::Select);
	sceneUIs[(size_t)Scene::Tutorial - 1].SetScene(Scene::Tutorial);
	sceneUIs[(size_t)Scene::Play - 1].SetScene(Scene::Play);
	for (auto& sceneUI : sceneUIs) { sceneUI.Load(); }
}

void UIDrawer::Update()
{
	scene = SceneManager::GetInstance()->GetNowScene();
	for (auto& sceneUI : sceneUIs) { if (sceneUI.GetScene() == scene) { sceneUI.Update(); } }
}

void UIDrawer::Draw()
{
	SpriteCommon::PreDraw();
	for (auto& sceneUI : sceneUIs) { if (sceneUI.GetScene() == scene) { sceneUI.Draw(); } }
	SpriteCommon::PostDraw();
}

Sprite* UIDrawer::GetUI(size_t index)
{
	scene = SceneManager::GetInstance()->GetNowScene();
	return sceneUIs[(size_t)scene - 1].GetUI(index);
}

void UIReset()
{
	// �C���UI
	// �`���ԏ�����
	size_t candleUIIndex = (size_t)UIType::Play::Candle;
	for (size_t i = candleUIIndex; i < candleUIIndex + 8; i++) { UIDrawer::GetUI(i)->SetIsInvisible(true); }
	for (size_t i = 0; i < Candle::GetLightNum(); i++)
	{
		Sprite* candleUI = nullptr;
		candleUI = UIDrawer::GetUI(candleUIIndex + i);
		candleUI->SetIsInvisible(false);
		candleUI->SetPosition({ candleUI->GetSize().x * i + 32,85 });
		candleUI->SetColor({ 1,1,1,0.5f });
	}
	// ����UI
	// �`���ԏ�����
	size_t keyUIIndex = (size_t)UIType::Play::Key;
	size_t keyPartsUIIndex = (size_t)UIType::Play::KeyParts;
	UIDrawer::GetUI(keyUIIndex)->SetIsInvisible(true);
	for (size_t i = keyPartsUIIndex; i < keyPartsUIIndex + 6; i++) { UIDrawer::GetUI(i)->SetIsInvisible(true); }
	for (size_t i = 0; i < KeyLock::GetKeyNum(); i++)
	{
		Sprite* keyUI = nullptr;
		if (KeyLock::GetKeyNum() == 1) { keyUI = UIDrawer::GetUI(keyUIIndex); }
		else { keyUI = UIDrawer::GetUI(keyPartsUIIndex + i); }
		keyUI->SetIsInvisible(false);
		keyUI->SetPosition({ keyUI->GetSize().x * i + 32, 165 });
		keyUI->SetColor({ 1,1,1,0.5f });
	}
}