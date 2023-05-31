#pragma once
#include <unordered_map>
#include <imgui.h>
#include "Random.h"
#include "Gimmick.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "Stage.h"
#include "SceneManager.h"
#include "UIDrawer.h"

#pragma region 静的メンバ変数の初期化
// ギミック基底クラス
LightGroup* Gimmick::lightGroup = nullptr;
std::unordered_map<UINT16, std::vector<EventParam>> Gimmick::events;
// 燭台
size_t Candle::lightNum = 0;
size_t Candle::lightedNum = 0;
// 壁
Player* Block::player = nullptr;
// 鍵
size_t KeyLock::keyNum = 0;
size_t KeyLock::collectedKeyNum = 0;
// ステージ2のドア
UINT RoomDoor::roomNum = 1;
std::array<UINT, 3> RoomDoor::allNextRoomNums;
// スイッチ
size_t Switch::switchNum = 0;
#pragma endregion

#pragma region Gimmick
// 各派生クラス共通処理
void Gimmick::Initialize(const GimmickParam& param)
{
	// 当たり判定設定
	collisionAttribute = CollisionAttribute::Block;
	collisionMask = CollisionMask::Block;
	// worldTransform初期化とパラメータ代入
	worldTransform.Initialize();
	worldTransform.translation = param.pos;
	worldTransform.scale = param.scale;
	worldTransform.rotation = param.rot * (PI / 180);
}

void Gimmick::CheckIsCameraCapture()
{
	isCameraCapture = true;
	ViewProjection* vp = Model::GetViewProjection();
	// スケールがfarZ / 2.0f以上のときは判定しない
	for (size_t i = 0; i < 3; i++)
	{
		if (worldTransform.scale[i] < vp->farZ / 2.0f) { continue; }
		return;
	}
	// カメラ位置からオブジェクトまでの距離を計算
	float dis = Length(worldTransform.GetWorldPosition() - vp->eye);
	// 距離がfarZ以上のときは描画しない
	if (dis >= vp->farZ) { isCameraCapture = false; }
}

bool Gimmick::CheckEventFlag(const UINT16 index)
{
	const auto& thisEvents = events[index];
	std::vector<EventParam> eitherTrueEvents;

	// Eitherフラグがtrueのイベントの抽出
	for (const auto& event_ : thisEvents)
	{
		if (event_.isEither) { eitherTrueEvents.push_back(event_); }
	}
	// Eitherフラグがtrueのイベントが一つもないなら
	if (eitherTrueEvents.empty())
	{
		for (const auto& event_ : thisEvents)
		{
			if (!event_.isFlag) { return false; }
		}
		return true;
	}
	// Eitherフラグがtrueのイベントがあるなら
	else
	{
		for (const auto& event_ : eitherTrueEvents)
		{
			if (event_.isFlag) { return true; }
		}
		return false;
	}
}
#pragma endregion

#pragma region Door
void BaseDoor::Initialize(const GimmickParam& param)
{
	// パラメータセット
	Gimmick::Initialize(param);
	collisionAttribute = CollisionAttribute::Door;
	// モデル読み込み
	model = Model::Create("door", true);
	model_back = Model::Create("door_back");
	model->Update();
	// 各モデルのworldTransform初期化とモデルの位置調整
	worldTransform.translation += { 0, 1.35f };
	worldTransform.scale = { 1.8f,1.9f,2.0f };
	for (auto& w : door)
	{
		// 親子関係
		w.parent = &worldTransform;
		w.Initialize();
		// 大きさを調整
		w.scale = { 1.1f,1.05f,1.0f };
		// 開ける
		w.rotation.y = -PI / 2.0f;
	}
	// 座標を調整
	door[(int)WTType::L].translation += {-1.4f, -1.3f, 0.0f};
	door[(int)WTType::R].translation += { 1.4f, -1.3f, 0.0f};
}

void BaseDoor::Update()
{
	// ドアの更新処理
	CheckIsCameraCapture();
	worldTransform.Update();
	for (auto& w : door) { w.Update(); }
}

