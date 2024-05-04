#pragma once

#include "Base.h"

BEGIN(Engine)

class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)

private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	_int			Tick(_double TimeDelta);
	_int			LateTick(_double TimeDelta);

public: /* GameInstance 내부에서 사용할 함수들 함수들 */
	HRESULT			Reserve_Container(_uint iNumLevels); // 게임 시작하자마자 호출해서 사본 객체를 담을 컨테이너를 동적할당
	HRESULT			Clear_LevelUI(_uint iLevelIndex);

public: /* Client 에 보여줄 함수들 */
	HRESULT			Add_UI_Prototype(const _tchar* pPrototypeTag, class CUI* pPrototype); // UI 원형 추가
	HRESULT			Add_UI_Clone(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pCloneTag, void* pArg); // UI 사본 추가

	HRESULT			Add_Disposable_UI(class CUI* pUI);

	CUI*			Get_UI(_uint iLevelIndex, const _tchar* pCloneTag); // UI 사본을 탐색

	HRESULT			UI_On(_uint iLevelIndex, const _tchar* pCloneTag);
	HRESULT			UI_Off(_uint iLevelIndex, const _tchar* pCloneTag);
	HRESULT			All_UI_Off(_uint iLevelIndex);

private:
	class CUI*		Find_UI_Prototype(const _tchar* pPrototypeTag);

private: /* 멤버변수 */
	map<const _tchar*, class CUI*>		m_UI_Prototypes; /* UI 원형 보관 */
	map<const _tchar*, class CUI*>*		m_UI_Clone;		 /* UI 사본 보관(레벨별로 동적할당 할거임) */

	vector<class CUI*>					m_Disposable_UI; /* 일회성 UI 들 모아놓는 곳 (데미지, 대화창 등) */

	_uint								m_iNumLevels;

public:
	virtual void	Free() override;
};

END