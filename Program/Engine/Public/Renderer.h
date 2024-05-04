#pragma once

#include "Component.h"

//	ȭ�鿡(�ΰ��� ����) �׷����� �ϴ� ��ü���� ��Ƽ� ����(�׷����� �������)�Ѵ�
//	��ü���� �׸��� �Լ�(Render)�� ȣ�����ش�

//	���� ������ ������ ȣ��Ǹ� �ȵ�, �̱������� ¥�� ����� ����
//	���� �����ڴ� ������ ���� X, ������ ������ X

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	//	PRIORITY : �� ���� �׸�(��ī�̹ڽ�), NONALPHA : �������� �ֵ�, ALPHA : ����, UI : ���� �������� �׸�(���� �񱳸� ���� �ʴ´�)
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_NONBLEND, RENDER_BLEND, RENDER_UI, RENDER_END };

protected:
	explicit CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CRenderer() = default;

public:
	virtual	HRESULT			NativeConstruct_Prototype() override;
	virtual	HRESULT			NativeConstruct(void* pArg) override;

public:
	//	���° �׷쿡 �ִ��� �߿��ϴ� eRenderGroup, �׸����� ��ü pRenderObject
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
	//	������Ʈ �ڽ����� ������� Clone �Լ� ������ ������ ��, �ȿ��� ��ɼ��� ����
	virtual	CComponent*		Clone(void* pArg) override;
	virtual	void			Free() override;
};

END