#pragma once
#include "Sprite.h"

// マテリアル
class Material
{
private:
	struct ConstBufferData
	{
		ColorRGB ambient;
		float pad1;
		ColorRGB diffuse;
		float pad2;
		ColorRGB specular;
		float alpha;
	};

	std::string materialName;
	ColorRGB ambient = { 0.3f,0.3f,0.3f };
	ColorRGB diffuse;
	ColorRGB specular;
	ConstBufferData* constMap = nullptr;

protected:
	std::string textureFilename;
	std::unique_ptr<Sprite> sprite;		// テクスチャ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer;	// 定数バッファ
	
	void LoadMaterial(const std::string& directoryPath, const std::string& filename); // マテリアル読み込み
	void Initialize();
	void Update();
	void SetMaterial(Material* material);

public:
	void SetAnbient(ColorRGB anbient_) { ambient = anbient_; }
	void SetDiffuse(ColorRGB diffuse_) { diffuse = diffuse_; }
	void SetSpecular(ColorRGB specular_) { specular = specular_; }
	void SetSprite(std::unique_ptr<Sprite> sprite_) { sprite = move(sprite_); }
	Sprite* GetSprite() { return sprite.get(); }
};