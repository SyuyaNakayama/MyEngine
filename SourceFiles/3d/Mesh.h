#pragma once
#include "Material.h"
#include <unordered_map>

// 形状データ
class Mesh : public Material
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

	ComPtr<ID3D12Resource> vertBuff;	// 頂点バッファ
	ComPtr<ID3D12Resource> indexBuff;	// インデックスバッファ
	D3D12_VERTEX_BUFFER_VIEW vbView{};	// 頂点バッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView{};	// インデックスバッファビュー
	VertexData* vertMap = nullptr;		// 頂点バッファのマップ

	// 頂点法線の平均化
	void CalculateSmoothedVertexNormals();

protected:
	std::vector<VertexData> vertices;	// 頂点データ配列
	std::vector<UINT16> indices;		// 頂点インデックス配列
	std::unordered_map<UINT16, std::vector<UINT16>> smoothData;	// 頂点座標スムージング用データ
	bool isSmooth = false;	// スムージング

	void LoadOBJ(const std::string& modelName);
	void CreateBuffers(); // バッファの生成
	void SetMesh(Mesh* mesh);
	void Draw();

public:
	virtual ~Mesh() = default;
	void Update();
};
