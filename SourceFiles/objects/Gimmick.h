#pragma once
#include "Player.h"
#include "ParticleManager.h"
#include <unordered_map>

// ギミックのパラメータ
struct GimmickParam {
	Vector3 pos;	// 座標
	Vector3 scale = { 1,1,1 };	// スケール
	Vector3 rot;	// 回転
	std::vector<Vector3> pathPoints;	// 経路点
	int interval = 200; // 経路点座標まで行ったら静止する時間
	UINT16 vanishFlag = 0;		// 消えるかフラグ
	bool moveFlag = false;		// 移動フラグ
	bool repeatFlag = true;		// 経路点の最後まで行ったら折り返すか
	UINT16 textureIndex = 0;	// テクスチャインデックス
	UINT16 modelIndex = 0;		// モデルインデックス
	UINT16 eventIndex = 0;		// イベントインデックス
	// 同イベントに所属している場合これをペアでtrueにすると
	// どれか一つでも条件を満たせばイベントが起こる
	bool isEither = false;
};

// イベントパラメータ
struct EventParam {
	bool isFlag = false;
	bool isEither = false;
	bool KorS = false; // falseなら鍵、trueならスイッチ
};

struct EventIterator
{
	UINT16 eventIndex = 0; // 連想配列のインデックス
	UINT16 paramIndex = 0; // EventParamのインデックス
};

class Gimmick : public BoxCollider
{
protected:
	static LightGroup* lightGroup;
	static std::unordered_map<UINT16, std::vector<EventParam>> events;
	std::unique_ptr<Model> model;
	bool isCameraCapture = true; // カメラに映る範囲内にあるか
	EventIterator eventItr = { 0,0 };	// イテレータ
	Input* input = Input::GetInstance();

public:
	virtual ~Gimmick() { model.release(); }
	virtual void Initialize(const GimmickParam& param);
	virtual void Update() = 0;
	virtual void Draw() { if (isCameraCapture) { model->Draw(worldTransform); } }
	void CheckIsCameraCapture(); // カメラに映る範囲内にあるかを調べる
	static bool CheckEventFlag(const UINT16 index);
	static void ResetEventParam() { events.clear(); }

	// アクセッサ
	Vector3 GetRotation() { return worldTransform.rotation; }
};

// ドアクラスの基底クラス
class BaseDoor : public Gimmick
{
protected:
	enum class WTType { L, R };

	// モデル
	std::unique_ptr<Model> model_back;
	std::array<WorldTransform, 2> door;
public:
	virtual void Initialize(const GimmickParam& param);
	virtual void Update();
	virtual void Draw();
	// プレイヤー以外のライトをオフに
	void CandleLightOff();

	//当たり判定の大きさを調整
	Vector3 GetRadius3D() { return { 1.8f,1.9f,1.0f }; }
};

class GoalDoor : public BaseDoor
{
protected:
	// ドアが閉じている時にnullptrになる
	void (GoalDoor::* Move)() = &GoalDoor::Closed;

	UINT16 doorIndex = 0;
	Input* input = Input::GetInstance();
	float rot = 0;

public:
	void Open();	// ドアが開く時に呼び出される関数
	virtual void Closed();	// ドアが閉じている時に呼び出される関数
	void Opened() {}	// ゴール判定に使用される空の関数
	void Initialize(const GimmickParam& param);
	void Update();
	void OnCollision(BoxCollider* boxCollider);
};

// ステージセレクト画面のドア
class SelectDoor : public GoalDoor
{
private:
	UINT16 doorIndex = 0;

	void Closed();	// ドアが閉じている時に呼び出される関数

public:
	SelectDoor(UINT16 doorIndex_) { doorIndex = doorIndex_; }
	void OnCollision(BoxCollider* boxCollider);
};

// ステージ2のドア
class RoomDoor : public BaseDoor
{
private:
	static UINT roomNum; // 現在の部屋番号(部屋にある燭台の数)
	static std::array<UINT, 3> allNextRoomNums; // 全てのドアが示す部屋番号
	static const UINT FINAL_ROOM_NUM = 5;
	UINT16 doorIndex = 0;
	UINT nextRoomNum = 0; // ドアが示す部屋番号
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
	// 当たり判定
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
	static size_t lightNum; // ステージにある燭台の数
	static size_t lightedNum; // 灯した数

	size_t lightIndex = 0;
	DiffuseParticle::AddProp particleProp;
	Vector3 lightPos;
	int particleTimer = 60; // 方向パーティクル発生時間
	Sprite* ui = nullptr;
	bool isExist = true;
	PlayerHealZone healZone;
	ParticleGroup* pParticleGroup = nullptr;
	Player* pPlayer = nullptr;

	void Dark(); // 光っていない時の処理
	void PreLight(); // 光る前
	void PostLight(); // 光った後
public:
	// 引数付きコンストラクタ
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
public: // 列挙クラス
	// ブロックのステータス
	enum class BlockStatus {
		NORMAL = 0b0000,
		MOVE = 0b0001,
		VANISH_RED = 0b0010,
		VANISH_BLUE = 0b0100,
		VANISH_KEY = 0b1000,
		REPEAT = 0b10000,
	};
private:
	// プレイヤー
	static Player* player;

	// ブロックの状態
	int blockState = (int)BlockStatus::NORMAL;
	// 移動関連
	bool isMove = false;
	float speed = 0.1f;
	Timer interval = 200;
	float timeRate = 0;
	std::vector<Vector3> pathPoints;
	int pathIndex = 0;
	bool isTurn = false; // 戻ってるか
	// イベント
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