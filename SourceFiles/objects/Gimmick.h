#pragma once
#include "Player.h"
#include "ParticleManager.h"
#include <unordered_map>

// �M�~�b�N�̃p�����[�^
struct GimmickParam {
	Vector3 pos;	// ���W
	Vector3 scale = { 1,1,1 };	// �X�P�[��
	Vector3 rot;	// ��]
	std::vector<Vector3> pathPoints;	// �o�H�_
	int interval = 200; // �o�H�_���W�܂ōs������Î~���鎞��
	UINT16 vanishFlag = 0;		// �����邩�t���O
	bool moveFlag = false;		// �ړ��t���O
	bool repeatFlag = true;		// �o�H�_�̍Ō�܂ōs������܂�Ԃ���
	UINT16 textureIndex = 0;	// �e�N�X�`���C���f�b�N�X
	UINT16 modelIndex = 0;		// ���f���C���f�b�N�X
	UINT16 eventIndex = 0;		// �C�x���g�C���f�b�N�X
	// ���C�x���g�ɏ������Ă���ꍇ������y�A��true�ɂ����
	// �ǂꂩ��ł������𖞂����΃C�x���g���N����
	bool isEither = false;
};

// �C�x���g�p�����[�^
struct EventParam {
	bool isFlag = false;
	bool isEither = false;
	bool KorS = false; // false�Ȃ献�Atrue�Ȃ�X�C�b�`
};

struct EventIterator
{
	UINT16 eventIndex = 0; // �A�z�z��̃C���f�b�N�X
	UINT16 paramIndex = 0; // EventParam�̃C���f�b�N�X
};

class Gimmick : public BoxCollider
{
protected:
	static LightGroup* lightGroup;
	static std::unordered_map<UINT16, std::vector<EventParam>> events;
	std::unique_ptr<Model> model;
	bool isCameraCapture = true; // �J�����ɉf��͈͓��ɂ��邩
	EventIterator eventItr = { 0,0 };	// �C�e���[�^
	Input* input = Input::GetInstance();

public:
	virtual ~Gimmick() { model.release(); }
	virtual void Initialize(const GimmickParam& param);
	virtual void Update() = 0;
	virtual void Draw() { if (isCameraCapture) { model->Draw(worldTransform); } }
	void CheckIsCameraCapture(); // �J�����ɉf��͈͓��ɂ��邩�𒲂ׂ�
	static bool CheckEventFlag(const UINT16 index);
	static void ResetEventParam() { events.clear(); }

	// �A�N�Z�b�T
	Vector3 GetRotation() { return worldTransform.rotation; }
};

// �h�A�N���X�̊��N���X
class BaseDoor : public Gimmick
{
protected:
	enum class WTType { L, R };

	// ���f��
	std::unique_ptr<Model> model_back;
	std::array<WorldTransform, 2> door;
public:
	virtual void Initialize(const GimmickParam& param);
	virtual void Update();
	virtual void Draw();
	// �v���C���[�ȊO�̃��C�g���I�t��
	void CandleLightOff();

	//�����蔻��̑傫���𒲐�
	Vector3 GetRadius3D() { return { 1.8f,1.9f,1.0f }; }
};

class GoalDoor : public BaseDoor
{
protected:
	// �h�A�����Ă��鎞��nullptr�ɂȂ�
	void (GoalDoor::* Move)() = &GoalDoor::Closed;

	UINT16 doorIndex = 0;
	Input* input = Input::GetInstance();
	float rot = 0;

public:
	void Open();	// �h�A���J�����ɌĂяo�����֐�
	virtual void Closed();	// �h�A�����Ă��鎞�ɌĂяo�����֐�
	void Opened() {}	// �S�[������Ɏg�p������̊֐�
	void Initialize(const GimmickParam& param);
	void Update();
	void OnCollision(BoxCollider* boxCollider);
};

// �X�e�[�W�Z���N�g��ʂ̃h�A
class SelectDoor : public GoalDoor
{
private:
	UINT16 doorIndex = 0;

	void Closed();	// �h�A�����Ă��鎞�ɌĂяo�����֐�

public:
	SelectDoor(UINT16 doorIndex_) { doorIndex = doorIndex_; }
	void OnCollision(BoxCollider* boxCollider);
};

