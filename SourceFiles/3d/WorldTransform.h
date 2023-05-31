#pragma once
#include "Matrix4.h"
#include "WindowsAPI.h"
#include <wrl.h>
#include <d3d12.h>

class WorldTransform
{
private:
	// 定数バッファ用データ構造体
	struct ConstBufferData
	{
		Matrix4 world; // ワールド行列
	};

public:
	Matrix4 matWorld;
	ConstBufferData* constMap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer;
	Vector3 scale = { 1,1,1 }, rotation, translation;
	WorldTransform* parent = nullptr;

	void Initialize();
	void Update();
	Vector3 GetWorldPosition() { return { matWorld.m[3][0],matWorld.m[3][1],matWorld.m[3][2] }; }
};