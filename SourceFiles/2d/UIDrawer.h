#pragma once
#include <vector>
#include "SpriteAnimation.h"
#include "Scene.h"

namespace UIType
{
	// �X�e�[�W�I����UI
	enum class Select
	{
		Light, // ����
	};

	// �`���[�g���A����UI
	enum class Tutorial
	{
		Move = 26,
		CameraMove = 28,
		LightChange = 30, // ���C�g�؂�ւ�
		Jump = 32, // �W�����v
		TutorialText1 = 34,
		TutorialText2,
		TutorialText3,
		TutorialText4,
		TutorialText5,
		TutorialText6,
		TutorialText7,
		TutorialText8,
		TutorialText9,
		TutorialText10,
		TutorialText11,
		TutorialText12,
	};

	// �v���C�V�[����UI(�`���[�g���A���V�[���ƌ��p)
	enum class Play
	{
		PlayerGuage = 0, // HP�Q�[�W
		PlayerGuageFrame = 2, // HP�Q�[�W�̘g
		Candle = 3, // �C��
		Key = 11, // ��
		KeyParts = 12, // ���p�[�c
		Light = 18,
		Lever = 20, // ���o�[
		KeyOpen = 22, // �����J����
		Reset = 24, // ���Z�b�g
	};
}

class UIDrawer
{
private:
	class SceneUI
	{
	private:
		std::vector<std::unique_ptr<Sprite>> uis;
		std::vector<SpriteAnimation> animationUIs;
		Scene scene = Scene::Null;

		void LoadUI(const std::string& fileName, float sizeRate = 1.0f, Vector2 anchorPoint = {}, bool isInvisible = false);

	public:
		void SetScene(Scene nowScene) { scene = nowScene; }
		Scene GetScene() { return scene; }
		void Load();
		void Update();
		void Draw();
		Sprite* GetUI(size_t index) { return uis[index].get(); }
	};

	static Scene scene;
	static std::array<SceneUI, (size_t)Scene::SceneNum - 1> sceneUIs;

public:
	UIDrawer() = delete;
	~UIDrawer() = delete;

	static void LoadAll();
	static void Update();
	static void Draw();
	static Sprite* GetUI(size_t index);
};

void UIReset(); // �C��ƌ���UI������