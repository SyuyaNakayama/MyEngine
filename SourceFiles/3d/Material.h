#pragma once
#include "Sprite.h"

// �}�e���A��
class Material
{
public:
	// �}�e���A��
	struct ConstBufferData
	{
		ColorRGB ambient;
		float pad1;
		ColorRGB diffuse;
		float pad2;
		ColorRGB specular;
		float alpha;
	};

private:
	std::string materialName;
	ColorRGB ambient = { 0.3f,0.3f,0.3f };
	ColorRGB diffuse;
	ColorRGB specular;
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer;	// �萔�o�b�t�@
	ConstBufferData* constMap = nullptr;

	std::unique_ptr<Sprite> sprite;		// �e�N�X�`��

public:
	std::string textureFilename;
	void Load(const std::string& directoryPath, const std::string& filename); // �}�e���A���ǂݍ���
	void SetAnbient(ColorRGB anbient_) { ambient = anbient_; }
	void SetDiffuse(ColorRGB diffuse_) { diffuse = diffuse_; }
	void SetSpecular(ColorRGB specular_) { specular = specular_; }
	const ColorRGB& GetAnbient() { return ambient; }
	const ColorRGB& GetDiffuse() { return diffuse; }
	const ColorRGB& GetSpecular() { return specular; }
	void SetSprite(std::unique_ptr<Sprite> sprite_) { sprite = move(sprite_); }
	Sprite* GetSprite() { return sprite.get(); }
	void Draw();
};