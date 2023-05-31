#pragma once
#include "Input.h"
#include "UIDrawer.h"
#include "DebugCamera.h"
#include "Stage.h"
class SceneManager;

class BaseScene
{
protected:
	SceneManager* sceneManager_ = nullptr;
	Input* input = Input::GetInstance();
	DebugCamera debugCamera;
	Stage stage;

public:
	BaseScene();
	virtual ~BaseScene() = default;
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Finalize() {};
};