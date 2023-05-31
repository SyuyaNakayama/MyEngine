#include "Jump.h"
#include "Input.h"
#include <imgui.h>

void Jump::Start(float jumpV0)
{
	if (isJump) { return; }
	jumpSpd = jumpV0;
	isJump = true;
	collisionAttribute = CollisionAttribute::Player;
	collisionMask = CollisionMask::PlayerJump;
}

void Jump::Update()
{
	worldTransform = *pwt;
	if (!isJump) { return; }
	pwt->translation.y += jumpSpd;
	pwt->Update();
	jumpSpd -= gravity;
	// 落下速度の上限
	if (jumpSpd < 0) { jumpSpd = max(-2.0f, jumpSpd); }
}

// 自分の真下の地面を検出
void Jump::OnCollision(BoxCollider* collider)
{
	// 相手コライダーの上底のy座標を取得
	float pairPosY = collider->GetWorldPosition().y + collider->GetRadius3D().y;
	// 自座標と相手座標のy軸の差を計算
	float disY = std::abs(pwt->translation.y - pairPosY);
	// それが自分のy軸方向の半径未満なら着地している
	if (pwt->scale.y >= disY)
	{
		// 落ちてる状態じゃなければ無視
		if (jumpSpd >= 0 || !isJump) { return; }
		// ジャンプ状態解除
		pwt->translation.y = pairPosY + 1.0f;
		isJump = false;
	}
	else { Start(0); }
}
