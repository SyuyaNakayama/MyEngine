#include "Player.h"
#include "ImGuiManager.h"
#include "UIDrawer.h"
#include "Stage.h"
#include "SceneManager.h"
#include <imgui.h>
#include <algorithm>

int Player::maxHp = 0;

void Player::Initialize(const Vector3& startPos, const Vector3& startRot)
{
	// 当たり判定設定
	collisionAttribute = CollisionAttribute::Player;
	collisionMask = CollisionMask::Player;
	worldTransform.Initialize();
	worldTransform.translation = startPos;
	worldTransform.rotation = startRot * (PI / 180);
	input = Input::GetInstance();
	camera.Initialize(&worldTransform);

	lightGroup = Model::GetLightGroup();
	lightGroup->SetPointLightActive(0, true);
	lightGroup->SetPointLightAtten(0, { 0.2f,0.0f,0.001f });
	lightGroup->SetPointLightColor(0, { 1.0f,0.5f,0.5f });

	maxHp = 5000; // 最大HP
	// ステージ1の場合プレイヤーの最大HPを減らす
	if (Stage::GetStageNum() == (int)Stage::StageNum::Stage2) { maxHp /= 4; }
	hp = maxHp;
	// HPゲージ
	if (Stage::GetStageNum() != (int)Stage::StageNum::Select)
	{
		hpBackUI = UIDrawer::GetUI((size_t)UIType::Play::PlayerGuage);
		hpBackUI->SetColor({ 0,0,0,0.3f });
		hpBackUI->SetSize({ 640,32 });
		hpBackUI->SetPosition({ 43,36 });

		hpUI = UIDrawer::GetUI((size_t)UIType::Play::PlayerGuage + 1);
		hpUI->SetColor({ 1,0,0,1 });
		hpUI->SetPosition({ 43,36 });

		hpFrameUI = UIDrawer::GetUI((size_t)UIType::Play::PlayerGuageFrame);
		hpFrameUI->SetSize({ 690,64 * 0.8f });
		hpFrameUI->SetIsInvisible(false);
		hpFrameUI->SetPosition({ 18,26 });
	}

	jump.SetGravity(0.1f);
	jump.SetWorldTransform(&worldTransform);

	heal.Initialize(&worldTransform);
	heal.SetHpPointer(&hp);

	motion.Initialize(&worldTransform);
	sceneManager = SceneManager::GetInstance();
}

void Player::Move()
{
	if (sceneManager->GetNowScene() == Scene::Title) { return; }
	// 前フレーム座標取得
	prePos = worldTransform.translation;
	// 移動ベクトルを計算
	Vector3 move;
	if (!input->IsConnectGamePad())
	{
		move.z = input->Move(Key::W, Key::S, 1.0f);
		move.x = input->Move(Key::D, Key::A, 1.0f);
	}
	else
	{
		move.z = input->ConLStick(1).y;
		move.x = input->ConLStick(1).x;
	}

	// 移動している時
	if (move.Length() == 0) { return; } // 止まっている時
	move *= Matrix4::RotateY(camera.GetAngle().x + worldTransform.rotation.y);
	move.Normalize();
	// y軸回転を取り出す
	// 移動方向に合わせて回転する
	float bodyRotY = motion.GetBodyRotation().y;
	// 2Dベクトルの作成
	Vector2 forward =
	{
		// 向いてる方向
		std::cos(bodyRotY + PI / 2.0f + worldTransform.rotation.y),
		std::sin(bodyRotY + PI / 2.0f + worldTransform.rotation.y)
	};
	Vector2 move2D = { -move.x, move.z }; // 向かせたい方向
	float sign = Cross(forward, move2D) > 0 ? 1 : -1; // 2Dベクトルの左右判定
	float angle = std::acos(std::clamp(Dot(forward, move2D), -1.0f, 1.0f)) * sign; // 角度の差を計算
	if (angle != angle) { sceneManager->ChangeScene(Scene::Play); } // モーションがバグったら強制リトライ
	motion.SetBodyRotation({ 0,bodyRotY + angle * 0.4f }); // 回転の補間
	// 移動
	const float MOVE_SPD = 0.5f;
	move *= MOVE_SPD;
	worldTransform.translation += move;
}

