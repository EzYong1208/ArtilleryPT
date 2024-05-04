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

public: /* GameInstance ���ο��� ����� �Լ��� �Լ��� */
	HRESULT			Reserve_Container(_uint iNumLevels); // ���� �������ڸ��� ȣ���ؼ� �纻 ��ü�� ���� �����̳ʸ� �����Ҵ�
	HRESULT			Clear_LevelUI(_uint iLevelIndex);

public: /* Client �� ������ �Լ��� */
	HRESULT			Add_UI_Prototype(const _tchar* pPrototypeTag, class CUI* pPrototype); // UI ���� �߰�
	HRESULT			Add_UI_Clone(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pCloneTag, void* pArg); // UI �纻 �߰�

	HRESULT			Add_Disposable_UI(class CUI* pUI);

	CUI*			Get_UI(_uint iLevelIndex, const _tchar* pCloneTag); // UI �纻�� Ž��

	HRESULT			UI_On(_uint iLevelIndex, const _tchar* pCloneTag);
	HRESULT			UI_Off(_uint iLevelIndex, const _tchar* pCloneTag);
	HRESULT			All_UI_Off(_uint iLevelIndex);

private:
	class CUI*		Find_UI_Prototype(const _tchar* pPrototypeTag);

private: /* ������� */
	map<const _tchar*, class CUI*>		m_UI_Prototypes; /* UI ���� ���� */
	map<const _tchar*, class CUI*>*		m_UI_Clone;		 /* UI �纻 ����(�������� �����Ҵ� �Ұ���) */

	vector<class CUI*>					m_Disposable_UI; /* ��ȸ�� UI �� ��Ƴ��� �� (������, ��ȭâ ��) */

	_uint								m_iNumLevels;

public:
	virtual void	Free() override;
};

END