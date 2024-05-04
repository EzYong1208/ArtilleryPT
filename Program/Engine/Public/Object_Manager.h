#pragma once

#include "Base.h"

//	객체들의 원형을 생성해서 모아놓는다, 원형을 찾아서 복제한 후 모아서 관리한다

//	프로토타입 디자인 패턴
//	-> 객체 생성 속도를 빠르게 하기 위해
//	미리 원형 객체를 만들어 두고 복제해서 사용, 로딩동안 원형 객체들을 만들어 둘 것
//	로딩동안 원형 객체 생성 - 인게임에서 몬스터가 필요한 순간 - 만들어 놓은 원형 객체를 복사함?

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)

public:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	class CComponent*	Get_Component(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex);
	class CGameObject*	Get_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, _uint iIndex); // 사본 게임오브젝트를 반환
	class CLayer*		Get_Layer(_uint iLevelIndex, const _tchar* pLayerTag); // 이거 ImGui 랑 Collision_Manager 에서 쓸거임

public:
	HRESULT				Reserve_Container(_uint iNumLevels);
	//	원형 객체를 컨테이너에 추가하는 기능
	HRESULT				Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);	//	넣을 객체, 태그(키값)
																									//	사본(게임내에 출연해야 할) 객체를 컨테이너에 추가하는 기능
	HRESULT				Add_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg);	//	태그(키값), 레벨인덱스, 프로토타입 태그(원형객체들을 검색하고 복사해서 추가할려고)
																																//	원형 지우는것이 아닌 사본 객체를 지우는 함수
	HRESULT				Clear_LevelObject(_uint iLevelIndex);

public:
	_int				Tick(_double TimeDelta);
	_int				LateTick(_double TimeDelta);

private:
	//	원형을 생성하고 보관할 map 컨테이너
	map<const _tchar*, class CGameObject*>			m_Prototypes;
	typedef map<const _tchar*, class CGameObject*>	PROTOTYPES;

private:
	//	실제 복제한 놈들을 보관할 map 컨테이너
	//	어떤 이름을 가진(const _tchar*) 객체들이다(list<class CGameObject*>)
	//	stl 컨테이너 안에 컨테이너를 직접 참조하는 것은 좋지 않으므로 CLayer*로 감쌈
	//	CLayer 클래스를 만들어서 거기에 list m_pObject 컨테이너
	map<const _tchar*, class CLayer*>*				m_pLayers = nullptr;	//	동적배열의 포인터로 사용하겠다
	typedef map<const _tchar*, class CLayer*>		LAYERS;

	_uint				m_iNumLevels = 0;

private:
	//	중복된 키값이 있는지 체크하는 함수
	class CGameObject*	Find_PrototypeTag(const _tchar* pPrototypeTag);
	//	맵에 있는 어떤 레이어(어떤 리스트)에다가 넣어야할지 먼저 찾아보는 함수, 배열로 만들었으니 iLevelIndex
	class CLayer*		Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);


public:
	virtual	void		Free() override;

#if defined (USE_IMGUI)
public: /* ImGui 에서 사용할 함수들*/
	map<const _tchar*, class CGameObject*>& Get_Prototypes_Map() { return m_Prototypes; }
	map<const _tchar*, class CLayer*>*		Get_Layers_Map() { return m_pLayers; }
#endif
};

END