void Player::RedFire()
{
	if (input->IsTrigger(Key::Q) || input->IsTrigger(JoyPad::L))
	{
		LightUpdate = &Player::BlueFire;
		lightGroup->SetPointLightColor(0, { 0.5f,0.5f,1 });
	}
	if (Stage::GetStageNum() != (int)Stage::StageNum::Select) { hp--; }
}

void Player::BlueFire()
{
	if (input->IsTrigger(Key::Q) || input->IsTrigger(JoyPad::L))
	{
		LightUpdate = &Player::RedFire;
		lightGroup->SetPointLightColor(0, { 1.0f,0.5f,0.5f });
	}
	if (Stage::GetStageNum() != (int)Stage::StageNum::Select) { hp -= 2; }
}

void Player::ObjectUpdate()
{
	// 行列の更新
	worldTransform.Update();
	motion.TransformUpdate();
	camera.Update();
	lightGroup->SetPointLightPos(0, worldTransform.GetWorldPosition());
}

void Player::Update()
{
	// ジャンプ
	if ((input->IsInput(Key::Space) || input->IsInput(JoyPad::B)) && sceneManager->GetNowScene() != Scene::Title) { jump.Start(1); }
	jump.Update();
	Move(); // 移動
	if (hpUI) { hpUI->SetSize({ (float)hp / maxHp * WindowsAPI::WIN_SIZE.x / 3.0f,32 }); } // HPゲージの調整
	(this->*LightUpdate)(); // ライト
	motion.MotionUpdate();
	ObjectUpdate(); // オブジェクトの更新
	heal.Update(); // 回復エリア更新
	baseRayDirection = Vector3::MakeAxis(Axis::Z) * Matrix4::RotateY(motion.GetBodyRotation().y);
	// 落ちるかHPが0になったら強制リトライ
	if (worldTransform.translation.y <= -20.0f || hp <= 0) { sceneManager->ChangeScene(Scene::Play); }
	// パーティクル
	TrackParticle::AddProp addProp =
	{
		&worldTransform,
		worldTransform.translation + Vector3(0,0.3f),
		{0,0.01f,0},{0,0.0005f,0},
		0.025f,0.001f,0,40,0.8f
	};
	ParticleManager::GetParticleGroup(IsBlueFire())->Add(addProp);

	//HPゲージの色を赤から青へ
	if (hpUI) { hpUI->SetColor({ (float)hp / maxHp, 0, (float)-hp / maxHp, 1 }); }
}

void Player::Draw()
{
	motion.Draw();
}

void Player::OnCollision(BoxCollider* boxCollider)
{
	if (boxCollider->GetCollisionAttribute() != CollisionAttribute::Block) { return; }
	// それぞれの座標、半径取得
	Vector3 boxPos = boxCollider->GetWorldPosition();
	Vector3 boxRadius = boxCollider->GetRadius3D();
	Vector3 playerRadius = BoxCollider::GetRadius3D();

	// 前フレームとの差で侵入方向を確認する
	if (prePos.y < boxPos.y - boxRadius.y) {
		// ボックスよりも下側に押し出す
		worldTransform.translation.y = boxPos.y - boxRadius.y - playerRadius.y;
	}
	else if (prePos.y > boxPos.y + boxRadius.y) {
		// ボックスよりも上側に押し出す
		worldTransform.translation.y = boxPos.y + boxRadius.y + playerRadius.y;
	}
	else if (prePos.x < boxPos.x - boxRadius.x) {
		// ボックスよりも左側に押し出す
		worldTransform.translation.x = boxPos.x - boxRadius.x - playerRadius.x;
	}
	else if (prePos.x > boxPos.x + boxRadius.x) {
		// ボックスよりも右側に押し出す
		worldTransform.translation.x = boxPos.x + boxRadius.x + playerRadius.x;
	}
	else if (prePos.z < boxPos.z - boxRadius.z) {
		// ボックスよりも手前側に押し出す
		worldTransform.translation.z = boxPos.z - boxRadius.z - playerRadius.z;
	}
	else if (prePos.z > boxPos.z + boxRadius.z) {
		// ボックスよりも奥側に押し出す
		worldTransform.translation.z = boxPos.z + boxRadius.z + playerRadius.z;
	}
	// 行列の更新
	ObjectUpdate();
}

void Heal::OnCollision(SphereCollider* sphereCollider)
{
	*hp += 10;
	*hp = min(*hp, Player::GetMaxHp() + 1);
}