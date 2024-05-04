#pragma once

#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
public:
	CLayer();
	virtual	~CLayer() = default;

public:
	class CComponent*	Get_Component(const _tchar* pComponentTag, _uint iIndex);
	class CGameObject*	Get_GameObject(_uint iIndex);

public:
	//	Layer에 Add_GameObject 할때는 레퍼런스 카운트 증가X
	HRESULT				Add_GameObject(class CGameObject* pGameObject);
	_int				Tick(_double TimeDelta);
	_int				LateTick(_double TimeDelta);

public:
	list<class CGameObject*>& Get_Objects() { return m_Objects; }

private:
	list<class CGameObject*>			m_Objects;
	typedef	list<class CGameObject*>	OBJECTS;

public:
	static	CLayer*		Create();
	virtual	void		Free();
};

END