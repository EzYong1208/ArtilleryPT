#pragma once

#include "Base.h"

BEGIN(Engine)

#define	ALIVE 0
#define ISDEAD 4
#define	THEREISNONEXT 100

class CComponent;
class CCollider;
class ENGINE_DLL CGameObject abstract : public CBase		//	abstract: 추상클래스화 했다(절대 독립적으로 동적할당 할 수 없음)
{
//public:
//	typedef	enum eSoundState
//	{
//		ATTACK,
//
//	};

protected:
	explicit CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	//	복사 생성자
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	class CComponent*		Get_Component(const _tchar* pComponentTag);

public:
	//	원형객체 초기화 함수
	virtual	HRESULT			NativeConstruct_Prototype();
	virtual HRESULT			NativeConstruct(void* pArg);
	virtual _int			Tick(_double TimeDelta);
	virtual _int			LateTick(_double TimeDelta);
	virtual HRESULT			Render();

public:
	//	자기의 멤버변수에 값을 채워줄 수 있는 함수
	HRESULT					Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, CComponent** ppOut, void* pArg = nullptr);

public:
	//	전역변수처럼 사용할려고
	static const _tchar*	m_pTransformTag;

protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

protected:
	//	모든 게임오브젝트들에게 Transform을 다 주고싶다
	//	컴포넌트 매니저에 트랜스폼은 없고, 객체선언시에 무조건 트랜스폼이 넣어짐
	//	유니티는 모든 객체에 트랜스폼이 들어가기에 이런 형태로 한다
	class CTransform*		m_pTransform = nullptr;

protected:
	_uint					m_iHit = 0;
	_uint					m_iCurrentLevel = 0;
	_float					m_fCamDistance;

public:
	HRESULT					Hitted();
	_uint					Get_Hit() { return m_iHit; }
	_float					Get_CamDistance() const { return m_fCamDistance; }

protected:
	//	추가하기 전에 중복검사 하기 위한 함수
	CComponent*				Find_Component(const _tchar* pComponentTag);
	HRESULT					Compute_CamDistance(CTransform* pTransform);

protected:
	//	각 자식 게임오브젝트에 컴포넌트들을 그냥 멤버변수로 박아두는 것은 내부에서 접근이 용이하다는 장점이 있음
	//	하지만 컴포넌트들이 계속해서 늘어나고, 다른 게임오브젝트에서 또 다른 게임오브젝트의 컴포넌트 정보를 얻고싶을 때 컴포넌트마다 Get 함수를 만드는 것은 비효율적임
	//	map 으로 보관하면 다른 게임오브젝트에서 컴포넌트를 탐색하기 편해지니까 map 으로 컴포넌트들을 보관하고, 멤버변수로도 컴포넌트를 보관하기로 함
	//	map 으로 보관하는 것은 굳이 자식객체에 할 필요없이 부모객체에 한 번에 하면 되니까 GameObject 에 선언함
	map<const _tchar*, CComponent*>				m_Components;
	typedef map<const _tchar*, CComponent*>		COMPONENTS;

public: /* 수업 프레임워크 외 추가한 순수가상함수 & 가상함수 */
	virtual HRESULT Save_Data(HANDLE hFile) = 0; // 각 자식 클래스 내부에서 저장하고싶은 데이터를 선택해서 저장
	virtual HRESULT Load_Data(HANDLE hFile) = 0; // 각 자식 클래스 내부에서 불러올 데이터를 선택해서 불러옴

public:
	//	복제된 객체를 리턴할 함수
	virtual CGameObject*	Clone(void* pArg) = 0;
	virtual	void			Free() override;
};

END