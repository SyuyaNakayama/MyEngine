#pragma once
#include "Material.h"
#include <unordered_map>

// 形状データ
class Mesh
{
public:
	// 頂点データ構造体
	struct VertexData
	{
		Vector3 pos; // xyz座標
		Vector3 normal; // 法線ベクトル
		Vector2 uv;  // uv座標
		ColorRGBA color; // 色
	};

private:
	// Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	// 頂点法線の平均化
	void CalculateSmoothedVertexNormals();
	std::vector<VertexData> vertices;	// 頂点データ配列
	std::vector<UINT16> indices;		// 頂点インデックス配列
	std::unordered_map<UINT16, std::vector<UINT16>> smoothData;	// 頂点座標スムージング用データ

public:
	bool isSmooth = false;	// スムージング
	std::string modelName;	// モデル名
	std::string materialFileName;	// マテリアルファイル名
	std::string directoryPath;	// ディレクトリパス名

	void LoadOBJ(const std::string& modelName, bool isSmooth);
	const std::vector<VertexData>& GetVertices() { return vertices; }
	const std::vector<UINT16>& GetIndices() { return indices; }
	void Draw();
};