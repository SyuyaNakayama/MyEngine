#pragma once
#include "Sprite.h"

// マテリアル
class Material
{
private:
	std::string materialName;
	ColorRGB ambient = { 0.3f,0.3f,0.3f };
	ColorRGB diffuse;
	ColorRGB specular;

	std::unique_ptr<Sprite> sprite;		// テクスチャ

public:
	std::string textureFilename;
	void Load(const std::string& directoryPath, const std::string& filename); // マテリアル読み込み
	void SetAnbient(ColorRGB anbient_) { ambient = anbient_; }
	void SetDiffuse(ColorRGB diffuse_) { diffuse = diffuse_; }
	void SetSpecular(ColorRGB specular_) { specular = specular_; }
	const ColorRGB& GetAnbient() { return ambient; }
	const ColorRGB& GetDiffuse() { return diffuse; }
	const ColorRGB& GetSpecular() { return specular; }
	void SetSprite(std::unique_ptr<Sprite> sprite_) { sprite = move(sprite_); }
	Sprite* GetSprite() { return sprite.get(); }
};