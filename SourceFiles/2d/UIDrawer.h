#pragma once
#include <vector>
#include "SpriteAnimation.h"
#include "Scene.h"

namespace UIType
{
	// ステージ選択のUI
	enum class Select
	{
		Light, // 灯す
	};

	// チュートリアルのUI
	enum class Tutorial
	{
		Move = 26,
		CameraMove = 28,
		LightChange = 30, // ライト切り替え
		Jump = 32, // ジャンプ
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

	// プレイシーンのUI(チュートリアルシーンと兼用)
	enum class Play
	{
		PlayerGuage = 0, // HPゲージ
		PlayerGuageFrame = 2, // HPゲージの枠
		Candle = 3, // 燭台
		Key = 11, // 鍵
		KeyParts = 12, // 鍵パーツ
		Light = 18,
		Lever = 20, // レバー
		KeyOpen = 22, // 鍵を開ける
		Reset = 24, // リセット
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

void UIReset(); // 燭台と鍵のUIを消す