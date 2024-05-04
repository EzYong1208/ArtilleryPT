#include "..\Public\Frustum.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CFrustum)

CFrustum::CFrustum()
{
}

HRESULT CFrustum::Initialize()
{
	//	생각 - 6면체.png 참고
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

	//	2차원 투영 스페이스 상의 정의된 절두체의 8개 점을 월드로 옮긴다
	//	* 투영 행렬의 역행렬 * 뷰 행렬의 역행렬

	//	원래는 나눴던 w로 곱해야함, 마지막에 w를 나눠서 투영스페이스로 간다임
	//	실제 이 투영 행렬의 역을 곱할때 XMVector3TransformCoord() 로 곱할건데 기본적으로 w 나누기 함수가 따라붙는 함수라 1/w 을 보관했다가 나중에 이걸 사용해서 자동으로 해줘서 따로 안함
	//	쉐이더에서 이러면 안됨!, mul 함수는 w 나누기 안해줌

	//	투영 행렬의 역행렬, 뷰 행렬의 역행렬
	_matrix			ProjMatrixInv, ViewMatrixInv;

	//	투영 행렬 가져오고
	ProjMatrixInv = pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);
	//	XMMatrixInverse() 로 역행렬 구하고 ProjMatrixInv 에 저장
	ProjMatrixInv = XMMatrixInverse(nullptr, ProjMatrixInv);

	//	뷰 행렬 가져오고
	ViewMatrixInv = pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	//	XMMatrixInverse() 로 역행렬 구하고 ViewMatrixInv 에 저장
	ViewMatrixInv = XMMatrixInverse(nullptr, ViewMatrixInv);

	//	뷰 스페이스로 변환
	//	8개 점을 모두 다 처리해줘야함
	for (_uint i = 0; i < 8; ++i)
	{
		_vector		vPoint;

		//	XMVector3TransformCoord() 로 i번째 점과 투영 행렬의 역행렬을 곱하고 vPoint 에 저장
		vPoint = XMVector3TransformCoord(XMLoadFloat3(&m_vPoint[i]), ProjMatrixInv);
		//	위에 곱한 결과값 vPoint와 뷰 행렬의 역행렬을 곱하고 vPoint 에 저장
		vPoint = XMVector3TransformCoord(vPoint, ViewMatrixInv);

		//	다 곱하기 처리된 vPoint를 월드 스페이스 상의 점 i번째에 저장
		XMStoreFloat3(&m_vPointInWorld[i], vPoint);
	}

	//	월드 평면 구해둠
	Compute_Plane(m_vPointInWorld, m_PlaneInWorld);

	RELEASE_INSTANCE(CPipeLine);
}

HRESULT CFrustum::Transform_ToLocalSpace(CTransform * pTransform)
{
	for (_uint i = 0; i < 8; ++i)
	{
		_vector		vPoint;

		//	월드 스페이스 상의 점 8개에 월드 행렬의 역행렬을 곱해서 vPoint에 저장
		vPoint = XMVector3TransformCoord(XMLoadFloat3(&m_vPointInWorld[i]), pTransform->Get_WorldMatrixInverse());

		//	곱하기 처리된 vPoint를 로컬 스페이스 상의 점 i번째에 저장
		XMStoreFloat3(&m_vPointInLocal[i], vPoint);
	}

	//	로컬 평면 구해둠
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
		//	XMPlaneDotCoord() : w 자리에 무조건 1을 넣고 Dot(내적)한다 -> 평면방정식을 해서 해를 구해줌
		//	이 해가 fRange보다 크면(한번이라도 나갔다면, 밖에 나갔다면)
		//	XMVectorGetX() 저장된 값 중에 딱 하나만(X) 꺼내서 비교함
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
		//	XMPlaneDotCoord() : w 자리에 무조건 1을 넣고 Dot(내적)한다 -> 평면방정식을 해서 해를 구해줌
		//	이 해가 fRange보다 크면(한번이라도 나갔다면, 밖에 나갔다면)
		//	XMVectorGetX() 저장된 값 중에 딱 하나만(X) 꺼내서 비교함
		if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_PlaneInLocal[i]), vPoint)) > fRange)
			return false;
	}

	return true;
}

void CFrustum::Compute_Plane(const _float3 * pPoints, _float4 * pPlanes)
{
	//	생각 - 6면체.png 참고
	//	+x
	//	1, 5, 6번 점을 pPlanes 0번째에 담음
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
