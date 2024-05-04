#pragma once

#include "Component.h"

//	화면에(인게임 내에) 그려져야 하는 객체들을 모아서 보관(그려지는 순서대로)한다
//	객체들의 그리는 함수(Render)를 호출해준다

//	게임 내에서 여러번 호출되면 안됨, 싱글턴으로 짜도 상관은 없다
//	복사 생성자는 정의할 생각 X, 복사할 생각이 X

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	//	PRIORITY : 젤 먼저 그림(스카이박스), NONALPHA : 불투명한 애들, ALPHA : 투명, UI : 가장 마지막에 그림(깊이 비교를 하지 않는다)
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_NONBLEND, RENDER_BLEND, RENDER_UI, RENDER_END };

protected:
	explicit CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CRenderer() = default;

public:
	virtual	HRESULT			NativeConstruct_Prototype() override;
	virtual	HRESULT			NativeConstruct(void* pArg) override;

public:
	//	몇번째 그룹에 넣는지 중요하니 eRenderGroup, 그릴려는 객체 pRenderObject
	HRESULT					Add_RenderGroup(RENDERGROUP	eRenderGroup, class CGameObject* pRenderObject);
	HRESULT					Render();

	HRESULT					Clear_LevelObject(_uint iLevelIndex);

private:
	list<class CGameObject*>			m_RenderObjects[RENDER_END];
	typedef list<class CGameObject*>	RENDEROBJECTS;

	_uint					m_iNumLevels = 0;

private:
	HRESULT					Render_Priority();
	HRESULT					Render_NonAlpha();
	HRESULT					Render_Alpha();
	HRESULT					Render_UI();

public:
	static	CRenderer*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	//	컴포넌트 자식으로 만들려고 Clone 함수 무조건 만들어야 함, 안에서 기능수행 안함
	virtual	CComponent*		Clone(void* pArg) override;
	virtual	void			Free() override;
};

END