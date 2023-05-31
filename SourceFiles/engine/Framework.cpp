#include "Framework.h"
#include "SpriteCommon.h"

void Framework::Initialize()
{
	wAPI->Initialize(L"燈火の迷宮");
	dxCommon->Initialize();
	SpriteCommon::GetInstance()->Initialize();
	input->Initialize();
	sceneManager->Initialize();
}

void Framework::Update()
{
	input->Update();
	sceneManager->Update();
}

void Framework::Finalize()
{
	sceneManager->Finalize();
	wAPI->Finalize();
}

bool Framework::IsEndRequest()
{
	// Xボタンで終了メッセージが来たらゲームループを抜ける
	if (wAPI->ProcessMessage()) { return true; }
	// ESCキーを押したらゲームループを抜ける
	if (input->IsTrigger(Key::Escape)) { return true; }
	return false;
}

void Framework::Run()
{
	Initialize();

	while (!IsEndRequest())
	{
		Update();
		Draw();
	}

	Finalize();
}