void BaseDoor::Draw()
{
	if (!isCameraCapture) { return; }
	model_back->Draw(worldTransform);
	for (auto& w : door) { model->Draw(w); }
}

void BaseDoor::CandleLightOff()
{
	for (size_t i = 1; i < LightGroup::POINT_LIGHT_NUM; i++)
	{
		lightGroup->SetPointLightActive(i, false);
	}
}

void GoalDoor::Initialize(const GimmickParam& param)
{
	BaseDoor::Initialize(param);
	// 閉める
	door[(int)WTType::L].rotation.y = PI;
	door[(int)WTType::R].rotation.y = 0;
}

void GoalDoor::Update()
{
	assert(Move);
	(this->*Move)(); // 扉を動かす
	BaseDoor::Update();
}

void GoalDoor::Open()
{
	// ドアを開く
	if (++rot >= 90) { Move = &GoalDoor::Opened; }

	door[(int)WTType::L].rotation.y = (rot + 180) * PI / 180;
	door[(int)WTType::R].rotation.y = -rot * PI / 180;
}

void GoalDoor::Closed()
{
	// ドアを開ける
	if (Candle::GetLightNum() == Candle::GetLightedNum()) { Move = &GoalDoor::Open; }
}

void GoalDoor::OnCollision(BoxCollider* boxCollider)
{
	// ドアに当たった時
	if (Move != &GoalDoor::Opened) { return; } // ドアが空いている時ゴール
	SceneManager::GetInstance()->ChangeScene(Scene::Select);
	Stage::SetStageNum(0);
	CandleLightOff();
}

void SelectDoor::Closed()
{
	// ライトがついている時
	if (lightGroup->GetPointLightActive(doorIndex)) { Move = &GoalDoor::Open; }
}

void SelectDoor::OnCollision(BoxCollider* boxCollider)
{
	if (Move != &GoalDoor::Opened) { return; } // ドアが空いている時
	Stage::SetStageNum(doorIndex + 1);
	SceneManager::GetInstance()->ChangeScene(Scene::Play);
	CandleLightOff();
}

void RoomDoor::Initialize(const GimmickParam& param)
{
	BaseDoor::Initialize(param);
	// 次の部屋番号の設定
	Random_Int rand(1, 5);

	// 乱数の重複を無くす
	bool isExistNextRoomNum = false;
	do
	{
		nextRoomNum = rand();
		for (size_t i = 0; i < (size_t)doorIndex - 1; i++)
		{
			// 乱数の値がリストに登録されていた場合
			if (allNextRoomNums[i] == nextRoomNum)
			{
				isExistNextRoomNum = true;
				break;
			}
			else { isExistNextRoomNum = false; }
		}
	} while (isExistNextRoomNum);

	// 3つ目のドアの時
	if (doorIndex >= allNextRoomNums.size())
	{
		// 次の部屋を示すドアがあるかチェック
		bool isExistRightNextRoomNum = false;
		for (UINT nextRoomNumTemp : allNextRoomNums)
		{
			if (nextRoomNumTemp == roomNum + 1)
			{
				isExistRightNextRoomNum = true;
				break;
			}
		}
		// なかった場合は次の部屋番号をセット
		if (!isExistRightNextRoomNum) { nextRoomNum = roomNum + 1; }
	}
	// 乱数を配列に登録
	allNextRoomNums[doorIndex - 1] = nextRoomNum;

	// 次の部屋番号を視覚化
	std::unique_ptr<Sprite> sprite = Sprite::Create("ui/candleui.png");
	candlePlaneObj.Initialize();
	candlePlaneObj.translation = worldTransform.translation;
	candlePlaneObj.translation.y += 7.5f;
	candlePlaneObj.translation.z -= 0.05f;
	candlePlaneObj.scale.x = (float)nextRoomNum;
	candlePlaneObj.scale.y = sprite->GetSize().y / sprite->GetSize().x;
	candlePlaneObj.scale.z = 0.0001f;
	candlePlaneModel = Model::Create("cube");
	candlePlaneModel->SetAnbient({ 1,1,1 });
	sprite->SetSize({ sprite->GetSize().x / (float)nextRoomNum ,sprite->GetSize().y });
	candlePlaneModel->SetSprite(std::move(sprite));
	candlePlaneModel->Update();
}

