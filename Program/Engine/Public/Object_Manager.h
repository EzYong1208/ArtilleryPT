#pragma once

#include "Base.h"

//	��ü���� ������ �����ؼ� ��Ƴ��´�, ������ ã�Ƽ� ������ �� ��Ƽ� �����Ѵ�

//	������Ÿ�� ������ ����
//	-> ��ü ���� �ӵ��� ������ �ϱ� ����
//	�̸� ���� ��ü�� ����� �ΰ� �����ؼ� ���, �ε����� ���� ��ü���� ����� �� ��
//	�ε����� ���� ��ü ���� - �ΰ��ӿ��� ���Ͱ� �ʿ��� ���� - ����� ���� ���� ��ü�� ������?

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)

public:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	class CComponent*	Get_Component(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex);
	class CGameObject*	Get_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, _uint iIndex); // �纻 ���ӿ�����Ʈ�� ��ȯ
	class CLayer*		Get_Layer(_uint iLevelIndex, const _tchar* pLayerTag); // �̰� ImGui �� Collision_Manager ���� ������

public:
	HRESULT				Reserve_Container(_uint iNumLevels);
	//	���� ��ü�� �����̳ʿ� �߰��ϴ� ���
	HRESULT				Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);	//	���� ��ü, �±�(Ű��)
																									//	�纻(���ӳ��� �⿬�ؾ� ��) ��ü�� �����̳ʿ� �߰��ϴ� ���
	HRESULT				Add_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg);	//	�±�(Ű��), �����ε���, ������Ÿ�� �±�(������ü���� �˻��ϰ� �����ؼ� �߰��ҷ���)
																																//	���� ����°��� �ƴ� �纻 ��ü�� ����� �Լ�
	HRESULT				Clear_LevelObject(_uint iLevelIndex);

public:
	_int				Tick(_double TimeDelta);
	_int				LateTick(_double TimeDelta);

private:
	//	������ �����ϰ� ������ map �����̳�
	map<const _tchar*, class CGameObject*>			m_Prototypes;
	typedef map<const _tchar*, class CGameObject*>	PROTOTYPES;

private:
	//	���� ������ ����� ������ map �����̳�
	//	� �̸��� ����(const _tchar*) ��ü���̴�(list<class CGameObject*>)
	//	stl �����̳� �ȿ� �����̳ʸ� ���� �����ϴ� ���� ���� �����Ƿ� CLayer*�� ����
	//	CLayer Ŭ������ ���� �ű⿡ list m_pObject �����̳�
	map<const _tchar*, class CLayer*>*				m_pLayers = nullptr;	//	�����迭�� �����ͷ� ����ϰڴ�
	typedef map<const _tchar*, class CLayer*>		LAYERS;

	_uint				m_iNumLevels = 0;

private:
	//	�ߺ��� Ű���� �ִ��� üũ�ϴ� �Լ�
	class CGameObject*	Find_PrototypeTag(const _tchar* pPrototypeTag);
	//	�ʿ� �ִ� � ���̾�(� ����Ʈ)���ٰ� �־������ ���� ã�ƺ��� �Լ�, �迭�� ��������� iLevelIndex
	class CLayer*		Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);


public:
	virtual	void		Free() override;

#if defined (USE_IMGUI)
public: /* ImGui ���� ����� �Լ���*/
	map<const _tchar*, class CGameObject*>& Get_Prototypes_Map() { return m_Prototypes; }
	map<const _tchar*, class CLayer*>*		Get_Layers_Map() { return m_pLayers; }
#endif
};

END