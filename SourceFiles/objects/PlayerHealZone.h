#pragma once
#include "Collider.h"

// �v���C���[�̉񕜃G���A
class PlayerHealZone : public SphereCollider
{
public:
	void SetCollisionMask(CollisionMask collisionMask_) { collisionMask = collisionMask_; }
	void Initialize(WorldTransform* parent);
	void Update();
};