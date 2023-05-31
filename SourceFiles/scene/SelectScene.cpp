#include "SelectScene.h"

void SelectScene::Initialize()
{
	lightGroup = Model::GetLightGroup();
	Stage::SetStageNum(0);
	stage.Initialize();

	std::string stageName[] = { "1", "2", "3", "4" };
	for (size_t i = 0; i < stagePlanes.size(); i++)
	{
		stagePlanes[i].Initialize("Stage" + stageName[i] + ".png", { -30 + 20 * (float)i,7.5f,24 }, { 5,1,0.01f });
	}
}

void SelectScene::Update()
{
	debugCamera.Update();
	stage.Update();
}

void SelectScene::Draw()
{
	Model::PreDraw();
	stage.Draw();
	for (auto& stagePlane : stagePlanes) { stagePlane.Draw(); }
	Model::PostDraw();
}

void StagePlane::Initialize(const std::string& textureName, const Vector3& pos, const Vector3& scale)
{
	model = Model::Create("cube");
	model->SetAnbient({ 0.5f,0.5f,0.5f });
	std::unique_ptr<Sprite> texture = Sprite::Create("UI/stage/" + textureName);
	Vector2 textureSize = texture->GetSize();
	textureSize.x = -textureSize.x;
	texture->SetSize(textureSize);
	model->SetSprite(std::move(texture));
	worldTransform.Initialize();
	worldTransform.translation = pos;
	worldTransform.scale = scale;
	model->Update();
	worldTransform.Update();
}