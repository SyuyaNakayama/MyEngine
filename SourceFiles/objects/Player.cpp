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
	// �����蔻��ݒ�
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

	maxHp = 5000; // �ő�HP
	// �X�e�[�W1�̏ꍇ�v���C���[�̍ő�HP�����炷
	if (Stage::GetStageNum() == (int)Stage::StageNum::Stage2) { maxHp /= 4; }
	hp = maxHp;
	// HP�Q�[�W
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
	// �O�t���[�����W�擾
	prePos = worldTransform.translation;
	// �ړ��x�N�g�����v�Z
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

	// �ړ����Ă��鎞
	if (move.Length() == 0) { return; } // �~�܂��Ă��鎞
	move *= Matrix4::RotateY(camera.GetAngle().x + worldTransform.rotation.y);
	move.Normalize();
	// y����]�����o��
	// �ړ������ɍ��킹�ĉ�]����
	float bodyRotY = motion.GetBodyRotation().y;
	// 2D�x�N�g���̍쐬
	Vector2 forward =
	{
		// �����Ă����
		std::cos(bodyRotY + PI / 2.0f + worldTransform.rotation.y),
		std::sin(bodyRotY + PI / 2.0f + worldTransform.rotation.y)
	};
	Vector2 move2D = { -move.x, move.z }; // ��������������
	float sign = Cross(forward, move2D) > 0 ? 1 : -1; // 2D�x�N�g���̍��E����
	float angle = std::acos(std::clamp(Dot(forward, move2D), -1.0f, 1.0f)) * sign; // �p�x�̍����v�Z
	if (angle != angle) { sceneManager->ChangeScene(Scene::Play); } // ���[�V�������o�O�����狭�����g���C
	motion.SetBodyRotation({ 0,bodyRotY + angle * 0.4f }); // ��]�̕��
	// �ړ�
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
	// �s��̍X�V
	worldTransform.Update();
	motion.TransformUpdate();
	camera.Update();
	lightGroup->SetPointLightPos(0, worldTransform.GetWorldPosition());
}

void Player::Update()
{
	// �W�����v
	if ((input->IsInput(Key::Space) || input->IsInput(JoyPad::B)) && sceneManager->GetNowScene() != Scene::Title) { jump.Start(1); }
	jump.Update();
	Move(); // �ړ�
	if (hpUI) { hpUI->SetSize({ (float)hp / maxHp * WindowsAPI::WIN_SIZE.x / 3.0f,32 }); } // HP�Q�[�W�̒���
	(this->*LightUpdate)(); // ���C�g
	motion.MotionUpdate();
	ObjectUpdate(); // �I�u�W�F�N�g�̍X�V
	heal.Update(); // �񕜃G���A�X�V
	baseRayDirection = Vector3::MakeAxis(Axis::Z) * Matrix4::RotateY(motion.GetBodyRotation().y);
	// �����邩HP��0�ɂȂ����狭�����g���C
	if (worldTransform.translation.y <= -20.0f || hp <= 0) { sceneManager->ChangeScene(Scene::Play); }
	// �p�[�e�B�N��
	TrackParticle::AddProp addProp =
	{
		&worldTransform,
		worldTransform.translation + Vector3(0,0.3f),
		{0,0.01f,0},{0,0.0005f,0},
		0.025f,0.001f,0,40,0.8f
	};
	ParticleManager::GetParticleGroup(IsBlueFire())->Add(addProp);

	//HP�Q�[�W�̐F��Ԃ����
	if (hpUI) { hpUI->SetColor({ (float)hp / maxHp, 0, (float)-hp / maxHp, 1 }); }
}

void Player::Draw()
{
	motion.Draw();
}

void Player::OnCollision(BoxCollider* boxCollider)
{
	if (boxCollider->GetCollisionAttribute() != CollisionAttribute::Block) { return; }
	// ���ꂼ��̍��W�A���a�擾
	Vector3 boxPos = boxCollider->GetWorldPosition();
	Vector3 boxRadius = boxCollider->GetRadius3D();
	Vector3 playerRadius = BoxCollider::GetRadius3D();

	// �O�t���[���Ƃ̍��ŐN���������m�F����
	if (prePos.y < boxPos.y - boxRadius.y) {
		// �{�b�N�X���������ɉ����o��
		worldTransform.translation.y = boxPos.y - boxRadius.y - playerRadius.y;
	}
	else if (prePos.y > boxPos.y + boxRadius.y) {
		// �{�b�N�X�����㑤�ɉ����o��
		worldTransform.translation.y = boxPos.y + boxRadius.y + playerRadius.y;
	}
	else if (prePos.x < boxPos.x - boxRadius.x) {
		// �{�b�N�X���������ɉ����o��
		worldTransform.translation.x = boxPos.x - boxRadius.x - playerRadius.x;
	}
	else if (prePos.x > boxPos.x + boxRadius.x) {
		// �{�b�N�X�����E���ɉ����o��
		worldTransform.translation.x = boxPos.x + boxRadius.x + playerRadius.x;
	}
	else if (prePos.z < boxPos.z - boxRadius.z) {
		// �{�b�N�X������O���ɉ����o��
		worldTransform.translation.z = boxPos.z - boxRadius.z - playerRadius.z;
	}
	else if (prePos.z > boxPos.z + boxRadius.z) {
		// �{�b�N�X���������ɉ����o��
		worldTransform.translation.z = boxPos.z + boxRadius.z + playerRadius.z;
	}
	// �s��̍X�V
	ObjectUpdate();
}

void Heal::OnCollision(SphereCollider* sphereCollider)
{
	*hp += 10;
	*hp = min(*hp, Player::GetMaxHp() + 1);
}