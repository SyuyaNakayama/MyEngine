#pragma once
#include <string>
#include <list>
#include "DirectXCommon.h"

struct TextureData
{
	std::string fileName;
	Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle{};
	D3D12_GPU_DESCRIPTOR_HANDLE	gpuHandle{};
};

class SpriteCommon
{
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	static const size_t MAX_SRV_COUNT = 2056;
	static const uint32_t MIP_LEVELS_DEFAULT = -1;
	// デフォルトディレクトリ
	static std::string DEFAULT_TEXTURE_DIRECTORY_PATH;
	static ComPtr<ID3D12RootSignature> rootSignature;
	static ComPtr<ID3D12PipelineState> pipelineState;
	static ComPtr<ID3D12DescriptorHeap> srvHeap;
	static std::list<TextureData> textures;

	SpriteCommon() = default;
public:
	static SpriteCommon* GetInstance();
	void Initialize();
	uint32_t LoadTexture(const std::string& FILE_NAME, uint32_t mipLevels = MIP_LEVELS_DEFAULT);
	static void PreDraw();
	static void PostDraw() {}
	static TextureData* GetTextureData(uint32_t index)
	{
		auto itr = textures.begin();
		for (size_t i = 0; i < index; i++) { itr++; }
		return &*itr;
	}
	static ID3D12DescriptorHeap* GetDescriptorHeap() { return srvHeap.Get(); }
	void SetDescriptorHeaps();
};