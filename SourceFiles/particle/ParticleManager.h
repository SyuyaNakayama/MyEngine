#pragma once
#include "Matrix4.h"
#include "ParticleGroup.h"

class ParticleManager final
{
private:
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	// 定数バッファ用データ構造体
	struct ConstBufferData
	{
		Matrix4 mat;	// 3D変換行列
		Matrix4 matBillboard; // ビルボード行列
	};

	// ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootsignature;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelinestate;
	// 定数バッファ
	static ComPtr<ID3D12Resource> constBuff;
	static ConstBufferData* constMap;
	// パーティクルグループの配列
	static std::vector<ParticleGroup> particleGroups;

	static void InitializeGraphicsPipeline(); // グラフィックパイプライン生成
	static void CreateConstBuffer(); // 定数バッファ生成
	ParticleManager() = delete;
	ParticleManager(const ParticleManager& obj) = delete;

public:
	static void Initialize(); // 静的初期化
	static void Update(); // 毎フレーム処理
	static void Draw(); // 描画
	static void Clear(); // パーティクルの削除
	static void AddParticleGroup(const std::string& textureName); // パーティクルグループの追加
	static ParticleGroup* GetParticleGroup(size_t index) { return &particleGroups[index]; }
};