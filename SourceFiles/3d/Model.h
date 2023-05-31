#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "LightGroup.h"
#include "Mesh.h"

class Model : public Mesh
{
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
	static std::list<Model*> models;
	// ビュープロジェクションのポインタ
	static ViewProjection* viewProjection;
	
	std::string modelName;	// モデル名

public:
	~Model() { models.remove(this); }
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
	void Draw(const WorldTransform& worldTransform);
};