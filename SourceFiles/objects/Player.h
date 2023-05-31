#pragma once
#include "Camera.h"
#include "Jump.h"
#include "PlayerHealZone.h"
#include "Motion.h"

class SceneManager;

class Heal : public PlayerHealZone
{
	int* hp = nullptr;
public:
	void SetHpPointer(int* hp_) { hp = hp_; }
	void OnCollision(SphereCollider* sphereCollider);
};

class Player : public BoxCollider, public RayCollider
{
private:
	static int maxHp;
	Input* input = nullptr;
	Camera camera;
	LightGroup* lightGroup = nullptr; // ���C�g
	Vector3 prePos; // �O�t���[�����W
	int hp = 0;
	Jump jump;
	Sprite* hpUI = nullptr;
	Sprite* hpFrameUI = nullptr;
	Sprite* hpBackUI = nullptr;
	Heal heal; // �񕜃G���A
	PlayerMotion motion;
	SceneManager* sceneManager = nullptr;

	// �ړ�
	void Move();
	// ���C�g����
	void (Player::* LightUpdate)() = &Player::RedFire;
	void RedFire();
	void BlueFire();
	// �\�����̍X�V
	void ObjectUpdate();

public:
	void Initialize(const Vector3& startPos, const Vector3& startRot);
	void Update();
	void Draw();
	// �A�N�Z�b�T
	bool IsRedFire() { return LightUpdate == &Player::RedFire; }
	bool IsBlueFire() { return LightUpdate == &Player::BlueFire; } // ���Ȃ�true
	static int GetMaxHp() { return maxHp; }

	// �����蔻��̏���
	void OnCollision(BoxCollider* boxCollider) override;
};