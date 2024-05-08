#pragma once

//	������ ���� �߰��� ��� �������� �߰��� ��
#include "VIBuffer_Rect.h"
#include "Navigation.h"
#include "Transform.h"
#include "Renderer.h"
#include "Texture.h"
#include "Shader.h"

//	������Ʈ �������� ��Ƴ��´�
//	������Ʈ �Ŵ����� ����غ������� ������Ʈ �Ŵ����� �����鸸 ��Ƴ���, �纻�� �� ��ü���� �����Ұ���!

BEGIN(Engine)

class CComponent_Manager final : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)

protected:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	typedef	struct tagRenderDesc
	{
		_tchar	Name[MAX_PATH];
		_uint	iLevelIndex;
	}RENDERDESC;

public:
	HRESULT				Reserve_Container(_uint iNumLevels);
	HRESULT				Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	//	������Ʈ �Ŵ������� �����ؼ� �������� �Լ�
	//	iLevelIndex : � �迭�� ���ִ��� Ȯ���ҷ���
	CComponent*			Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg);
	//	ObjMgr Clear_LevelObject()�� �Ȱ���
	HRESULT				Clear_LevelObject(_uint iLevelIndex);

	HRESULT				AddRenderDesc(_uint iLevelIndex);

private:
	//	map �����̳� -> �����迭�� ����
	map<const _tchar*, class CComponent*>*			m_pPrototypes = nullptr;
	typedef map<const _tchar*, class CComponent*>	PROTOTYPES;

private:
	_uint				m_iNumLevels;
	RENDERDESC			m_RenderDesc;

private:
	class CComponent*	Find_Component(_uint iLevelIndex, const _tchar* pPrototypeTag);

public:
	virtual	void		Free() override;

#if defined (USE_IMGUI)
public: /* ImGui ���� ����� �Լ���*/
	map<const _tchar*, class CComponent*>* Get_Prototype_Components() { return m_pPrototypes; }
#endif
};

END