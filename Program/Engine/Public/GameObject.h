#pragma once

#include "Base.h"

BEGIN(Engine)

#define	ALIVE 0
#define ISDEAD 4
#define	THEREISNONEXT 100

class CComponent;
class CCollider;
class ENGINE_DLL CGameObject abstract : public CBase		//	abstract: �߻�Ŭ����ȭ �ߴ�(���� ���������� �����Ҵ� �� �� ����)
{
//public:
//	typedef	enum eSoundState
//	{
//		ATTACK,
//
//	};

protected:
	explicit CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	//	���� ������
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	class CComponent*		Get_Component(const _tchar* pComponentTag);

public:
	//	������ü �ʱ�ȭ �Լ�
	virtual	HRESULT			NativeConstruct_Prototype();
	virtual HRESULT			NativeConstruct(void* pArg);
	virtual _int			Tick(_double TimeDelta);
	virtual _int			LateTick(_double TimeDelta);
	virtual HRESULT			Render();

public:
	//	�ڱ��� ��������� ���� ä���� �� �ִ� �Լ�
	HRESULT					Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, CComponent** ppOut, void* pArg = nullptr);

public:
	//	��������ó�� ����ҷ���
	static const _tchar*	m_pTransformTag;

protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

protected:
	//	��� ���ӿ�����Ʈ�鿡�� Transform�� �� �ְ�ʹ�
	//	������Ʈ �Ŵ����� Ʈ�������� ����, ��ü����ÿ� ������ Ʈ�������� �־���
	//	����Ƽ�� ��� ��ü�� Ʈ�������� ���⿡ �̷� ���·� �Ѵ�
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
	//	�߰��ϱ� ���� �ߺ��˻� �ϱ� ���� �Լ�
	CComponent*				Find_Component(const _tchar* pComponentTag);
	HRESULT					Compute_CamDistance(CTransform* pTransform);

protected:
	//	�� �ڽ� ���ӿ�����Ʈ�� ������Ʈ���� �׳� ��������� �ھƵδ� ���� ���ο��� ������ �����ϴٴ� ������ ����
	//	������ ������Ʈ���� ����ؼ� �þ��, �ٸ� ���ӿ�����Ʈ���� �� �ٸ� ���ӿ�����Ʈ�� ������Ʈ ������ ������ �� ������Ʈ���� Get �Լ��� ����� ���� ��ȿ������
	//	map ���� �����ϸ� �ٸ� ���ӿ�����Ʈ���� ������Ʈ�� Ž���ϱ� �������ϱ� map ���� ������Ʈ���� �����ϰ�, ��������ε� ������Ʈ�� �����ϱ�� ��
	//	map ���� �����ϴ� ���� ���� �ڽİ�ü�� �� �ʿ���� �θ�ü�� �� ���� �ϸ� �Ǵϱ� GameObject �� ������
	map<const _tchar*, CComponent*>				m_Components;
	typedef map<const _tchar*, CComponent*>		COMPONENTS;

public: /* ���� �����ӿ�ũ �� �߰��� ���������Լ� & �����Լ� */
	virtual HRESULT Save_Data(HANDLE hFile) = 0; // �� �ڽ� Ŭ���� ���ο��� �����ϰ���� �����͸� �����ؼ� ����
	virtual HRESULT Load_Data(HANDLE hFile) = 0; // �� �ڽ� Ŭ���� ���ο��� �ҷ��� �����͸� �����ؼ� �ҷ���

public:
	//	������ ��ü�� ������ �Լ�
	virtual CGameObject*	Clone(void* pArg) = 0;
	virtual	void			Free() override;
};

END