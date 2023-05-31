#include "ViewProjection.h"
#include "D3D12Common.h"
#include <DirectXMath.h>
using namespace DirectX;

void ViewProjection::Initialize()
{
	CreateBuffer(constBuffer.GetAddressOf(), &constMap, (sizeof(ConstBufferData) + 0xff) & ~0xff);
}

XMFLOAT3 ChangeVec(const Vector3& v) { return XMFLOAT3(v.x, v.y, v.z); }
Matrix4 ChangeMat(const XMMATRIX& m)
{
	Matrix4 mat;

	for (size_t i = 0; i < 4; i++) {
		for (size_t j = 0; j < 4; j++)
		{
			mat.m[i][j] = m.r[i].m128_f32[j];
		}
	}

	return mat;
}

void ViewProjection::Update()
{
	// プロジェクション行列を求める
	matProjection = Matrix4::Zero();
	matProjection.m[0][0] = 1.0f / (aspectRatio * std::tan(fovAngleY / 2.0f));
	matProjection.m[1][1] = 1.0f / std::tan(fovAngleY / 2.0f);
	matProjection.m[2][2] = farZ / (farZ - nearZ);
	matProjection.m[2][3] = 1.0f;
	matProjection.m[3][2] = -nearZ * farZ / (farZ - nearZ);

	// ビュー行列を求める
	matView = ChangeMat(XMMatrixLookAtLH(XMLoadFloat3(&ChangeVec(eye)), XMLoadFloat3(&ChangeVec(target)), XMLoadFloat3(&ChangeVec(up))));

	constMap->viewproj = GetViewProjectionMatrix();
	constMap->cameraPos = eye;
}

void ViewProjection::CameraMove(const Vector3& move)
{
	eye += move;
	target += move;
}