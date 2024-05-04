#include "..\Public\Frustum.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CFrustum)

CFrustum::CFrustum()
{
}

HRESULT CFrustum::Initialize()
{
	//	���� - 6��ü.png ����
	m_vPoint[0] = _float3(-1.f, 1.f, 0.f);
	m_vPoint[1] = _float3(1.f, 1.f, 0.f);
	m_vPoint[2] = _float3(1.f, -1.f, 0.f);
	m_vPoint[3] = _float3(-1.f, -1.f, 0.f);

	m_vPoint[4] = _float3(-1.f, 1.f, 1.f);
	m_vPoint[5] = _float3(1.f, 1.f, 1.f);
	m_vPoint[6] = _float3(1.f, -1.f, 1.f);
	m_vPoint[7] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}

void CFrustum::Update()
{
	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	//	2���� ���� �����̽� ���� ���ǵ� ����ü�� 8�� ���� ����� �ű��
	//	* ���� ����� ����� * �� ����� �����

	//	������ ������ w�� ���ؾ���, �������� w�� ������ ���������̽��� ������
	//	���� �� ���� ����� ���� ���Ҷ� XMVector3TransformCoord() �� ���Ұǵ� �⺻������ w ������ �Լ��� ����ٴ� �Լ��� 1/w �� �����ߴٰ� ���߿� �̰� ����ؼ� �ڵ����� ���༭ ���� ����
	//	���̴����� �̷��� �ȵ�!, mul �Լ��� w ������ ������

	//	���� ����� �����, �� ����� �����
	_matrix			ProjMatrixInv, ViewMatrixInv;

	//	���� ��� ��������
	ProjMatrixInv = pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);
	//	XMMatrixInverse() �� ����� ���ϰ� ProjMatrixInv �� ����
	ProjMatrixInv = XMMatrixInverse(nullptr, ProjMatrixInv);

	//	�� ��� ��������
	ViewMatrixInv = pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	//	XMMatrixInverse() �� ����� ���ϰ� ViewMatrixInv �� ����
	ViewMatrixInv = XMMatrixInverse(nullptr, ViewMatrixInv);

	//	�� �����̽��� ��ȯ
	//	8�� ���� ��� �� ó���������
	for (_uint i = 0; i < 8; ++i)
	{
		_vector		vPoint;

		//	XMVector3TransformCoord() �� i��° ���� ���� ����� ������� ���ϰ� vPoint �� ����
		vPoint = XMVector3TransformCoord(XMLoadFloat3(&m_vPoint[i]), ProjMatrixInv);
		//	���� ���� ����� vPoint�� �� ����� ������� ���ϰ� vPoint �� ����
		vPoint = XMVector3TransformCoord(vPoint, ViewMatrixInv);

		//	�� ���ϱ� ó���� vPoint�� ���� �����̽� ���� �� i��°�� ����
		XMStoreFloat3(&m_vPointInWorld[i], vPoint);
	}

	//	���� ��� ���ص�
	Compute_Plane(m_vPointInWorld, m_PlaneInWorld);

	RELEASE_INSTANCE(CPipeLine);
}

HRESULT CFrustum::Transform_ToLocalSpace(CTransform * pTransform)
{
	for (_uint i = 0; i < 8; ++i)
	{
		_vector		vPoint;

		//	���� �����̽� ���� �� 8���� ���� ����� ������� ���ؼ� vPoint�� ����
		vPoint = XMVector3TransformCoord(XMLoadFloat3(&m_vPointInWorld[i]), pTransform->Get_WorldMatrixInverse());

		//	���ϱ� ó���� vPoint�� ���� �����̽� ���� �� i��°�� ����
		XMStoreFloat3(&m_vPointInLocal[i], vPoint);
	}

	//	���� ��� ���ص�
	Compute_Plane(m_vPointInLocal, m_PlaneInLocal);

	return S_OK;
}

_bool CFrustum::isIn_WorldSpace(_fvector vPoint, _float fRange)
{
	/*
	a b c d
	x y z 1(w)
	ax + by + cz + d
	*/

	for (_uint i = 0; i < 6; ++i)
	{
		//	XMPlaneDotCoord() : w �ڸ��� ������ 1�� �ְ� Dot(����)�Ѵ� -> ���������� �ؼ� �ظ� ������
		//	�� �ذ� fRange���� ũ��(�ѹ��̶� �����ٸ�, �ۿ� �����ٸ�)
		//	XMVectorGetX() ����� �� �߿� �� �ϳ���(X) ������ ����
		if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_PlaneInWorld[i]), vPoint)) > fRange)
			return false;
	}

	return true;
}

_bool CFrustum::isIn_LocalSpace(_fvector vPoint, _float fRange)
{
	/*
	a b c d
	x y z 1(w)
	ax + by + cz + d
	*/

	for (_uint i = 0; i < 6; ++i)
	{
		//	XMPlaneDotCoord() : w �ڸ��� ������ 1�� �ְ� Dot(����)�Ѵ� -> ���������� �ؼ� �ظ� ������
		//	�� �ذ� fRange���� ũ��(�ѹ��̶� �����ٸ�, �ۿ� �����ٸ�)
		//	XMVectorGetX() ����� �� �߿� �� �ϳ���(X) ������ ����
		if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_PlaneInLocal[i]), vPoint)) > fRange)
			return false;
	}

	return true;
}

void CFrustum::Compute_Plane(const _float3 * pPoints, _float4 * pPlanes)
{
	//	���� - 6��ü.png ����
	//	+x
	//	1, 5, 6�� ���� pPlanes 0��°�� ����
	XMStoreFloat4(&pPlanes[0], XMPlaneFromPoints(XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[6])));
	//	-x
	XMStoreFloat4(&pPlanes[1], XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&pPoints[3])));
	//	+y
	XMStoreFloat4(&pPlanes[2], XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[1])));
	//	-y
	XMStoreFloat4(&pPlanes[3], XMPlaneFromPoints(XMLoadFloat3(&pPoints[6]), XMLoadFloat3(&pPoints[7]), XMLoadFloat3(&pPoints[3])));
	//	+z
	XMStoreFloat4(&pPlanes[4], XMPlaneFromPoints(XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[7])));
	//	-z
	XMStoreFloat4(&pPlanes[5], XMPlaneFromPoints(XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[2])));
}

void CFrustum::Free()
{
}
