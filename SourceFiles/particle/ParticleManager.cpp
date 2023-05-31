#include <random>
#include "ParticleManager.h"
#include "SpriteCommon.h"
#include "D3D12Common.h"
#include "Model.h"
using namespace Microsoft::WRL;

// 静的メンバ変数の実体
ComPtr<ID3D12RootSignature> ParticleManager::rootsignature;
ComPtr<ID3D12PipelineState> ParticleManager::pipelinestate;
ComPtr<ID3D12Resource> ParticleManager::constBuff;
ParticleManager::ConstBufferData* ParticleManager::constMap = nullptr;
std::vector<ParticleGroup> ParticleManager::particleGroups;

void ParticleManager::Initialize()
{
	// パイプライン初期化
	InitializeGraphicsPipeline();
	// 定数バッファ生成
	CreateConstBuffer();
}

void ParticleManager::InitializeGraphicsPipeline()
{
	PipelineManager pipelineManager;
	pipelineManager.LoadShaders(L"ParticleVS", L"ParticlePS", L"ParticleGS");
	pipelineManager.AddInputLayout("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	pipelineManager.AddInputLayout("TEXCOORD", DXGI_FORMAT_R32G32B32_FLOAT);
	pipelineManager.InitDepthStencilState();
	pipelineManager.SetBlendDesc(D3D12_BLEND_OP_ADD, D3D12_BLEND_ONE, D3D12_BLEND_ONE); // 光パーティクル
	//pipelineManager.SetBlendDesc(D3D12_BLEND_OP_REV_SUBTRACT, D3D12_BLEND_ONE, D3D12_BLEND_ONE); // 闇パーティクル
	pipelineManager.InitDSVFormat();
	pipelineManager.SetDepthWriteMask(D3D12_DEPTH_WRITE_MASK_ZERO);
	pipelineManager.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT);
	pipelineManager.AddRootParameter(PipelineManager::RootParamType::CBV);
	pipelineManager.AddRootParameter(PipelineManager::RootParamType::DescriptorTable);
	pipelineManager.CreatePipeline(pipelinestate, rootsignature);
}

void ParticleManager::CreateConstBuffer()
{
	CreateBuffer(&constBuff, &constMap, (sizeof(ConstBufferData) + 0xff) & ~0xff);
}

void ParticleManager::Update()
{
	for (auto& particleGroup : particleGroups) { particleGroup.Update(); }
	// 定数バッファへデータ転送
	constMap->mat = Model::GetViewProjection()->GetViewProjectionMatrix();
	constMap->matBillboard = Matrix4::GetBillboard();
}

void ParticleManager::Draw()
{
	// コマンドリストをセット
	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCommandList();
	// パイプラインステートの設定
	cmdList->SetPipelineState(pipelinestate.Get());
	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(rootsignature.Get());
	// プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
	for (auto& particleGroup : particleGroups) { particleGroup.Draw(); }
}

void ParticleManager::Clear()
{
	for (auto& particleGroup : particleGroups) { particleGroup.Clear(); }
}

void ParticleManager::AddParticleGroup(const std::string& textureName)
{
	ParticleGroup pGroup;
	pGroup.Initialize(textureName);
	particleGroups.push_back(pGroup);
}