#include "Mesh.h"
#include "D3D12Common.h"
#include <fstream>
#include <sstream>
#include "SpriteCommon.h"
using namespace std;

void LoadVector3Stream(istringstream& stream, Vector3& vec)
{
	stream >> vec.x;
	stream >> vec.y;
	stream >> vec.z;
}

void Mesh::CalculateSmoothedVertexNormals()
{
	for (auto& itr : smoothData)
	{
		// 各面用の共通頂点コンテナ
		std::vector<UINT16>& v = itr.second;
		// 全頂点の法線を平均する
		Vector3 normal;
		for (UINT16 index : v) { normal += vertices[index].normal; }
		normal = Normalize(normal / (float)v.size());
		// 共通法線を使用する全ての頂点データに書き込む
		for (UINT16 index : v) { vertices[index].normal = normal; }
	}
}

void Mesh::LoadOBJ(const std::string& modelName_, bool isSmooth_)
{
	modelName = modelName_;
	isSmooth = isSmooth_;
	const string FILENAME = modelName + ".obj";
	const string DIRECTORY_PATH = "Resources/models/" + modelName + "/";

	ifstream file;
	file.open(DIRECTORY_PATH + FILENAME);
	assert(!file.fail());

	vector<Vector3> positions, normals;
	vector<Vector2> texcoords;
	int indexCountTex = 0;
	string line;

	while (getline(file, line))
	{
		istringstream line_stream(line);
		string key;
		getline(line_stream, key, ' ');

		// マテリアル読み込み
		if (key == "mtllib")
		{
			string filename;
			line_stream >> filename;
			directoryPath = DIRECTORY_PATH;
			materialFileName = filename;
		}
		// 頂点座標読み込み
		if (key == "v")
		{
			Vector3 position{};
			LoadVector3Stream(line_stream, position);
			positions.emplace_back(position);
		}
		// テクスチャ座標読み込み
		if (key == "vt")
		{
			Vector2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.emplace_back(texcoord);
		}
		// 法線ベクトル読み込み
		if (key == "vn")
		{
			Vector3 normal{};
			LoadVector3Stream(line_stream, normal);
			normals.emplace_back(normal);
		}
		// ポリゴンデータ読み込み
		if (key == "f")
		{
			string index_string;
			int faceIndexCount = 0;

			while (getline(line_stream, index_string, ' '))
			{
				istringstream index_stream(index_string);
				unsigned short indexPosition, indexNormal, indexTexcoord;
				index_stream >> indexPosition;
				index_stream.seekg(1, ios_base::cur); // スラッシュを飛ばす
				index_stream >> indexTexcoord;
				index_stream.seekg(1, ios_base::cur);
				index_stream >> indexNormal;

				// 頂点データの追加
				Mesh::VertexData vertex;
				vertex.pos = positions[(size_t)indexPosition - 1];
				vertex.normal = normals[(size_t)indexNormal - 1];
				vertex.uv = texcoords[(size_t)indexTexcoord - 1];
				vertices.emplace_back(vertex);
				if (isSmooth) { smoothData[indexPosition].emplace_back((UINT16)vertices.size() - 1); }

				// インデックスデータの追加
				if (faceIndexCount >= 3)
				{
					// 四角形ポリゴンの4点目なので、
					// 四角形の0,1,2,3の内 2,3,0で三角形を構築する
					indices.emplace_back(indexCountTex - 1);
					indices.emplace_back(indexCountTex);
					indices.emplace_back(indexCountTex - 3);
				}
				else { indices.emplace_back(indexCountTex); }

				indexCountTex++;
				faceIndexCount++;
			}
		}
	}
	file.close();

	if (isSmooth) { CalculateSmoothedVertexNormals(); }
}

void Mesh::Draw()
{
	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCommandList();
	SpriteCommon* spCommon = SpriteCommon::GetInstance();
	// 描画コマンド
	cmdList->DrawIndexedInstanced((UINT)indices.size(), 1, 0, 0, 0);
}