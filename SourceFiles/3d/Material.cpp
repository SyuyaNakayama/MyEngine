#include "Material.h"
#include <fstream>
#include <sstream>
#include "D3D12Common.h"
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

void Material::LoadMaterial(const string& directoryPath, const string& filename)
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
}

void Material::Initialize()
{
	// 定数バッファ生成
	CreateBuffer(&constBuffer, &constMap, (sizeof(ConstBufferData) + 0xff) & ~0xff);
	Update();
}

void Material::Update()
{
	constMap->ambient = ambient;
	constMap->diffuse = diffuse;
	constMap->specular = specular;
	constMap->alpha = 1.0f;
}

void Material::SetMaterial(Material* material)
{
	materialName = material->materialName;
	ambient = material->ambient;
	diffuse = material->diffuse;
	specular = material->specular;
	textureFilename = material->textureFilename;
}