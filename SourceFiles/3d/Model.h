#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "LightGroup.h"
#include "Mesh.h"

class Model
{
public:
	// �}�e���A��
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
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState> pipelinestate;
	// ���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> rootsignature;
	// ���C�g
	static std::unique_ptr<LightGroup> lightGroup;
	// �ǂݍ��񂾃��f�����
	static std::list<std::unique_ptr<Mesh>> meshes;
	// �r���[�v���W�F�N�V�����̃|�C���^
	static ViewProjection* viewProjection;
	
	Mesh* mesh = nullptr; // ���b�V���̃|�C���^
	ComPtr<ID3D12Resource> vertBuff;	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> indexBuff;	// �C���f�b�N�X�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer;	// �萔�o�b�t�@
	Mesh::VertexData* vertMap = nullptr;		// ���_�o�b�t�@�̃}�b�v
	ConstBufferData* constMap = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbView{};	// ���_�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView{};	// �C���f�b�N�X�o�b�t�@�r���[

	void CreateBuffers();

public:
	// �ÓI������
	static void StaticInitialize();
	// �ÓI�X�V
	static void StaticUpdate();
	// �`��O����
	static void PreDraw();
	// �`��㏈��
	static void PostDraw() {}
	// ���f���쐬
	static std::unique_ptr<Model> Create(const std::string& modelName, bool smoothing = false);
	static LightGroup* GetLightGroup() { return lightGroup.get(); }
	static void SetViewProjection(ViewProjection* viewProjection_) { viewProjection = viewProjection_; }
	static ViewProjection* GetViewProjection() { return viewProjection; }
	Mesh* GetMesh() { return mesh; }
	void Update();
	void Draw(const WorldTransform& worldTransform);
};