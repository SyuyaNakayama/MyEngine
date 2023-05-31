#include "PlayerHealZone.h"

void PlayerHealZone::Initialize(WorldTransform* parent)
{
	worldTransform.Initialize();
	worldTransform.scale *= 6.0f; // �񕜃G���A�̑傫��
	worldTransform.parent = parent; // �e�|�C���^(Candle�N���X)
	// �����蔻��ݒ�
	collisionAttribute = CollisionAttribute::PlayerHeal;
	collisionMask = CollisionMask::PlayerHeal;
}

void PlayerHealZone::Update()
{
	worldTransform.Update();
}