void RoomDoor::Update()
{
	// 最終部屋の場合は更新しない
	if (roomNum == FINAL_ROOM_NUM) { return; }
	BaseDoor::Update();
	candlePlaneObj.Update();
}

void RoomDoor::Draw()
{
	if (!isCameraCapture) { return; }
	BaseDoor::Draw();
	candlePlaneModel->Draw(candlePlaneObj);
}

void RoomDoor::OnCollision(BoxCollider* boxCollider)
{
	// 最終部屋の場合は当たり判定を取らない
	if (roomNum == FINAL_ROOM_NUM) { return; }

	SceneManager::GetInstance()->ChangeScene(Scene::Play);
	// 正解のドアだった場合、roomNumをインクリメント
	if (nextRoomNum == roomNum + 1) { roomNum++; }
	// 不正解のドアだった場合、スタートの部屋に戻す
	else { roomNum = 1; }
	CandleLightOff();
}
#pragma endregion

#pragma region KeyLock
void KeyLock::Initialize(const GimmickParam& param)
{
	// モデル読み込み
	switch (param.modelIndex)
	{
	case 0:	model = Model::Create("key", true);	break;
	default:
		model = Model::Create("keyParts" + std::to_string(param.modelIndex), true);
		break;
	}
	modelIndex = param.modelIndex;
	// パラメータセット
	Gimmick::Initialize(param);
	EventParam key;
	key.isEither = param.isEither;
	key.KorS = false;
	// コンテナにプッシュ
	if (param.eventIndex != 0) { events[param.eventIndex].push_back(key); }
	// イテレータをセット
	eventItr = { param.eventIndex ,(UINT16)keyNum };
	// インクリメント
	keyNum++;
}

// 更新
void KeyLock::Update()
{
	// 鍵合体演出
	if (keyNum != 1 && keyNum == collectedKeyNum &&
		!UIDrawer::GetUI((size_t)UIType::Play::KeyParts)->GetIsInvisible())
	{
		std::array<Sprite*, 6> keyUIs{};
		for (size_t i = 0; i < keyNum; i++)
		{
			keyUIs[i] = UIDrawer::GetUI((size_t)UIType::Play::KeyParts + i);
			Vector2 newPos = keyUIs[i]->GetPosition();
			newPos.x -= 32.0f;
			newPos.x *= 0.99f;
			newPos.x += 32.0f;
			keyUIs[i]->SetPosition(newPos);
			// UIを鍵のものにする
			if (i != 5) { continue; }
			if (newPos.x > 35.0f) { continue; }
			for (auto& keyUI : keyUIs) { keyUI->SetIsInvisible(true); }
			Sprite* keyUI = UIDrawer::GetUI((size_t)UIType::Play::Key);
			keyUI->SetIsInvisible(false);
			keyUI->SetPosition({ 32, 165 });
		}
	}

	rot++;
	if (rot >= 360) { rot = 0; }

	// 更新
	worldTransform.rotation.y = rot * PI / 180;
	worldTransform.Update();
}

void KeyLock::Draw()
{
	// まだ取得されてないなら描画する
	if (!events[eventItr.eventIndex][eventItr.paramIndex].isFlag) { Gimmick::Draw(); }
}

void KeyLock::OnCollision(BoxCollider* boxCollider)
{
	// フラグをオンに
	events[eventItr.eventIndex][eventItr.paramIndex].isFlag = true;
	// 当たり判定をなくす
	collisionMask = CollisionMask::None;
	collectedKeyNum++;
	// UIカラー変更
	if (keyNum == 1) { UIDrawer::GetUI((size_t)UIType::Play::Key)->SetColor({ 1,1,1,1 }); }
	else { UIDrawer::GetUI((size_t)UIType::Play::KeyParts + modelIndex - 1)->SetColor({ 1,1,1,1 }); }
}
#pragma endregion

