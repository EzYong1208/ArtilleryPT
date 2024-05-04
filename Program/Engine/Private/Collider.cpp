#include "..\Public\Collider.h"
#include "DebugDraw.h"
#include "PipeLine.h"

CCollider::CCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{

}

CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs)
	, m_pBasicEffect(rhs.m_pBasicEffect)
	, m_pBatch(rhs.m_pBatch)
	, m_eType(rhs.m_eType)
	, m_pInputLayout(rhs.m_pInputLayout)
	, m_isCollision(rhs.m_isCollision)
{
	for (_uint i = 0; i < BOUNDING_END; ++i)
	{
		if (nullptr != rhs.m_pAABB[BOUNDING_ORIGINAL])
			m_pAABB[i] = new BoundingBox(*rhs.m_pAABB[BOUNDING_ORIGINAL]);

		if (nullptr != rhs.m_pOBB[BOUNDING_ORIGINAL])
			m_pOBB[i] = new BoundingOrientedBox(*rhs.m_pOBB[BOUNDING_ORIGINAL]);

		if (nullptr != rhs.m_pSphere[BOUNDING_ORIGINAL])
			m_pSphere[i] = new BoundingSphere(*rhs.m_pSphere[BOUNDING_ORIGINAL]);
	}

	Safe_AddRef(m_pInputLayout);
}

HRESULT CCollider::NativeConstruct_Prototype(TYPE eType)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_eType = eType;

	/*
	원형객체 생성할 때 각 콜라이더들의 중심점은 원점으로, 모든 사이즈는 1로 정의해서 기본적으로 만들어 놓고
	복제 처리할 때 원본을 복제해서 사본 만들때 다시 만들어
	해당콜라이더의 초기 위치값 스케일값 회전값 등을 복제할때 세팅할 수 있게끔 만들자
	*/

	/* 중심점은 원점. 모든 사이즈는 1로. */

	switch (m_eType)
	{
	case TYPE_AABB:
		//	BoundingBox(center(박스를 띄우기 위한 중심점 위치, 무조건 원점으로 가주는걸로), extents(사이즈, 여기 사이즈를 다 1(반지름?이라 0.5)로 줌))
		m_pAABB[BOUNDING_ORIGINAL] = new BoundingBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f));
		break;

	case TYPE_OBB:
		//	BoundingOrientedBox(center, extents, orientation(방향, 기본적인 회전상태로 또 세팅, 퀀터니언 초기값은 0001))
		m_pOBB[BOUNDING_ORIGINAL] = new BoundingOrientedBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f), _float4(0.f, 0.f, 0.f, 1.f));
		break;

	case TYPE_SPHERE:
		//	BoundingSphere(center, radius(반지름))
		m_pSphere[BOUNDING_ORIGINAL] = new BoundingSphere(_float3(0.f, 0.f, 0.f), 0.5f);
		break;
	}

#ifdef _DEBUG
	m_pBasicEffect = new BasicEffect(m_pDevice);

	//	색깔 처리하려면 SetVertexColorEnabled() 함수
	//	그릴려고 하는 색상을 정점에 적용해서 그릴수 있게 됨
	m_pBasicEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = nullptr;
	size_t		iShaderByteCodeLength = 0;

	/*
	Input 시그니쳐를 가진 바이트코드가 필요행
	BasicEffect 안에 GetVertexShaderByteCode
	해당값 받아주고 바이트코드, length 넣고, m_pInputLayout에다 저장
	*/
	m_pBasicEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	//	InputLayout 만드는 작업을 직접 해줘야함
	//	CreateInputLayout(정점 구성정보의 배열(DirectX::VertexPositionCOler안에 ImputElemets가 있음, 이거 넣어주면 됨), ElemetsCount 갯수(DirectX::VertexPositionColor::InputElementCount)
	if (FAILED(m_pDevice->CreateInputLayout(DirectX::VertexPositionColor::InputElements, DirectX::VertexPositionColor::InputElementCount,
		pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;

	m_pBatch = new PrimitiveBatch<DirectX::VertexPositionColor>(m_pDeviceContext);

#endif // _DEBUG



	return S_OK;
}
HRESULT CCollider::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	_matrix		TransformMatrix;

	//	스케일(pArg가 null이 아니면 ColliderDesc값 memcpy)
	//	BoundingBox는 Transform은 회전값은 아예 무시됨, AABB냐 OBB냐에 따라 TransformMatrix를 따로 만들 필요가 없다
	if (nullptr != pArg)
	{
		//	Collider ColliderDesc 제로메모리 ㄴㄴ해
		memcpy(&m_ColliderDesc, pArg, sizeof(COLLIDERDESC));
		TransformMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&m_ColliderDesc.vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&m_ColliderDesc.vRotation), XMLoadFloat4(&m_ColliderDesc.vTranslation));
	}
	else
		TransformMatrix = XMMatrixIdentity();
	
	if (nullptr != m_pAABB[BOUNDING_ORIGINAL])
		m_pAABB[BOUNDING_ORIGINAL]->Transform(*m_pAABB[BOUNDING_ORIGINAL], TransformMatrix);
	if (nullptr != m_pOBB[BOUNDING_ORIGINAL])
		m_pOBB[BOUNDING_ORIGINAL]->Transform(*m_pOBB[BOUNDING_ORIGINAL], TransformMatrix);
	if (nullptr != m_pSphere[BOUNDING_ORIGINAL])
		m_pSphere[BOUNDING_ORIGINAL]->Transform(*m_pSphere[BOUNDING_ORIGINAL], TransformMatrix);

	return S_OK;
}

