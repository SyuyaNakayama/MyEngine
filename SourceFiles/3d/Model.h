#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "LightGroup.h"
#include "Mesh.h"

class Model
{
public:
	// マテリアル
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
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelinestate;
	// ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootsignature;
	// ライト
	static std::unique_ptr<LightGroup> lightGroup;
	// 読み込んだモデル情報
	static std::list<std::unique_ptr<Mesh>> meshes;
	// ビュープロジェクションのポインタ
	static ViewProjection* viewProjection;
	
	Mesh* mesh = nullptr; // メッシュのポインタ
	ComPtr<ID3D12Resource> vertBuff;	// 頂点バッファ
	ComPtr<ID3D12Resource> indexBuff;	// インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer;	// 定数バッファ
	Mesh::VertexData* vertMap = nullptr;		// 頂点バッファのマップ
	ConstBufferData* constMap = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbView{};	// 頂点バッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView{};	// インデックスバッファビュー

	void CreateBuffers();

public:
	// 静的初期化
	static void StaticInitialize();
	// 静的更新
	static void StaticUpdate();
	// 描画前処理
	static void PreDraw();
	// 描画後処理
	static void PostDraw() {}
	// モデル作成
	static std::unique_ptr<Model> Create(const std::string& modelName, bool smoothing = false);
	static LightGroup* GetLightGroup() { return lightGroup.get(); }
	static void SetViewProjection(ViewProjection* viewProjection_) { viewProjection = viewProjection_; }
	static ViewProjection* GetViewProjection() { return viewProjection; }
	Mesh* GetMesh() { return mesh; }
	void Update();
	void Draw(const WorldTransform& worldTransform);
};