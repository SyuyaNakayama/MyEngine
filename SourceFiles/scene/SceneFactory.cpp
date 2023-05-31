#include "SceneFactory.h"
#include "TitleScene.h"
#include "GamePlayScene.h"
#include "TutorialScene.h"
#include "SelectScene.h"
#include <cassert>

SceneFactory* SceneFactory::GetInstance()
{
	static SceneFactory instance;
	return &instance;
}

BaseScene* SceneFactory::CreateScene(Scene scene)
{
	BaseScene* newScene = nullptr;

	switch (scene)
	{
	case Scene::Null: return nullptr;
	case Scene::Title:      newScene = new TitleScene;		break;
	case Scene::Select:     newScene = new SelectScene;		break;
	case Scene::Tutorial:   newScene = new TutorialScene;	break;
	case Scene::Play:       newScene = new GamePlayScene;	break;
	}

	assert(newScene);
	return newScene;
}