void CCollider::Update_Transform(_fmatrix TransformMatrix)
{
	//	현재 띄우고자하는 객체의 월드행렬이 필요

	if (nullptr != m_pAABB[BOUNDING_ORIGINAL])
		m_pAABB[BOUNDING_ORIGINAL]->Transform(*m_pAABB[BOUNDING_TRANSFORM], Remove_Rotation(TransformMatrix));

	if (nullptr != m_pOBB[BOUNDING_ORIGINAL])
		m_pOBB[BOUNDING_ORIGINAL]->Transform(*m_pOBB[BOUNDING_TRANSFORM], TransformMatrix);

	if (nullptr != m_pSphere[BOUNDING_ORIGINAL])
		m_pSphere[BOUNDING_ORIGINAL]->Transform(*m_pSphere[BOUNDING_TRANSFORM], TransformMatrix);
}

//_bool CCollider::Collision_AABB(CCollider * pPlayerCollider, CCollider * pTargetCollider)
//{
//	if (nullptr == pPlayerCollider->m_pAABB[BOUNDING_TRANSFORM] ||
//		nullptr == pTargetCollider->m_pAABB[BOUNDING_TRANSFORM])
//		return false;
//
//	//	Intersects 함수로 충돌구현-------------------
//	if (true == pPlayerCollider->m_pAABB[BOUNDING_TRANSFORM]->Intersects(*pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]))
//		m_isCollision = true;
//	else
//		m_isCollision = false;
//
//
//	////	직접 충돌구현--------------------------------
//	////	나의 Sour Min, Max
//	//_float3		vSourMin, vSourMax;
//	////	쟤꺼의 Dest Min, Max
//	//_float3		vDestMin, vDestMax;
//
//	////	이렇게 min max를 구할수 있는 이유는 박스가 회전하지 않는다는 보장이 있기 때문
//	////	콜라이더의 Center x가 중점 x에서 중점 x의 사이즈(extents) 뺌
//	//vSourMin = _float3(pPlayerCollider->m_pAABB[BOUNDING_TRANSFORM]->Center.x - pPlayerCollider->m_pAABB[BOUNDING_TRANSFORM]->Extents.x,
//	//	pPlayerCollider->m_pAABB[BOUNDING_TRANSFORM]->Center.y - m_pAABB[BOUNDING_TRANSFORM]->Extents.y,
//	//	pPlayerCollider->m_pAABB[BOUNDING_TRANSFORM]->Center.z - m_pAABB[BOUNDING_TRANSFORM]->Extents.z);
//
//	//vSourMax = _float3(pPlayerCollider->m_pAABB[BOUNDING_TRANSFORM]->Center.x + pPlayerCollider->m_pAABB[BOUNDING_TRANSFORM]->Extents.x,
//	//	pPlayerCollider->m_pAABB[BOUNDING_TRANSFORM]->Center.y + pPlayerCollider->m_pAABB[BOUNDING_TRANSFORM]->Extents.y,
//	//	pPlayerCollider->m_pAABB[BOUNDING_TRANSFORM]->Center.z + pPlayerCollider->m_pAABB[BOUNDING_TRANSFORM]->Extents.z);
//
//	//vDestMin = _float3(pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]->Center.x - pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]->Extents.x,
//	//	pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]->Center.y - pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]->Extents.y,
//	//	pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]->Center.z - pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]->Extents.z);
//
//	//vDestMax = _float3(pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]->Center.x + pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]->Extents.x,
//	//	pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]->Center.y + pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]->Extents.y,
//	//	pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]->Center.z + pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]->Extents.z);
//
//	//m_isCollision = true;
//
//	////	min중에 큰게 max중에 작은거보다 크면 충돌이 아님
//	//if (max(vSourMin.x, vDestMin.x) > min(vSourMax.x, vDestMax.x))
//	//	m_isCollision = false;
//
//	//if (max(vSourMin.y, vDestMin.y) > min(vSourMax.y, vDestMax.y))
//	//	m_isCollision = false;
//
//	//if (max(vSourMin.z, vDestMin.z) > min(vSourMax.z, vDestMax.z))
//	//	m_isCollision = false;
//
//	return m_isCollision;
//}

