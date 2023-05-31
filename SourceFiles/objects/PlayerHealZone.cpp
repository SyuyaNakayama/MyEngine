#include "PlayerHealZone.h"

void PlayerHealZone::Initialize(WorldTransform* parent)
{
	worldTransform.Initialize();
	worldTransform.scale *= 6.0f; // 回復エリアの大きさ
	worldTransform.parent = parent; // 親ポインタ(Candleクラス)
	// 当たり判定設定
	collisionAttribute = CollisionAttribute::PlayerHeal;
	collisionMask = CollisionMask::PlayerHeal;
}

void PlayerHealZone::Update()
{
	worldTransform.Update();
}
