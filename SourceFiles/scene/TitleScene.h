#pragma once
#include "BaseScene.h"

struct SelectUI
{
	Sprite* sprite;
	Vector2 pos;
	Vector2 offset;
	Angle lerpRateRad = 0;

	void Initialize();
	void Update();
	void SetSpritePos() { sprite->SetPosition(pos); }
};

class TitleScene : public BaseScene
{
private:
	void (TitleScene::* Select)() = &TitleScene::ToPlay;
	void ToPlay(); // プレイシーンへ
	void ToTutorial();
	Sprite* titleUI = nullptr;
	std::array<SelectUI, 2> selectUIs;

public:
	void Initialize();
	void Update();
	void Draw();
};