// �X�e�[�W2�̃h�A
class RoomDoor : public BaseDoor
{
private:
	static UINT roomNum; // ���݂̕����ԍ�(�����ɂ���C��̐�)
	static std::array<UINT, 3> allNextRoomNums; // �S�Ẵh�A�����������ԍ�
	static const UINT FINAL_ROOM_NUM = 5;
	UINT16 doorIndex = 0;
	UINT nextRoomNum = 0; // �h�A�����������ԍ�
	std::unique_ptr<Model> candlePlaneModel;
	WorldTransform candlePlaneObj;

public:
	RoomDoor(UINT16 doorIndex_) { doorIndex = doorIndex_; }
	static UINT GetRoomNumber() { return roomNum; }
	void Initialize(const GimmickParam& param);
	void Update();
	void Draw();
	void OnCollision(BoxCollider* boxCollider);
};

class KeyLock : public Gimmick
{
private:
	float rot = 0.0f;
	// �����蔻��
	void OnCollision(BoxCollider* boxCollider);

public:
	static size_t keyNum;
	static size_t collectedKeyNum;
	UINT16 modelIndex = 0;

	void Initialize(const GimmickParam& param);
	void Update();
	void Draw() override;
	static size_t GetKeyNum() { return keyNum; }
	static size_t GetCollectedKeyNum() { return collectedKeyNum; }
	static void ResetKeyNum() { keyNum = collectedKeyNum = 0; }
};

class Candle : public Gimmick, public SphereCollider
{
private:
	void (Candle::* Fire)() = &Candle::Dark;
	static size_t lightNum; // �X�e�[�W�ɂ���C��̐�
	static size_t lightedNum; // ��������

	size_t lightIndex = 0;
	DiffuseParticle::AddProp particleProp;
	Vector3 lightPos;
	int particleTimer = 60; // �����p�[�e�B�N����������
	Sprite* ui = nullptr;
	bool isExist = true;
	PlayerHealZone healZone;
	ParticleGroup* pParticleGroup = nullptr;
	Player* pPlayer = nullptr;

	void Dark(); // �����Ă��Ȃ����̏���
	void PreLight(); // ����O
	void PostLight(); // ��������
public:
	// �����t���R���X�g���N�^
	Candle(size_t index) { lightIndex = index; lightNum++; }
	void OnCollision(RayCollider* rayCollider);
	void Initialize(const GimmickParam& param);
	void Update();
	static size_t GetLightNum() { return lightNum; }
	static size_t GetLightedNum() { return lightedNum; }
	static void Reset() { lightNum = lightedNum = 0; }
	Vector3 GetRadius3D()
	{
		Vector3 rad = worldTransform.scale;
		rad.x *= 0.5f; rad.z *= 0.5f; rad.y *= 4.0f;
		return rad;
	}
	float GetRadius() { return worldTransform.scale.x * 2.0f; }
};

class Block : public Gimmick
{
public: // �񋓃N���X
	// �u���b�N�̃X�e�[�^�X
	enum class BlockStatus {
		NORMAL = 0b0000,
		MOVE = 0b0001,
		VANISH_RED = 0b0010,
		VANISH_BLUE = 0b0100,
		VANISH_KEY = 0b1000,
		REPEAT = 0b10000,
	};
private:
	// �v���C���[
	static Player* player;

	// �u���b�N�̏��
	int blockState = (int)BlockStatus::NORMAL;
	// �ړ��֘A
	bool isMove = false;
	float speed = 0.1f;
	Timer interval = 200;
	float timeRate = 0;
	std::vector<Vector3> pathPoints;
	int pathIndex = 0;
	bool isTurn = false; // �߂��Ă邩
	// �C�x���g
	UINT16 eventIndex = 0;

public:
	static void SetPlayerAddress(Player* pPlayer) { player = pPlayer; }
	void Initialize(const GimmickParam& param);
	void Update();
	void Draw() override;
	void Move();
	void OnCollision(BoxCollider* boxCollider);
	static Player* GetPlayerAddress() { return player; }
};

class Switch : public Gimmick, public SphereCollider
{
private:
	WorldTransform wo2;
	std::unique_ptr<Model> model_lever;
	Sprite* ui = nullptr;

public:
	static size_t switchNum;

	void Initialize(const GimmickParam& param);
	void Update();
	void Draw() override;
	void OnCollision(RayCollider* rayCollider);
	static void ResetSwitchNum() { switchNum = 0; }
	float GetRadius() { return worldTransform.scale.x * 2.0f; }
};