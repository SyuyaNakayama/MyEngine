#pragma once
#include <array>
#include <memory>
#include "Color.h"
#include "SpriteCommon.h"

struct Sprite
{
	enum class VertexNumber
	{
		LB, // ����
		LT, // ����
		RB, // �E��
		RT  // �E��
	};

	Vector2 position;
	float rotation = 0;
	ColorRGBA color;
	Vector2 size; // �\���T�C�Y
	Vector2 anchorPoint;
	bool isFlipX = false;
	bool isFlipY = false;
	bool isInvisible = false;
	Vector2 textureLeftTop; // �؂���̈�̍�����W
	Vector2 textureSize; // �؂���̈�̃T�C�Y

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
	Matrix4 matWorld;
	TextureData* tex = nullptr;

	void AdjustTextureSize();
	void Initialize();

public:
	static std::unique_ptr<Sprite> Create(const std::string& FILE_NAME);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() { return tex->gpuHandle; }
	void Update();
	void Draw();
};