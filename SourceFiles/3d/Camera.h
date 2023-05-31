#pragma once
#include "Collider.h"
#include "ViewProjection.h"

// オブジェクトに追従するカメラ
class Camera //: public BoxCollider
{
private:
	const float DISTANCE_MAX = 20.0f;
	ViewProjection viewProjection;
	Vector2 angle = {0,0.3f}; // カメラ回転量
	Vector3 prePos,parentPrePos;
	float disToParent = 20.0f; // 親オブジェクトまでの距離
	WorldTransform worldTransform;

public:
	void Initialize(WorldTransform* parent);
	void Update();
	ViewProjection* GetViewProjection() { return &viewProjection; }
	const Vector2& GetAngle() { return angle; }
	//Vector3 GetRadius3D() { return { 0.5f,0.5f,0.5f }; }
	//void OnCollision(BoxCollider* collider);
};