#pragma region Candle
void Candle::Initialize(const GimmickParam& param)
{
	// パラメータセット
	Gimmick::Initialize(param);
	model = Model::Create("candle", true);
	worldTransform.translation.y -= 1.0f;
	worldTransform.scale = { 2.0f,2.0f,2.0f };
	lightGroup = Model::GetLightGroup();
	lightPos = worldTransform.translation + Vector3(0, worldTransform.scale.y + 1.2f);
	particleProp =
	{
		lightPos,{0,0.02f,0},{0,0.0005f,0},
		0.025f,0.001f,0,30,0.4f
	};
	lightGroup->SetPointLightAtten(lightIndex, { 0.2f, 0.01f });
	lightGroup->SetPointLightColor(lightIndex, { 1,0.5f,0.5f });
	size_t uiIndex = -1;
	if (SceneManager::GetInstance()->GetNowScene() == Scene::Select) { uiIndex = (size_t)UIType::Select::Light; }
	else if (SceneManager::GetInstance()->GetNowScene() != Scene::Title) { uiIndex = (size_t)UIType::Play::Light; }
	if (uiIndex != -1) { ui = UIDrawer::GetUI(uiIndex + input->IsConnectGamePad()); }
	healZone.Initialize(&worldTransform);
	pParticleGroup = ParticleManager::GetParticleGroup(0);
	// 当たり判定を無くす
	healZone.SetCollisionMask(CollisionMask::None);
}

void Candle::Update()
{
	CheckIsCameraCapture();

	// ステージ1の場合
	if (Stage::GetStageNum() == (UINT)Stage::StageNum::Stage2)
	{
		// 現在の部屋番号以下のインデックスの場合出現する
		isExist = lightIndex <= RoomDoor::GetRoomNumber();
	}
	if (!isExist) { return; }
	worldTransform.Update();
	(this->*Fire)();
	model->Update();
	if (ui) { ui->SetIsInvisible(true); }
	healZone.Update();
}

void Candle::Dark()
{
	lightGroup->SetPointLightActive(lightIndex, false);
	model->SetAnbient({ 0.1f,0.1f,0.1f });
}

void Candle::PreLight()
{
	// 状態変更
	if (--particleTimer <= 0)
	{
		Fire = &Candle::PostLight;
		lightGroup->SetPointLightActive(lightIndex, true); // 点灯
		model->SetAnbient({ 0.5f,0.25f,0.25f }); // マテリアル調整
		// パーティクル調整
		lightPos = worldTransform.translation + Vector3(0, worldTransform.scale.y + 1.2f);
		// 灯っている時のみ当たり判定を取る
		healZone.SetCollisionMask(CollisionMask::PlayerHeal);
	}
	// 乱数生成
	Random_Float randRadius(0, 2.0f), randAngle(-PI / 2.0f, PI / 2.0f);
	// パーティクル設定
	pPlayer = Block::GetPlayerAddress();
	DirectionalParticle::AddProp particleProp =
	{
		pPlayer->GetWorldPosition() + Vector3(0,0.3f),
		worldTransform.translation + Vector3(0, worldTransform.scale.y + 1.2f),
		0.5f,2,randAngle(),randRadius(),60
	};
	// パーティクル追加
	pParticleGroup->Add(particleProp);
}

void Candle::PostLight()
{
	Random_Float rndRadius(-0.25f, 0.25f);
	lightGroup->SetPointLightPos(lightIndex, lightPos + Vector3(rndRadius(), 0, rndRadius()));
	// パーティクル追加
	pParticleGroup->Add(particleProp);
}

