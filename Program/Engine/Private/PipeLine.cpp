#include "..\Public\PipeLine.h"

IMPLEMENT_SINGLETON(CPipeLine)

CPipeLine::CPipeLine()
{
}

HRESULT CPipeLine::Set_Transform(TRANSFORMSTATETYPE eStateType, _fmatrix TransformMatrix)
{
	XMStoreFloat4x4(&m_TransformMatrix[eStateType], TransformMatrix);

	return S_OK;
}

_float4x4 CPipeLine::Get_TransformFloat4x4_TP(TRANSFORMSTATETYPE eStateType)
{
	//	멤버변수에 바로 대입하는건 위험해서 지역변수 TransposeMatrix 만들고 전치해서 이걸 리턴
	_float4x4		TransposeMatrix;

	XMStoreFloat4x4(&TransposeMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformMatrix[eStateType])));

	return TransposeMatrix;
}

void CPipeLine::Update()
{
	//	뷰 행렬 저장
	_matrix			ViewMatrix = XMLoadFloat4x4(&m_TransformMatrix[D3DTS_VIEW]);

	//	역행렬을 구하고 지역변수에 저장
	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	//	이 뷰매트릭스의.r (월드행렬의 4번째 행 -> 위치 벡터) 이걸 저장
	//	XMVector 타입이라 XMStoreFloat4, m_vCamPosition에 저장
	XMStoreFloat4(&m_vCamPosition, ViewMatrix.r[3]);
}

void CPipeLine::Free()
{
}

