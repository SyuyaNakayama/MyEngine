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
		LoadUI("title/tutorial.png", 2.0f, { 0.5f,0.5f });	// タイトル
		LoadUI("title/select.png", 1.0f, { 0.5f,0.5f });	// ゲームプレイ
		LoadUI("title/tutorial.png", 0.9f, { 0.5f,0.5f });	// チュートリアル
		break;
	case Scene::Select:
		// 灯す
		LoadUI("keyboard/light.png", 0.8f, { 0.5f,0.5f }, true);
		LoadUI("controller/light.png", 0.8f, { 0.5f,0.5f }, true);
		break;
	case Scene::Tutorial:
	case Scene::Play:
		// プレイヤーのHPゲージの背景
		LoadUI("white1x1.png");
		LoadUI("white1x1.png");
		// プレイヤーのHPゲージの枠
		LoadUI("hpUI.png", 1.0f, { 0.0f,0.0f }, true);
		// 燭台のUI
		for (size_t i = 0; i < 8; i++) { LoadUI("CandleUI.png", objUISizeRate, {}, true); }
		// 鍵
		LoadUI("key/key.png", objUISizeRate, {}, true);
		// 鍵パーツ
		LoadUI("key/parts1.png", objUISizeRate * 2.0f, {}, true);
		LoadUI("key/parts2.png", objUISizeRate * 2.0f, {}, true);
		LoadUI("key/parts3.png", objUISizeRate * 2.0f, {}, true);
		LoadUI("key/parts4.png", objUISizeRate * 2.0f, {}, true);
		LoadUI("key/parts4.png", objUISizeRate * 2.0f, {}, true);
		LoadUI("key/parts5.png", objUISizeRate * 2.0f, {}, true);
		// 灯す
		LoadUI("keyboard/light.png", 0.8f, { 0.5f,0.5f }, true);
		LoadUI("controller/light.png", 0.8f, { 0.5f,0.5f }, true);
		// レバー切り替え
		LoadUI("keyboard/lever.png", 0.8f, { 0.5f,0.0f }, true);
		LoadUI("controller/lever.png", 0.8f, { 0.5f,0.0f }, true);
		// 鍵を開ける
		LoadUI("keyboard/keyOpen.png", 0.8f, { 0.5f,0.0f }, true);
		LoadUI("controller/keyOpen.png", 0.8f, { 0.5f,0.0f }, true);
		// リセットUI
		LoadUI("keyboard/reset.png", 0.8f, { -4.2f, -0.5f });
		LoadUI("controller/reset.png", 0.8f, { -4.2f, -0.15f });
		if (scene == Scene::Play) { break; }
		
		// ここからチュートリアルの説明UI
		// 移動
		LoadUI("keyboard/move.png", 1.0f, { 0.5f,0.5f }, true);
		LoadUI("controller/move.png", 1.0f, { 0.5f,0.5f }, true);
		// 視点移動
		LoadUI("keyboard/cameraMove.png", 1.0f, { 0.5f,0.5f }, true);
		LoadUI("controller/cameraMove.png", 1.0f, { 0.5f,0.5f }, true);
		// 火を変える
		LoadUI("keyboard/lightChange.png", 1.0f, { 0.5f,0.5f }, true);
		LoadUI("controller/lightChange.png", 1.0f, { 0.5f,0.5f }, true);
		// ジャンプ
		LoadUI("keyboard/jump.png", 1.0f, { 0.5f,0.0f }, true);
		LoadUI("controller/jump.png", 1.0f, { 0.5f,0.0f }, true);
		//チュートリアルテキスト
		LoadUI("tutorial/tutorial1.png", 1.0f, { 0.5f,0.5f }, true);	// 近くの燭台を灯そう
		LoadUI("tutorial/tutorial2.png", 1.0f, { 0.5f,0.5f }, true);	// HPが減るよ
		LoadUI("tutorial/tutorial3.png", 1.0f, { 0.5f,0.5f }, true);	// 新たな燭台を灯そう
		LoadUI("tutorial/tutorial4.png", 1.0f, { 0.5f,0.5f }, true);	// 火を切り替える
		LoadUI("tutorial/tutorial5.png", 1.0f, { 0.5f,0.5f }, true);	// 火の色によって変わるよ
		LoadUI("tutorial/tutorial6.png", 1.0f, { 0.5f,0.5f }, true);	// スイッチを移動しよう
		LoadUI("tutorial/tutorial7.png", 1.0f, { 0.5f,0.5f }, true);	// スイッチをは一つだけじゃない
		LoadUI("tutorial/tutorial8.png", 1.0f, { 0.5f,0.5f }, true);	// 上へ行きたい
		LoadUI("tutorial/tutorial9.png", 1.0f, { 0.5f,0.5f }, true);	// 鍵を取得してみよう
		LoadUI("tutorial/tutorial10.png", 1.0f, { 0.5f,0.5f }, true);	// 鍵の欠片もあるよ
		LoadUI("tutorial/tutorial11.png", 1.0f, { 0.5f,0.5f }, true);	// 鍵を使ってみよう
		LoadUI("tutorial/tutorial12.png", 1.0f, { 0.5f,0.5f }, true);	// あのドアがゴールだ
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
	// 燭台のUI
	// 描画状態初期化
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
	// 鍵のUI
	// 描画状態初期化
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