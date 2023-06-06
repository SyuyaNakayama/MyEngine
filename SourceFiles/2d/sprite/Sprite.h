#pragma once
#include <array>
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <memory>
#include "Matrix4.h"
#include "Color.h"

struct Sprite
{
	enum class VertexNumber
	{
		LB, // 左下
		LT, // 左上
		RB, // 右下
		RT  // 右上
	};

	Vector2 position;
	float rotation = 0;
	ColorRGBA color;
	Vector2 size; // 表示サイズ
	Vector2 anchorPoint;
	bool isFlipX = false;
	bool isFlipY = false;
	bool isInvisible = false;
	Vector2 textureLeftTop; // 切り取り領域の左上座標
	Vector2 textureSize; // 切り取り領域のサイズ

private:
	struct Vertex { Vector2 pos, uv; };

	struct ConstBufferData
	{
		Matrix4 mat;
		ColorRGBA color;
	};

	const static Matrix4 matProj;
	std::array<Vertex, 4> vertices;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;
	ConstBufferData* constMap = nullptr;
	Vertex* vertMap = nullptr;
	uint32_t textureIndex = 0;
	Matrix4 matWorld;

	void AdjustTextureSize();
	void Initialize(uint32_t textureIndex);

public:
	static std::unique_ptr<Sprite> Create(const std::string& FILE_NAME);
	uint32_t GetTextureIndex() { return textureIndex; }
	void Update();
	void Draw();
};