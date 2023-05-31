#pragma once
#include "Sprite.h"

// �}�e���A��
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
	std::unique_ptr<Sprite> sprite;		// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer;	// �萔�o�b�t�@
	
	void LoadMaterial(const std::string& directoryPath, const std::string& filename); // �}�e���A���ǂݍ���
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