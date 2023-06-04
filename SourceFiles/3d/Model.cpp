#include "Model.h"
#include "D3D12Common.h"
#include "SpriteCommon.h"
using namespace Microsoft::WRL;
using namespace std;

// �ÓI�����o�ϐ��̎���
ComPtr<ID3D12PipelineState> Model::pipelinestate = nullptr;
ComPtr<ID3D12RootSignature> Model::rootsignature = nullptr;
unique_ptr<LightGroup> Model::lightGroup;
list<unique_ptr<Mesh>> Model::meshes;
ViewProjection* Model::viewProjection = nullptr;

void Model::CreateBuffers()
{
	auto& vertices = mesh->GetVertices();
	UINT sizeVB = static_cast<UINT>(sizeof(Mesh::VertexData) * vertices.size());
	// ���_�o�b�t�@����
	CreateBuffer(&vertBuff, &vertMap, sizeVB);
	// �S���_�ɑ΂���
	copy(vertices.begin(), vertices.end(), vertMap); // ���W���R�s�[
	// ���_�o�b�t�@�r���[�̍쐬
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(Mesh::VertexData);

	UINT16* indexMap = nullptr;
	auto& indices = mesh->GetIndices();
	UINT sizeIB = static_cast<UINT>(sizeof(UINT16) * indices.size());
	// �C���f�b�N�X�o�b�t�@����
	CreateBuffer(&indexBuff, &indexMap, sizeIB);
	// �S�C���f�b�N�X�ɑ΂���
	copy(indices.begin(), indices.end(), indexMap);	// �C���f�b�N�X���R�s�[
	// �C���f�b�N�X�o�b�t�@�r���[�̍쐬
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	// �萔�o�b�t�@����
	CreateBuffer(&constBuffer, &constMap, (sizeof(ConstBufferData) + 0xff) & ~0xff);
	
	constMap->ambient = material.GetAnbient();
	constMap->diffuse = material.GetDiffuse();
	constMap->specular = material.GetSpecular();
	constMap->alpha = 1.0f;
}

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

	for (auto& mesh : meshes)
	{
		if (mesh->modelName.find(modelName) == string::npos) { continue; }
		if (mesh->isSmooth != smoothing) { continue; } // �X���[�W���O����/�Ȃ������
		// ���ɓǂݍ���ł������f���̏ꍇ
		newModel->mesh = mesh.get();
		newModel->material.Load(mesh->directoryPath, mesh->materialFileName);
		newModel->CreateBuffers();
		return newModel;
	}

	unique_ptr<Mesh> newMesh = make_unique<Mesh>();
	newMesh->LoadOBJ(modelName, smoothing);
	newModel->mesh = newMesh.get();
	newModel->material.Load(newMesh->directoryPath, newMesh->materialFileName);
	newModel->CreateBuffers();
	meshes.push_back(move(newMesh));
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

void Model::Update()
{
	Sprite* sprite = material.GetSprite();
	sprite->Update();
	Vector2 spriteSizeRate =
	{
		sprite->GetTextureSize().x / sprite->GetSize().x,
		sprite->GetTextureSize().y / sprite->GetSize().y
	};

	auto& vertices = mesh->GetVertices();
	for (size_t i = 0; i < vertices.size(); i++)
	{
		Vector2 uv = vertices[i].uv;
		uv.x *= spriteSizeRate.x;
		uv.y *= spriteSizeRate.y;
		uv += sprite->GetVerticesUv(Sprite::VertexNumber::LT);
		vertMap[i].uv = uv;
		vertMap[i].color = sprite->GetColor();
	}
}

void Model::Draw(const WorldTransform& worldTransform)
{
	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCommandList();
	cmdList->SetGraphicsRootConstantBufferView(1, worldTransform.constBuffer->GetGPUVirtualAddress());
	cmdList->SetGraphicsRootConstantBufferView(2, constBuffer->GetGPUVirtualAddress());
	// ���_�o�b�t�@�̐ݒ�
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	// �C���f�b�N�X�o�b�t�@�̐ݒ�
	cmdList->IASetIndexBuffer(&ibView);
	// �V�F�[�_���\�[�X�r���[���Z�b�g
	SpriteCommon* spCommon = SpriteCommon::GetInstance();
	cmdList->SetGraphicsRootDescriptorTable(0, spCommon->GetGpuHandle(material.GetSprite()->GetTextureIndex()));
	mesh->Draw();
}

void Model::StaticUpdate()
{
	lightGroup->Update();
	viewProjection->Update();
}