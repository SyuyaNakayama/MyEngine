#include <DirectXTex.h>
#include "SpriteCommon.h"
#include "D3D12Common.h"
using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

string SpriteCommon::DEFAULT_TEXTURE_DIRECTORY_PATH = "Resources/";
ComPtr<ID3D12RootSignature> SpriteCommon::rootSignature;
ComPtr<ID3D12PipelineState> SpriteCommon::pipelineState;
ComPtr<ID3D12DescriptorHeap> SpriteCommon::srvHeap;
vector<TextureData> SpriteCommon::textures;

SpriteCommon* SpriteCommon::GetInstance()
{
	static SpriteCommon spriteCommon;
	return &spriteCommon;
}

void SpriteCommon::Initialize()
{
	PipelineManager pipelineManager;
	pipelineManager.LoadShaders(L"SpriteVS", L"SpritePS");
	pipelineManager.AddInputLayout("POSITION", DXGI_FORMAT_R32G32_FLOAT);
	pipelineManager.AddInputLayout("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
	pipelineManager.SetBlendDesc(D3D12_BLEND_OP_ADD, D3D12_BLEND_SRC_ALPHA, D3D12_BLEND_INV_SRC_ALPHA);
	pipelineManager.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pipelineManager.AddRootParameter(RootParamType::CBV);
	pipelineManager.AddRootParameter(RootParamType::DescriptorTable);
	pipelineManager.CreatePipeline(pipelineState, rootSignature);
	
	ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvHeapDesc.NumDescriptors = MAX_SRV_COUNT;
	Result result = device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
}

void SpriteCommon::SetDescriptorHeaps()
{
	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCommandList();
	// デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { srvHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

uint32_t SpriteCommon::LoadTexture(const std::string& FILE_NAME, uint32_t mipLevels)
{
	uint32_t textureIndex = textures.size();

	// テクスチャの重複読み込みを検出
	for (uint32_t i = 0; i < textureIndex; i++)
	{
		if (textures[i].fileName.find(FILE_NAME) == string::npos) { continue; }
		return i;
	}

	TexMetadata metadata{};
	ScratchImage scratchImg{}, mipChain{};

	string fullPath = DEFAULT_TEXTURE_DIRECTORY_PATH + FILE_NAME;

	// ワイド文字列に変換した際の文字列バッファサイズを計算
	int filePathBufferSize = MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, nullptr, 0);
	// ワイド文字列に変換
	vector<wchar_t> wfilePath(filePathBufferSize);
	MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wfilePath.data(), filePathBufferSize);

	Result result = LoadFromWICFile(wfilePath.data(), WIC_FLAGS_NONE, &metadata, scratchImg);

	HRESULT result1 = GenerateMipMaps(scratchImg.GetImages(), scratchImg.GetImageCount(),
		scratchImg.GetMetadata(), TEX_FILTER_DEFAULT, 0, mipChain);
	if(SUCCEEDED(result1))
	{
		scratchImg = move(mipChain);
		metadata = scratchImg.GetMetadata();
	}

	CD3DX12_RESOURCE_DESC textureResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format, metadata.width, (UINT)metadata.height,
		(UINT16)metadata.arraySize, (UINT16)metadata.mipLevels);

	TextureData texture;

	ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texture.buffer));

	for (size_t i = 0; i < metadata.mipLevels; i++)
	{
		const Image* img = scratchImg.GetImage(i, 0, 0);
		result = texture.buffer->WriteToSubresource((UINT)i, nullptr, img->pixels,
			(UINT)img->rowPitch, (UINT)img->slicePitch);
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = textureResourceDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	if (mipLevels == MIP_LEVELS_DEFAULT) { srvDesc.Texture2D.MipLevels = textureResourceDesc.MipLevels; }
	else { srvDesc.Texture2D.MipLevels = mipLevels; }

	CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		srvHeap->GetCPUDescriptorHandleForHeapStart(), textureIndex,
		device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	device->CreateShaderResourceView(texture.buffer.Get(), &srvDesc, srvHandle);

	texture.fileName = FILE_NAME;
	texture.cpuHandle = srvHandle;
	texture.gpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(
		srvHeap->GetGPUDescriptorHandleForHeapStart(), textureIndex,
		device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	textures.push_back(texture);

	return textureIndex;
}

void SpriteCommon::PreDraw()
{
	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCommandList();
	// パイプラインステートとルートシグネチャの設定コマンド
	cmdList->SetPipelineState(pipelineState.Get());
	cmdList->SetGraphicsRootSignature(rootSignature.Get());
	// プリミティブ形状の設定コマンド
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // 三角形リスト
	// デスクリプタヒープの設定コマンド
	ID3D12DescriptorHeap* ppHeaps[] = { srvHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}