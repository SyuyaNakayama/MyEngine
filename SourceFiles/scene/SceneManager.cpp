#include "SceneManager.h"
#include "Sprite.h"
#include "CollisionManager.h"

BaseScene::BaseScene() { sceneManager = SceneManager::GetInstance(); }

SceneManager* SceneManager::GetInstance()
{
	static SceneManager sceneManager;
	return &sceneManager;
}

void SceneManager::Initialize()
{
	fadeManager_.Initialize();
}

void SceneManager::Update()
{
	fadeManager_.Update();

	bool isChangeScene = fadeManager_.IsChange() || !fadeManager_.IsFade();
	isChangeScene &= nextScene_ != Scene::Null;
	if (isChangeScene)
	{
		if (scene_)
		{
			scene_->Finalize();
			delete scene_;
		}

		scene_ = sceneFactory_->CreateScene(nextScene_);
		nowScene_ = nextScene_;
		nextScene_ = Scene::Null;
		scene_->Initialize();
	}

	if (!fadeManager_.IsFade())
	{
		scene_->Update();
		CollisionManager::CheckAllCollisions();
	}
}

void SceneManager::Draw()
{
	scene_->Draw();
	if (!fadeManager_.IsFade()) { return; }
	Sprite::PreDraw();
	fadeManager_.Draw();
	Sprite::PostDraw();
}

void SceneManager::ChangeScene(Scene nextScene, bool isUseFade)
{
	nextScene_ = nextScene;
	if (isUseFade) { fadeManager_.FadeScene(); }
}
