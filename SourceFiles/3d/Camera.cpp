#include "Camera.h"
#include "Input.h"
#include "ImGuiManager.h"
#include <imgui.h>
#include "Model.h"
#include <algorithm>
#include "SceneManager.h"

void Camera::Initialize(WorldTransform* parent)
{
	worldTransform.Initialize();
	Model::SetViewProjection(&viewProjection);
	worldTransform.translation = { 0,10.0f,-10.0f };
	viewProjection.farZ = 150.0f;
	worldTransform.parent = parent;
	viewProjection.Initialize();
	//collisionAttribute = CollisionAttribute::Player;
	//collisionMask = CollisionMask::Player;
	parentPrePos = parent->GetWorldPosition();
}

void Camera::Update()
{
	// �O�t���[�����W�擾
	prePos = worldTransform.GetWorldPosition();
	Vector2 cameraMove;

	Input* input = Input::GetInstance();
	if(!input->IsConnectGamePad())
	{
		cameraMove.x = Input::GetInstance()->Move(Key::Left, Key::Right, 10.0f);
		cameraMove.y = Input::GetInstance()->Move(Key::Up, Key::Down, 10.0f);
	}
	else
	{
		cameraMove = Input::GetInstance()->ConRStick(8.0f);
	}

	if (SceneManager::GetInstance()->GetNowScene() == Scene::Title)
	{
		cameraMove = {};
	}

	angle += cameraMove / 500.0f;
	angle.y = std::clamp(angle.y, -PI / 2.5f, PI / 2.5f);
	disToParent = std::clamp(disToParent, 0.1f, DISTANCE_MAX);
	if (disToParent < DISTANCE_MAX - 1.0e-4f)
	{
		float parentSpd = Length(parentPrePos - worldTransform.parent->GetWorldPosition());
		disToParent += parentSpd;
	}
	disToParent = std::clamp(disToParent, 0.1f, DISTANCE_MAX);
	Vector3 eyeVec = -Vector3::MakeAxis(Axis::Z) * Matrix4::Rotate({ angle.y,angle.x }) * disToParent;

	worldTransform.translation = eyeVec;
	worldTransform.Update();
	viewProjection.target = worldTransform.parent->GetWorldPosition();
	viewProjection.eye = worldTransform.GetWorldPosition();
	parentPrePos = worldTransform.parent->GetWorldPosition();
}

//void Camera::OnCollision(BoxCollider* collider)
//{
//	if (collider->GetCollisionAttribute() == CollisionAttribute::UI) { return; }
//	// ���ꂼ��̍��W�A���a�擾
//	Vector3 boxPos = collider->GetWorldPosition();
//	Vector3 boxRadius = collider->GetRadius3D();
//	Vector3 playerRadius = BoxCollider::GetRadius3D();
//
//	// �O�t���[���Ƃ̍��ŐN���������m�F����
//	if (prePos.y < boxPos.y - boxRadius.y) {
//		// �{�b�N�X���������ɉ����o��
//		worldTransform.translation.y = boxPos.y + boxRadius.y - playerRadius.y;
//		disToParent = Length(worldTransform.translation);
//		angle.y = std::atan2(worldTransform.translation.y, Length({ worldTransform.translation.x,worldTransform.translation.z }));
//		// �s��̍X�V
//		Update();
//		return;
//	}
//	else if (prePos.y > boxPos.y + boxRadius.y) {
//		// �{�b�N�X�����㑤�ɉ����o��
//		worldTransform.translation.y = boxPos.y + boxRadius.y + playerRadius.y;
//		disToParent = Length(worldTransform.translation);
//		angle.y = std::atan2(worldTransform.translation.y, Length({ worldTransform.translation.x,worldTransform.translation.z }));
//		// �s��̍X�V
//		Update();
//		return;
//	}
//	if (prePos.x < boxPos.x - boxRadius.x) {
//		// �{�b�N�X���������ɉ����o��
//		worldTransform.translation.x = boxRadius.x - playerRadius.x;
//	}
//	else if (prePos.x > boxPos.x + boxRadius.x) {
//		// �{�b�N�X�����E���ɉ����o��
//		worldTransform.translation.x = boxRadius.x + playerRadius.x;
//	}
//	if (prePos.z < boxPos.z - boxRadius.z) {
//		// �{�b�N�X������O���ɉ����o��
//		worldTransform.translation.z = boxRadius.z - playerRadius.z;
//	}
//	else if (prePos.z > boxPos.z + boxRadius.z) {
//		// �{�b�N�X���������ɉ����o��
//		worldTransform.translation.z = boxRadius.z + playerRadius.z;
//	}
//	disToParent = Length(worldTransform.translation);
//	Update();
//}
