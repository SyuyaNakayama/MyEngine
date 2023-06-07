#pragma once
#include "Material.h"
#include <unordered_map>

// 形状データ
class Mesh
{
private:
	// 頂点データ構造体
	struct VertexData
	{
		Vector3 pos; // xyz座標
		Vector3 normal; // 法線ベクトル
		Vector2 uv;  // uv座標
	};

	friend Material;
	// Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	std::vector<VertexData> vertices;	// 頂点データ配列
	std::vector<UINT16> indices;		// 頂点インデックス配列
	std::unordered_map<UINT16, std::vector<UINT16>> smoothData;	// 頂点座標スムージング用データ
	ComPtr<ID3D12Resource> vertBuff;	// 頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW vbView{};	// 頂点バッファビュー
	ComPtr<ID3D12Resource> indexBuff;	// インデックスバッファ
	D3D12_INDEX_BUFFER_VIEW ibView{};	// インデックスバッファビュー
	bool isSmooth = false;	// スムージング
	std::string modelName;	// モデル名
	std::string materialFileName;	// マテリアルファイル名
	std::string directoryPath;	// ディレクトリパス名

	// 頂点法線の平均化
	void CalculateSmoothedVertexNormals();
	void CreateBuffers();

public:
	void LoadOBJ(const std::string& modelName, bool isSmooth);
	void Draw();
	bool IsLoaded(const std::string& modelName, bool isSmooth);
};