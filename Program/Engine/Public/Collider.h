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
		_float3		vCenterAxis[3];		//	���Ϳ� ���͸� �մ� ����
		_float3		vAlignAxis[3];		//	������ ��
	}OBBDESC;

public:
	//	�ʱ���¿��� �󸶳� ��
	typedef struct tagColliderDesc
	{
		_float3 vScale;			//	���(�������� ����� �ƴ�, ��� �츮�� �� 1�� ��Ƶ�)
		_float4 vRotation;		//	OBB���Ը� ����ɰ���
		_float4 vTranslation;	//	�̵� ��
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
	//	ȸ������ ���� �ڽ�
	BoundingBox*			m_pAABB[BOUNDING_END] = { nullptr };
	//	ȸ����, �پ缺
	BoundingOrientedBox*	m_pOBB[BOUNDING_END] = { nullptr };
	//	��
	BoundingSphere*			m_pSphere[BOUNDING_END] = { nullptr };

	TYPE					m_eType = TYPE_END;
	COLLIDERDESC			m_ColliderDesc;
	//	�浹�� �ƴ��� �ȵƴ���
	_bool					m_isCollision = false;

#ifdef _DEBUG
private:
	//	�⺻ ����, �������� �׸� �� �ִ� ���̴��� ������
	BasicEffect*									m_pBasicEffect = nullptr;
	ID3D11InputLayout*								m_pInputLayout = nullptr;
	//	Draw �Լ��� ����Ҷ� �ʿ���, ���� ���� �������� �ʾƵ� �⺻���� ������ �׷����� �� �ִ� ��������, �ε��� ���۸� ������
	PrimitiveBatch<DirectX::VertexPositionColor>*	m_pBatch = nullptr;
#endif // _DEBUG

private:
	//	�����϶� ����� ���ϴ� ���� ��ȭ�� ����
	_matrix					Remove_Rotation(_fmatrix TransformMatrix);

public:
	static CCollider*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, TYPE eType);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};

END