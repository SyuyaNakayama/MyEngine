#pragma once
#include "Sprite.h"

class Mesh;

// マテリアル
class Material
{
private:
	// マテリアル
	struct ConstBufferData
	{
		ColorRGBA ambient; // パディング含む
		ColorRGBA diffuse;
		ColorRGBA specular;
		Vector2 uvOffset;  // uvずらし
		Vector2 tiling; // タイリング
		ColorRGBA color; // 色
	};

	std::string materialName;
	ColorRGB ambient = { 0.3f,0.3f,0.3f };
	ColorRGB diffuse;
	ColorRGB specular;
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer;	// 定数バッファ
	ConstBufferData* constMap = nullptr;
	std::unique_ptr<Sprite> sprite;		// テクスチャ

public:
	void Load(Mesh* mesh); // マテリアル読み込み
	void SetAnbient(ColorRGB anbient_) { ambient = anbient_; }
	void SetDiffuse(ColorRGB diffuse_) { diffuse = diffuse_; }
	void SetSpecular(ColorRGB specular_) { specular = specular_; }
	void SetSprite(std::unique_ptr<Sprite> sprite_) { sprite = move(sprite_); }
	Sprite* GetSprite() { return sprite.get(); }
	void Update();
	void Draw();
};