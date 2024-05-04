#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };
	enum BOUNDING { BOUNDING_ORIGINAL, BOUNDING_TRANSFORM, BOUNDING_END };

public:
	typedef struct tagOBBDesc
	{
		_float3		vCenterAxis[3];		//	센터와 센터를 잇는 벡터
		_float3		vAlignAxis[3];		//	평행한 축
	}OBBDESC;

public:
	//	초기상태에서 얼마나 더
	typedef struct tagColliderDesc
	{
		_float3 vScale;			//	배수(절대적인 사이즈가 아님, 대신 우리는 다 1로 잡아둠)
		_float4 vRotation;		//	OBB에게만 적용될거임
		_float4 vTranslation;	//	이동 값
	} COLLIDERDESC;

private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT			NativeConstruct_Prototype(TYPE eType);
	virtual HRESULT			NativeConstruct(void* pArg);

	void					Update_Transform(_fmatrix TransformMatrix);

public:
	_bool					Collision_AABB(CCollider* pTargetCollider);
	_bool					Collision_OBB(CCollider* pTargetCollider);
	_bool					Collision_Sphere(CCollider* pTargetCollider);

	_bool					CollisionMonster_OBB(CCollider * pPlayerCollider, CCollider* pTargetCollider);

	OBBDESC					Compute_OBBDesc();

#ifdef _DEBUG
public:
	virtual HRESULT			Render();
#endif // _DEBUG

private:
	//	회전되지 않은 박스
	BoundingBox*			m_pAABB[BOUNDING_END] = { nullptr };
	//	회전된, 다양성
	BoundingOrientedBox*	m_pOBB[BOUNDING_END] = { nullptr };
	//	구
	BoundingSphere*			m_pSphere[BOUNDING_END] = { nullptr };

	TYPE					m_eType = TYPE_END;
	COLLIDERDESC			m_ColliderDesc;
	//	충돌이 됐느냐 안됐느냐
	_bool					m_isCollision = false;

#ifdef _DEBUG
private:
	//	기본 버퍼, 정점들을 그릴 수 있는 셰이더를 제공함
	BasicEffect*									m_pBasicEffect = nullptr;
	ID3D11InputLayout*								m_pInputLayout = nullptr;
	//	Draw 함수를 사용할때 필요함, 내가 직접 정의하지 않아도 기본적인 도형을 그려나갈 수 있는 정점버퍼, 인덱스 버퍼를 제공함
	PrimitiveBatch<DirectX::VertexPositionColor>*	m_pBatch = nullptr;
#endif // _DEBUG

private:
	//	움직일때 사이즈가 변하는 등의 변화를 삭제
	_matrix					Remove_Rotation(_fmatrix TransformMatrix);

public:
	static CCollider*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, TYPE eType);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};

END