void Candle::OnCollision(RayCollider* rayCollider)
{
	if (Length(rayCollider->GetWorldPosition() - worldTransform.GetWorldPosition()) >= 12.0f) { return; }
	if (!pPlayer)
	{
		Player* player = dynamic_cast<Player*>(rayCollider);
		if (player) { pPlayer = player; }
	}
	// 説明UI調整
	ui->SetIsInvisible(Fire != &Candle::Dark);
	ui->SetPosition(To2DVector(rayCollider->GetWorldPosition() + Vector3(0, -3, 0)));
	if (!isExist) { return; }
	if (!input->IsTrigger(Key::Lshift) && !input->IsTrigger(Key::Rshift) && !input->IsTrigger(JoyPad::A)) { return; }
	if (Fire != &Candle::Dark) { return; }
	Fire = &Candle::PreLight;
	particleTimer = 60;
	if (Stage::GetStageNum() == (int)Stage::StageNum::Select) { return; }
	UIDrawer::GetUI((size_t)UIType::Play::Candle + lightedNum)->SetColor({ 1,1,1,1 }); // UI色変え
	lightedNum++; // 灯した数を増やす
}
#pragma endregion

#pragma region Block
void Block::Initialize(const GimmickParam& param)
{
	// テクスチャ読み込み
	std::unique_ptr<Sprite> sprite;
	switch (param.textureIndex)
	{
	case 0:	sprite = Sprite::Create("stages/wall.png");		break;
	case 1:	sprite = Sprite::Create("stages/floor.png");	break;
	}
	sprite->SetSize(sprite->GetSize() / max(max(param.scale.x, param.scale.y), param.scale.z) * 10.0f);
	// モデル読み込み
	if (param.modelIndex == 1) { model = Model::Create("elevator"); }
	else
	{
		model = Model::Create("cube");
		model->SetSprite(std::move(sprite));
		model->Update();
	}
	if (param.vanishFlag == 3) { model = Model::Create("keyDoor"); }
	// パラメータセット
	Gimmick::Initialize(param);
	if (param.vanishFlag == 1) { blockState |= (int)BlockStatus::VANISH_RED; }			// 赤炎の時消えるフラグ
	else if (param.vanishFlag == 2) { blockState |= (int)BlockStatus::VANISH_BLUE; }	// 青炎の時消えるフラグ
	else if (param.vanishFlag == 3) { blockState |= (int)BlockStatus::VANISH_KEY; }	// 鍵を持った時消えるフラグ
	if (param.moveFlag) { blockState |= (int)BlockStatus::MOVE; isMove = true; }
	if (param.repeatFlag) { blockState |= (int)BlockStatus::REPEAT; }
	// 動くかどうか
	if (!param.pathPoints.empty())
	{
		pathPoints.push_back(param.pos); // 始点は初期座標
		for (auto& pathPoint : param.pathPoints) { pathPoints.push_back(pathPoint); } // 経路点取得
		interval = param.interval;
	}
	if (param.eventIndex != 0) { eventIndex = param.eventIndex; isMove = false; }
}

void Block::Update()
{
	CheckIsCameraCapture();
	// 当たり判定設定
	if ((blockState & (int)BlockStatus::VANISH_RED) && player->IsRedFire()) { collisionMask = CollisionMask::None; }
	else if ((blockState & (int)BlockStatus::VANISH_BLUE) && player->IsBlueFire()) { collisionMask = CollisionMask::None; }
	else if (blockState & (int)BlockStatus::VANISH_KEY) {
		if (collisionMask != CollisionMask::None) { collisionMask = CollisionMask::Block; }
	}
	else { collisionMask = CollisionMask::Block; }
	// 移動
	if (blockState & (int)BlockStatus::MOVE) { isMove = CheckEventFlag(eventIndex); }
	if (blockState & (int)BlockStatus::MOVE && isMove == true) { Move(); }
	// 更新
	worldTransform.Update();
	if (blockState & (int)BlockStatus::VANISH_KEY)
	{
		UIDrawer::GetUI((size_t)UIType::Play::KeyOpen + input->IsConnectGamePad())->SetIsInvisible(true);
	}
}

