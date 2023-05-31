#pragma once
#include "DiffuseParticle.h"
#include "DirectionalParticle.h"
#include "TrackParticle.h"
#include <string>
#include <wrl.h>
#include <d3dx12.h>

// このインスタンス1つにつき1枚のテクスチャが使える
class ParticleGroup
{
private:
	// 頂点データ構造体
	struct VertexPos
	{
		Vector3 pos; // xyz座標
		float scale; // スケール
	};
	
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	
	static const int PARTICLE_MAX = 384; // パーティクル最大数
	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	VertexPos* vertMap;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView;
	// テクスチャインデックス
	uint32_t textureIndex = 0;
	// 拡散するパーティクル
	DiffuseParticle diffuseParticle;
	// 始点から終点へ向かうパーティクル
	DirectionalParticle directionalParticle;
	TrackParticle trackParticle;

	void CreateVertexBuffer(); // 頂点バッファ生成
	size_t AllParticleNum() { return diffuseParticle.GetParticles().size() + directionalParticle.GetParticles().size()+ trackParticle.GetParticles().size(); }
	bool IsParticleMax() { return AllParticleNum() >= PARTICLE_MAX; }

public:
	void Initialize(const std::string& textureName);
	void Update(); // 毎フレーム処理
	void Draw(); // 描画
	// パーティクルの追加
	void Add(const DiffuseParticle::AddProp& particleProp);
	void Add(const DirectionalParticle::AddProp& particleProp);
	void Add(const TrackParticle::AddProp& particleProp);
	void Clear(); // パーティクルの削除
};