#include "BaseCamera.h"

void BaseCamera::Initialize(WorldTransform* parent)
{
	worldTransform.Initialize();
	worldTransform.translation = { 0,10.0f,-10.0f };
	worldTransform.parent = parent;
	Model::SetViewProjection(&viewProjection);
	viewProjection.Initialize();
}

void BaseCamera::Update()
{
	Vector3 eyeVec = -Vector3::MakeAxis(Axis::Z) * Matrix4::Rotate({ angle.y,angle.x }) * distance;
	worldTransform.translation = eyeVec;
	worldTransform.Update();
	viewProjection.target = worldTransform.parent->GetWorldPosition();
	viewProjection.eye = worldTransform.GetWorldPosition();
}