_bool CCollider::Collision_AABB(CCollider * pTargetCollider)
{
	if (nullptr == m_pAABB[BOUNDING_TRANSFORM]/* ||
		nullptr == pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]*/)
		return false;

	////	Intersects 함수로 충돌구현-------------------

	if (nullptr != pTargetCollider->m_pAABB[BOUNDING_TRANSFORM])
	{
		if (true == m_pAABB[BOUNDING_TRANSFORM]->Intersects(*pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]))
			m_isCollision = true;
	}

	if (nullptr != pTargetCollider->m_pSphere[BOUNDING_TRANSFORM])
	{
		if (true == m_pSphere[BOUNDING_TRANSFORM]->Intersects(*pTargetCollider->m_pSphere[BOUNDING_TRANSFORM]))
			m_isCollision = true;
	}

	else
		m_isCollision = false;

	////	직접 충돌구현--------------------------------

	////	나의 Sour Min, Max
	//_float3		vSourMin, vSourMax;
	////	쟤꺼의 Dest Min, Max
	//_float3		vDestMin, vDestMax;

	////	이렇게 min max를 구할수 있는 이유는 박스가 회전하지 않는다는 보장이 있기 때문
	////	콜라이더의 Center x가 중점 x에서 중점 x의 사이즈(extents) 뺌
	//vSourMin = _float3(m_pAABB[BOUNDING_TRANSFORM]->Center.x - m_pAABB[BOUNDING_TRANSFORM]->Extents.x,
	//	m_pAABB[BOUNDING_TRANSFORM]->Center.y - m_pAABB[BOUNDING_TRANSFORM]->Extents.y,
	//	m_pAABB[BOUNDING_TRANSFORM]->Center.z - m_pAABB[BOUNDING_TRANSFORM]->Extents.z);

	//vSourMax = _float3(m_pAABB[BOUNDING_TRANSFORM]->Center.x + m_pAABB[BOUNDING_TRANSFORM]->Extents.x,
	//	m_pAABB[BOUNDING_TRANSFORM]->Center.y + m_pAABB[BOUNDING_TRANSFORM]->Extents.y,
	//	m_pAABB[BOUNDING_TRANSFORM]->Center.z + m_pAABB[BOUNDING_TRANSFORM]->Extents.z);

	//vDestMin = _float3(pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]->Center.x - pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]->Extents.x,
	//	pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]->Center.y - pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]->Extents.y,
	//	pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]->Center.z - pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]->Extents.z);

	//vDestMax = _float3(pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]->Center.x + pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]->Extents.x,
	//	pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]->Center.y + pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]->Extents.y,
	//	pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]->Center.z + pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]->Extents.z);

	//m_isCollision = true;

	////	min중에 큰게 max중에 작은거보다 크면 충돌이 아님
	//if (max(vSourMin.x, vDestMin.x) > min(vSourMax.x, vDestMax.x))
	//	m_isCollision = false;

	//if (max(vSourMin.y, vDestMin.y) > min(vSourMax.y, vDestMax.y))
	//	m_isCollision = false;

	//if (max(vSourMin.z, vDestMin.z) > min(vSourMax.z, vDestMax.z))
	//	m_isCollision = false;

	return m_isCollision;
}

