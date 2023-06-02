#include "Model.h"
#include "D3D12Common.h"
#include "SpriteCommon.h"
using namespace Microsoft::WRL;
using namespace std;

// �ÓI�����o�ϐ��̎���
ComPtr<ID3D12PipelineState> Model::pipelinestate = nullptr;
ComPtr<ID3D12RootSignature> Model::rootsignature = nullptr;
unique_ptr<LightGroup> Model::lightGroup;
list<Model*> Model::models;
ViewProjection* Model::viewProjection = nullptr;

void Model::StaticInitialize()
{
	PipelineManager pipelineManager;
	pipelineManager.LoadShaders(L"OBJVertexShader", L"OBJPixelShader");
	pipelineManager.AddInputLayout("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	pipelineManager.AddInputLayout("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	pipelineManager.AddInputLayout("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
	pipelineManager.AddInputLayout("COLOR", DXGI_FORMAT_R32G32B32A32_FLOAT);
	pipelineManager.InitDepthStencilState();
	pipelineManager.InitDSVFormat();
	pipelineManager.SetBlendDesc(D3D12_BLEND_OP_ADD, D3D12_BLEND_SRC_ALPHA, D3D12_BLEND_INV_SRC_ALPHA);
	pipelineManager.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pipelineManager.AddRootParameter(PipelineManager::RootParamType::DescriptorTable);
	for (size_t i = 0; i < 4; i++) { pipelineManager.AddRootParameter(PipelineManager::RootParamType::CBV); }
	pipelineManager.CreatePipeline(pipelinestate, rootsignature);
	// ���C�g�O���[�v����
	lightGroup = LightGroup::Create();
	// �J��������
	ViewProjection* viewProjection = new ViewProjection;
	viewProjection->Initialize();
	Model::viewProjection = viewProjection;
}

std::unique_ptr<Model> Model::Create(const string& modelName, bool smoothing)
{
	unique_ptr<Model> newModel = make_unique<Model>();

	for (auto& model : models)
	{
		if (model->modelName.find(modelName) == string::npos) { continue; }
		if (model->isSmooth != smoothing) { continue; } // �X���[�W���O����/�Ȃ������
		// ���ɓǂݍ���ł������f���̏ꍇ
		newModel->SetMesh(model); // ���b�V���f�[�^���R�s�[
		newModel->SetMaterial(model); // �}�e���A���f�[�^���R�s�[
		std::unique_ptr<Sprite> newSprite = Sprite::Create(newModel->textureFilename);
		newModel->sprite = move(newSprite);
		newModel->CreateBuffers();
		return newModel;
	}

	newModel->modelName = modelName;
	newModel->isSmooth = smoothing;
	newModel->LoadOBJ(modelName);
	newModel->CreateBuffers();
	models.push_back(newModel.get());
	return newModel;
}

void Model::PreDraw()
{
	// �R�}���h���X�g���Z�b�g
	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCommandList();

	// �p�C�v���C���X�e�[�g�̐ݒ�
	cmdList->SetPipelineState(pipelinestate.Get());
	// ���[�g�V�O�l�`���̐ݒ�
	cmdList->SetGraphicsRootSignature(rootsignature.Get());
	// �v���~�e�B�u�`���ݒ�
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// ���C�g�̕`��
	lightGroup->Draw();
	// �J����
	cmdList->SetGraphicsRootConstantBufferView(4, viewProjection->constBuffer->GetGPUVirtualAddress());
	// �f�X�N���v�^�q�[�v�̔z��
	SpriteCommon* spCommon = SpriteCommon::GetInstance();
	ID3D12DescriptorHeap* ppHeaps[] = { spCommon->GetDescriptorHeap() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

void Model::Draw(const WorldTransform& worldTransform)
{
	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCommandList();
	cmdList->SetGraphicsRootConstantBufferView(1, worldTransform.constBuffer->GetGPUVirtualAddress());
	Mesh::Draw();
}

void Model::StaticUpdate()
{
	lightGroup->Update();
	viewProjection->Update();
}