#pragma once
#include "Collider.h"

// プレイヤーの回復エリア
class PlayerHealZone : public SphereCollider
{
public:
	void SetCollisionMask(CollisionMask collisionMask_) { collisionMask = collisionMask_; }
	void Initialize(WorldTransform* parent);
	void Update();
};