_bool CCollider::Collision_OBB(CCollider * pTargetCollider)
{
	if (nullptr == m_pOBB[BOUNDING_TRANSFORM])
		return false;

	 //	Intersects 함수로 충돌구현-------------------
	//m_isCollision = false;
	//if (nullptr != pTargetCollider->m_pAABB[BOUNDING_TRANSFORM])
	//{
	//	if (true == m_pOBB[BOUNDING_TRANSFORM]->Intersects(*pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]))
	//		m_isCollision = true;
	//}
	//else
	//{
	//	if (true == m_pOBB[BOUNDING_TRANSFORM]->Intersects(*pTargetCollider->m_pOBB[BOUNDING_TRANSFORM]))
	//		m_isCollision = true;
	//}	

	//	직접 충돌구현--------------------------------
	 OBBDESC			OBBDesc[2];

	 OBBDesc[0] = Compute_OBBDesc();
	 OBBDesc[1] = pTargetCollider->Compute_OBBDesc();

	 m_isCollision = true;

	 _float		fDistance[3];

	 //	OBB.png 참고
	 for (_uint i = 0; i < 2; ++i)
	 {
		 for (_uint j = 0; j < 3; ++j)
		 {
			/*
			센터와 센터를 투영시킨 초록색
			하늘색 2개 구해야함
			x축에 투영시킨거를 찾아야해
			0번째꺼는 작은 박스의 중심점 중심점을 다 수직? 투영? 시킨거
			oBBDesc의 0번째의 vCenterAxis를 투영시킨 길이를 구해야해요
			0번째의 있는 AlignAxis0번째로 내적해주면 되요 Dot, 투영시킨 길이 하나를 구했어요
			바라보고 있는거, z 축에 있는거 길이를 다 합하면 되요
			음수길이라 의미가 없어, 내적을 할때마다 해줘야하니 fabs로 감싸줘
			*/
			 fDistance[0] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[0]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				 fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[1]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				 fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[2]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j]))));

			 fDistance[1] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[0]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				 fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[1]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				 fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[2]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j]))));

			 fDistance[2] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&pTargetCollider->m_pOBB[BOUNDING_TRANSFORM]->Center) -
				 XMLoadFloat3(&m_pOBB[BOUNDING_TRANSFORM]->Center), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j]))));

			 if (fDistance[0] + fDistance[1] < fDistance[2])
			 {
				 m_isCollision = false;
				 break;
			 }
		 }
		 if (false == m_isCollision)
			 break;
	 }

	return m_isCollision;
}

_bool CCollider::Collision_Sphere(CCollider * pTargetCollider)
{
	if (nullptr == m_pSphere[BOUNDING_TRANSFORM])
		return false;

	if (nullptr == pTargetCollider)
		return false;

	//	Intersects 함수로 충돌구현-------------------
	m_isCollision = false;

	if (nullptr != pTargetCollider->m_pAABB[BOUNDING_TRANSFORM])
	{
		if (true == m_pSphere[BOUNDING_TRANSFORM]->Intersects(*pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]))
			m_isCollision = true;
	}
	if (nullptr != pTargetCollider->m_pOBB[BOUNDING_TRANSFORM])
	{
		if (true == m_pSphere[BOUNDING_TRANSFORM]->Intersects(*pTargetCollider->m_pOBB[BOUNDING_TRANSFORM]))
			m_isCollision = true;
	}
	if (nullptr != pTargetCollider->m_pSphere[BOUNDING_TRANSFORM])
	{
		if (true == m_pSphere[BOUNDING_TRANSFORM]->Intersects(*pTargetCollider->m_pSphere[BOUNDING_TRANSFORM]))
			m_isCollision = true;
	}	

	return m_isCollision;
}

_bool CCollider::CollisionMonster_OBB(CCollider * pPlayerCollider, CCollider * pTargetCollider)
{
	if (nullptr == m_pOBB[BOUNDING_TRANSFORM])
		return false;

	//	Intersects 함수로 충돌구현-------------------
	m_isCollision = false;
	if (nullptr != pTargetCollider->m_pAABB[BOUNDING_TRANSFORM])
	{
		if (true == m_pOBB[BOUNDING_TRANSFORM]->Intersects(*pTargetCollider->m_pAABB[BOUNDING_TRANSFORM]))
			m_isCollision = true;
	}
	if (nullptr != pTargetCollider->m_pOBB[BOUNDING_TRANSFORM])
	{
		if (true == m_pOBB[BOUNDING_TRANSFORM]->Intersects(*pTargetCollider->m_pOBB[BOUNDING_TRANSFORM]))
			m_isCollision = true;
	}
	if (nullptr != pTargetCollider->m_pSphere[BOUNDING_TRANSFORM])
	{
		if (true == m_pOBB[BOUNDING_TRANSFORM]->Intersects(*pTargetCollider->m_pSphere[BOUNDING_TRANSFORM]))
			m_isCollision = true;
	}

	return m_isCollision;
}

