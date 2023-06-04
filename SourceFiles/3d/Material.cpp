#include "Material.h"
#include <fstream>
#include <sstream>
#include "D3D12Common.h"
#include "SpriteCommon.h"
using namespace std;

void LoadColorRGBStream(istringstream& stream, ColorRGB& color)
{
	Vector3 colorTemp;
	stream >> colorTemp.x;
	stream >> colorTemp.y;
	stream >> colorTemp.z;
	color.r = colorTemp.x;
	color.g = colorTemp.y;
	color.b = colorTemp.z;
}

void Material::Load(const string& directoryPath, const string& filename)
{
	ifstream file;
	file.open(directoryPath + filename);
	assert(!file.fail());

	string line;
	while (getline(file, line))
	{
		istringstream line_stream(line);
		string key;
		getline(line_stream, key, ' ');

		if (key[0] == '\t') { key.erase(key.begin()); }
		if (key == "newmtl") { line_stream >> materialName; }
		if (key == "Ka") { LoadColorRGBStream(line_stream, ambient); }
		if (key == "Kd") { LoadColorRGBStream(line_stream, diffuse); }
		if (key == "Ks") { LoadColorRGBStream(line_stream, specular); }
		if (key == "map_Kd")
		{
			line_stream >> textureFilename;
			string path = directoryPath;
			path.erase(path.begin(), path.begin() + 10);
			sprite = Sprite::Create(path + textureFilename);
		}
	}
	file.close();

	// 定数バッファ生成
	CreateBuffer(&constBuffer, &constMap, (sizeof(ConstBufferData) + 0xff) & ~0xff);

	constMap->ambient = ambient;
	constMap->diffuse = diffuse;
	constMap->specular = specular;
	constMap->alpha = 1.0f;
}

void Material::Draw()
{
	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCommandList();
	cmdList->SetGraphicsRootConstantBufferView(2, constBuffer->GetGPUVirtualAddress());
	// シェーダリソースビューをセット
	SpriteCommon* spCommon = SpriteCommon::GetInstance();
	cmdList->SetGraphicsRootDescriptorTable(0, spCommon->GetGpuHandle(sprite->GetTextureIndex()));
}