#pragma once

#include "Base.h"
#include "GameObject.h"
#include "Layer.h"
#include "Collider.h"

BEGIN(Engine)

class CCollision_Manager final : public CBase
{
	DECLARE_SINGLETON(CCollision_Manager)

private:
	CCollision_Manager();
	virtual ~CCollision_Manager() = default;

public:
	HRESULT				Collision_AABB(_tchar* pSourTag, _tchar* pDestTag);

public:
	CCollider*			m_pColliderCom = nullptr;

public:
	virtual void		Free() override;
};

END