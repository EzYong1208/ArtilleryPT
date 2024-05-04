#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNaviDesc
	{
		//	이웃이 없을 때 -1
		_uint		iCurrentIndex = -1;
		_uint		iCurrentLevel = 0;
	}NAVIDESC;

private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual	HRESULT			NativeConstruct_Prototype(const _tchar* pNaviDataFilePath, _uint iLevelIndex);
	virtual	HRESULT			NativeConstruct(void* pArg) override;

public:
	_bool					Move_OnNavigation(_fvector vPosition);

public:
	_vector					Get_Height(_fvector vPosition);
	_uint					Get_CurrentIndex() { return m_NaviDesc.iCurrentIndex; }
	void					Set_CurrentIndex(_uint iIndex) { m_NaviDesc.iCurrentIndex = iIndex; }

//#ifdef _DEBUG
//public:
//	HRESULT					Render(class CTransform* pTerrainTransform = nullptr);
//#endif // _DEBUG

private:
	vector<class CCell*>			m_Cells;
	typedef	vector<class CCell*>	CELLS;

//#ifdef _DEBUG
//private:
//	class CVIBuffer_Triangle*		m_pVIBuffer = nullptr;
//	class CShader*					m_pShader = nullptr;
//#endif // _DEBUG

private:
	NAVIDESC						m_NaviDesc;

private:
	HRESULT					SetUp_Neighbor();

private:
	//{ LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_STAGE01, LEVEL_STAGE02, LEVEL_BOSS, LEVEL_TEST, LEVEL_END };
	_uint							m_iLevelStage01 = 3;
	_uint							m_iLevelStage02 = 4;
	_uint							m_iLevelBoss = 5;
	_uint							m_iLevelTest = 6;

	_uint							m_iNeighborIndex = 0;
	_uint							m_iCurrentLevel = 0;

	HANDLE							m_hFile;
	_ulong							m_dwByte = 0;

	_bool							m_bSwitchWallB = true;
	_bool							m_bSwitchWallC = false;

public:
	void					Set_SwitchWallB(_bool bMove);
	void					Set_SwitchWallC(_bool bMove);

public:
	static CNavigation*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pNaviDataFilePath, _uint iLevelIndex);
	virtual	CComponent*		Clone(void* pArg) override;
	virtual	void			Free() override;
};

END