#pragma once

//	앞으로 내가 추가한 모든 원형들을 추가할 것
#include "VIBuffer_Rect.h"
#include "Navigation.h"
#include "Transform.h"
#include "Renderer.h"
#include "Texture.h"
#include "Shader.h"

//	컴포넌트 원형들을 모아놓는다
//	오브젝트 매니저와 비슷해보이지만 컴포넌트 매니저는 원형들만 모아놓음, 사본은 각 객체들이 보관할것임!

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
	//	컴포넌트 매니저에서 복제해서 가져오는 함수
	//	iLevelIndex : 어떤 배열에 들어가있는지 확인할려고
	CComponent*			Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg);
	//	ObjMgr Clear_LevelObject()랑 똑같음
	HRESULT				Clear_LevelObject(_uint iLevelIndex);

	HRESULT				AddRenderDesc(_uint iLevelIndex);

private:
	//	map 컨테이너 -> 동적배열로 관리
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
public: /* ImGui 에서 사용할 함수들*/
	map<const _tchar*, class CComponent*>* Get_Prototype_Components() { return m_pPrototypes; }
#endif
};

END