void Block::Draw()
{
	if (!isCameraCapture) { return; }
	// 当たり判定がないなら描画しない
	if (collisionMask == CollisionMask::None) { return; }
	// あるなら描画
	else { Gimmick::Draw(); }
}

void Block::Move()
{
	// 次のインデックスを返す関数
	auto nextPathIndex = [&] { return std::clamp(pathIndex + (isTurn ? -1 : 1), 0, (int)pathPoints.size() - 1); };

	// インターバル中ならスルー
	if (timeRate >= 1.0f)
	{
		if (!interval.CountDown()) { return; }
		pathIndex = nextPathIndex();
		if (blockState & (int)BlockStatus::REPEAT)
		{
			if (pathIndex >= pathPoints.size() - 1) { isTurn = true; }
			if (pathIndex < 1) { isTurn = false; }
		}
		timeRate = 0;
	}
	// 始点から終点の距離
	Vector3 start = pathPoints[pathIndex];
	Vector3 end = pathPoints[nextPathIndex()];
	Vector3 vec = start - end;
	timeRate += 0.3f / vec.Length();
	// 移動(線形補間)
	worldTransform.translation = Lerp(start, end, min(timeRate, 1.0f));
}

void Block::OnCollision(BoxCollider* boxCollider)
{
	// 鍵ドアの処理
	if (!(blockState & (int)BlockStatus::VANISH_KEY)) { return; } // 鍵ドアじゃない時
	if (!CheckEventFlag(eventIndex)) { return; }
	Sprite* ui = UIDrawer::GetUI((size_t)UIType::Play::KeyOpen + input->IsConnectGamePad());
	ui->SetIsInvisible(false);
	ui->SetPosition(To2DVector(boxCollider->GetWorldPosition() + Vector3(0, -6, 0)));
	// Shiftキーを押してない時
	if (!input->IsTrigger(Key::Lshift) && !input->IsTrigger(Key::Rshift) && !input->IsTrigger(JoyPad::A)) { return; }
	collisionMask = CollisionMask::None;
}
#pragma endregion

#pragma region Switch
void Switch::Initialize(const GimmickParam& param)
{
	// モデル読み込み
	model = Model::Create("switch_table");
	model_lever = Model::Create("switch_lever", true);
	// パラメータセット
	Gimmick::Initialize(param);
	wo2.parent = &worldTransform;
	wo2.Initialize();
	EventParam sw;
	sw.isEither = param.isEither;
	sw.KorS = true;
	// コンテナにプッシュ
	if (param.eventIndex != 0) { events[param.eventIndex].push_back(sw); }
	// イテレータをセット
	eventItr = { param.eventIndex,(UINT16)(events[param.eventIndex].size() - 1) };
	// インクリメント
	switchNum++;
	// UI取得
	ui = UIDrawer::GetUI((size_t)UIType::Play::Lever + input->IsConnectGamePad());
}

void Switch::Update()
{
	if (!events[eventItr.eventIndex][eventItr.paramIndex].isFlag) { wo2.rotation.z = 30 * PI / 180; }
	else { wo2.rotation.z = -30 * PI / 180; }

	// 更新
	worldTransform.Update();
	wo2.Update();
	ui->SetIsInvisible(true);
}

void Switch::Draw()
{
	model_lever->Draw(wo2);
	Gimmick::Draw();
}

void Switch::OnCollision(RayCollider* rayCollider)
{
	if (Length(rayCollider->GetWorldPosition() - worldTransform.GetWorldPosition()) >= 8.0f) { return; }
	if (!events[eventItr.eventIndex][eventItr.paramIndex].isFlag)
	{
		ui->SetIsInvisible(false);
		ui->SetPosition(To2DVector(rayCollider->GetWorldPosition() + Vector3(0, -3, 0)));
	}
	if (!input->IsTrigger(Key::Lshift) && !input->IsTrigger(Key::Rshift) && !input->IsTrigger(JoyPad::A)) { return; }
	events[eventItr.eventIndex][eventItr.paramIndex].isFlag = true;
}
#pragma endregion