CCollider::OBBDESC CCollider::Compute_OBBDesc()
{
	OBBDESC		OBBDesc;
	ZeroMemory(&OBBDesc, sizeof(OBBDESC));

	_float3		vPoint[8];
	m_pOBB[BOUNDING_TRANSFORM]->GetCorners(vPoint);

	//	OBB Compute_OBB.png 참고
	//	2번에서 3번을 빼면 x 축 평행
	//	2번에서 1번을 빼면 y 축 평행
	//	2번에서 6번을 뺴면 z 축 평행
	XMStoreFloat3(&OBBDesc.vAlignAxis[0], XMVector3Normalize(XMLoadFloat3(&vPoint[2]) - XMLoadFloat3(&vPoint[3])));
	XMStoreFloat3(&OBBDesc.vAlignAxis[1], XMVector3Normalize(XMLoadFloat3(&vPoint[2]) - XMLoadFloat3(&vPoint[1])));
	XMStoreFloat3(&OBBDesc.vAlignAxis[2], XMVector3Normalize(XMLoadFloat3(&vPoint[2]) - XMLoadFloat3(&vPoint[6])));

	XMStoreFloat3(&OBBDesc.vCenterAxis[0], (XMLoadFloat3(&vPoint[2]) + XMLoadFloat3(&vPoint[5])) * 0.5f - XMLoadFloat3(&m_pOBB[BOUNDING_TRANSFORM]->Center));
	XMStoreFloat3(&OBBDesc.vCenterAxis[1], (XMLoadFloat3(&vPoint[2]) + XMLoadFloat3(&vPoint[7])) * 0.5f - XMLoadFloat3(&m_pOBB[BOUNDING_TRANSFORM]->Center));
	XMStoreFloat3(&OBBDesc.vCenterAxis[2], (XMLoadFloat3(&vPoint[2]) + XMLoadFloat3(&vPoint[0])) * 0.5f - XMLoadFloat3(&m_pOBB[BOUNDING_TRANSFORM]->Center));

	return OBBDesc;
}

#ifdef _DEBUG

HRESULT CCollider::Render()
{
	//	NativeConstruct_Prototype DEBUG일때만 생성한 m_pInputLayout 넣어서 ID3D11InputLayout 세팅
	m_pDeviceContext->IASetInputLayout(m_pInputLayout);

	m_pBasicEffect->SetWorld(XMMatrixIdentity());

	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	m_pBasicEffect->SetView(XMLoadFloat4x4(&pPipeLine->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	m_pBasicEffect->SetProjection(XMLoadFloat4x4(&pPipeLine->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));

	RELEASE_INSTANCE(CPipeLine);

	//	패스는 무조건 하나라서 걍 디바이스컨텍스트 던져주면 됨
	m_pBasicEffect->Apply(m_pDeviceContext);

	//	충돌 안됐으면 초록색, 충돌 됐으면 빨강색
	_vector			vColliderColor = m_isCollision == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f);

	m_pBatch->Begin();

	if (nullptr != m_pAABB[BOUNDING_TRANSFORM])
		DX::Draw(m_pBatch, *m_pAABB[BOUNDING_TRANSFORM], vColliderColor);
	if (nullptr != m_pOBB[BOUNDING_TRANSFORM])
		DX::Draw(m_pBatch, *m_pOBB[BOUNDING_TRANSFORM], vColliderColor);
	if (nullptr != m_pSphere[BOUNDING_TRANSFORM])
		DX::Draw(m_pBatch, *m_pSphere[BOUNDING_TRANSFORM], vColliderColor);

	m_pBatch->End();

	return S_OK;
}
#endif // _DEBUG

_matrix CCollider::Remove_Rotation(_fmatrix TransformMatrix)
{
	_matrix		Matrix = TransformMatrix;

	Matrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[0]));
	Matrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[1]));
	Matrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[2]));

	return Matrix;
}

CCollider * CCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, TYPE eType)
{
	CCollider*	pInstance = new CCollider(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(eType)))
	{
		MSGBOX("Failed to Creating CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CCollider::Clone(void * pArg)
{
	CCollider*	pInstance = new CCollider(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

	for (_uint i = 0; i < BOUNDING_END; ++i)
	{
		Safe_Delete(m_pAABB[i]);
		Safe_Delete(m_pOBB[i]);
		Safe_Delete(m_pSphere[i]);
	}

	Safe_Release(m_pInputLayout);

#ifdef _DEBUG
	if (false == m_isCloned)
	{
		Safe_Delete(m_pBasicEffect);
		Safe_Delete(m_pBatch);
	}
#endif // _DEBUG
}
