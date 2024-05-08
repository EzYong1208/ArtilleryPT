#include "..\Public\GameInstance.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device(CGraphic_Device::GetInstance())
	, m_pInput_Device(CInput_Device::GetInstance())
	, m_pTimer_Manager(CTimer_Manager::GetInstance())
	, m_pComponent_Manager(CComponent_Manager::GetInstance())
	, m_pLevel_Manager(CLevel_Manager::GetInstance())
	, m_pObject_Manager(CObject_Manager::GetInstance())
	, m_pPipeLine(CPipeLine::GetInstance())
	, m_pLight_Manager(CLight_Manager::GetInstance())
	, m_pKey_Manager(CKey_Manager::GetInstance())
	, m_pUI_Manager(CUI_Manager::GetInstance())
	, m_pCollision_Manager(CCollision_Manager::GetInstance())
	, m_pFrustum(CFrustum::GetInstance())
	, m_pFont_Manager(CFont_Manager::GetInstance())
{
	Safe_AddRef(m_pFont_Manager);
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pCollision_Manager);
	Safe_AddRef(m_pUI_Manager);
	Safe_AddRef(m_pKey_Manager);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pObject_Manager);

	_AResourceManager = make_unique<AResourceManager>();
}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInstance, _uint iNumLevels, const CGraphic_Device::GRAPHICDESC & GraphicDesc, ID3D11Device ** ppDeviceOut, ID3D11DeviceContext ** ppDeviceContextOut)
{
	if (nullptr == m_pGraphic_Device ||
		nullptr == m_pObject_Manager)
		return E_FAIL;

	//	장치 초기화
	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iWinCX, GraphicDesc.iWinCY, ppDeviceOut, ppDeviceContextOut)))
		return E_FAIL;

	//	입력 장치 초기화
	if (FAILED(m_pInput_Device->Ready_Input_Device(hInstance, GraphicDesc.hWnd)))
		return E_FAIL;

	//	엔진 초기화시에 바로 오브젝트매니저 reserve
	if (FAILED(m_pObject_Manager->Reserve_Container(iNumLevels)))
		return E_FAIL;

	//	UI매니저 레벨별로 동적할당
	if (FAILED(m_pUI_Manager->Reserve_Container(iNumLevels)))
		return E_FAIL;

	//	엔진 초기화시에 바로 컴포넌트매니저 reserve
	if (FAILED(m_pComponent_Manager->Reserve_Container(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pFrustum->Initialize()))
		return E_FAIL;

	//Render prototype create
	// 여기서 create하는거 하고(intialize), 그 다음에 릴리즈도 해야함(밑에서)
	//CLayer* pLayer = Find_Layer(iLevelIndex, pLayerTag);
	// 여기 말고 MainApp에서 해보기 고려
	//string TextureKey = "";
	//_float4x4 Transform;
	//ZeroMemory(&Transform, sizeof(_float4x4));
	//CRenderObject* pRenderObject = CRenderObject::Create(*ppDeviceOut, *ppDeviceContextOut, TextureKey, Transform);


	return S_OK;
}

_int CGameInstance::Tick_Engine(_double TimeDelta)
{
	if (nullptr == m_pLevel_Manager ||
		nullptr == m_pObject_Manager ||
		nullptr == m_pFrustum)
		return -1;

	if (FAILED(m_pInput_Device->SetUp_InputDeviceState()))
		return -1;

	if (0 > m_pObject_Manager->Tick(TimeDelta))
		return -1;

	if (0 > m_pLevel_Manager->Tick(TimeDelta))
		return -1;

	//	카메라 틱에서 해당 처리를 다 해주고
	//	파이프라인 업데이트에서 카메라위치를 알아내고
	//	그걸 바탕으로 오브젝트 매니저 레이트틱에서 불러와서 사용할수 있게 오브젝트매니저 레이트틱 전에 호출
	m_pPipeLine->Update();

	m_pFrustum->Update();

	if (0 > m_pUI_Manager->Tick(TimeDelta))
		return -1;

	//------------------------------


	if (0 > m_pObject_Manager->LateTick(TimeDelta))
		return -1;

	if (0 > m_pLevel_Manager->LateTick(TimeDelta))
		return -1;

	if (0 > m_pUI_Manager->LateTick(TimeDelta))
		return -1;

	return _int();
}

HRESULT CGameInstance::Clear_LevelResource(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager ||
		nullptr == m_pComponent_Manager)
		return E_FAIL;


	if (FAILED(m_pObject_Manager->Clear_LevelObject(iLevelIndex)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Clear_LevelObject(iLevelIndex)))
		return E_FAIL;


	if (FAILED(m_pUI_Manager->Clear_LevelUI(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

_byte CGameInstance::Get_DIKeyState(_ubyte eKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKeyState(eKeyID);
}

_long CGameInstance::Get_DIMouseMoveState(CInput_Device::MOUSEMOVESTATE eMouseMoveState)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseMoveState(eMouseMoveState);
}

_byte CGameInstance::Get_DIMouseButtonState(CInput_Device::MOUSEBUTTONSTATE eMouseButtonState)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseButtonState(eMouseButtonState);
}

_float CGameInstance::Get_TimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.f;

	return (_float)m_pTimer_Manager->Get_TimeDelta(pTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(pTimerTag);
}

HRESULT CGameInstance::OpenLevel(_uint iLevelIndex, CLevel * pNextLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->OpenLevel(iLevelIndex, pNextLevel);
}

HRESULT CGameInstance::Render_Level()
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Render();
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype);
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
}

HRESULT CGameInstance::AddRenderDesc(_uint iLevelIndex)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->AddRenderDesc(iLevelIndex);
}

CComponent * CGameInstance::Get_Component(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Component(iLevelIndex, pLayerTag, pComponentTag, iIndex);
}

HRESULT CGameInstance::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Add_GameObject(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObject(iLevelIndex, pLayerTag, pPrototypeTag, pArg);
}

CGameObject * CGameInstance::Get_GameObject(_uint iLevelIndex, const _tchar * pLayerTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_GameObject(iLevelIndex, pLayerTag, iIndex);
}

CLayer * CGameInstance::Get_Layer(_uint iLevelIndex, const _tchar * pLayerTag)
{
	return m_pObject_Manager->Get_Layer(iLevelIndex, pLayerTag);
}

HRESULT CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATETYPE eStateType, _fmatrix TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	return m_pPipeLine->Set_Transform(eStateType, TransformMatrix);
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::TRANSFORMSTATETYPE eStateType)
{
	//	잘못됐다면 항등행렬을 리턴
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrix(eStateType);
}

_float4x4 CGameInstance::Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATETYPE eStateType)
{
	//	잘못됐으면 그냥 _float4x4 리턴
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4(eStateType);
}

_float4x4 CGameInstance::Get_TransformFloat4x4_TP(CPipeLine::TRANSFORMSTATETYPE eStateType)
{
	//	잘못됐으면 그냥 _float4x4 리턴
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4_TP(eStateType);
}

_vector CGameInstance::Get_CamPosition()
{
	if (nullptr == m_pPipeLine)
		return XMVectorZero();

	return m_pPipeLine->Get_CamPosition();
}

const LIGHTDESC * CGameInstance::Get_LightDesc(_uint iIndex) const
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const LIGHTDESC & LightDesc)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(pDevice, pDeviceContext, LightDesc);
}

bool CGameInstance::Key_Pressing(_int _key)
{
	if (nullptr == m_pKey_Manager)
		return 0;

	return m_pKey_Manager->Key_Pressing(_key);
}

bool CGameInstance::Key_Down(_int _key)
{
	if (nullptr == m_pKey_Manager)
		return 0;

	return m_pKey_Manager->Key_Down(_key);
}

bool CGameInstance::Key_Up(_int _key)
{
	if (nullptr == m_pKey_Manager)
		return 0;

	return m_pKey_Manager->Key_Up(_key);
}

HRESULT CGameInstance::Add_UI_Prototype(const _tchar * pPrototypeTag, CUI * pPrototype)
{
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	return m_pUI_Manager->Add_UI_Prototype(pPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Add_UI_Clone(_uint iLevelIndex, const _tchar * pPrototypeTag, const _tchar * pCloneTag, void * pArg)
{
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	return m_pUI_Manager->Add_UI_Clone(iLevelIndex, pPrototypeTag, pCloneTag, pArg);
}

HRESULT CGameInstance::Add_Disposable_UI(CUI * pUI)
{
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	return m_pUI_Manager->Add_Disposable_UI(pUI);
}

CUI * CGameInstance::Get_UI(_uint iLevelIndex, const _tchar * pCloneTag)
{
	if (nullptr == m_pUI_Manager)
		return nullptr;

	return m_pUI_Manager->Get_UI(iLevelIndex, pCloneTag);
}

HRESULT CGameInstance::UI_On(_uint iLevelIndex, const _tchar * pCloneTag)
{
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	return m_pUI_Manager->UI_On(iLevelIndex, pCloneTag);
}

HRESULT CGameInstance::UI_Off(_uint iLevelIndex, const _tchar * pCloneTag)
{
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	return m_pUI_Manager->UI_Off(iLevelIndex, pCloneTag);
}

HRESULT CGameInstance::All_UI_Off(_uint iLevelIndex)
{
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	return m_pUI_Manager->All_UI_Off(iLevelIndex);
}

HRESULT CGameInstance::Collision_AABB(_tchar * pSourTag, _tchar * pDestTag)
{
	if (nullptr == m_pCollision_Manager)
		return E_FAIL;

	return m_pCollision_Manager->Collision_AABB(pSourTag, pDestTag);
}

_bool CGameInstance::isIn_WorldSpace(_fvector vPoint, _float fRange)
{
	if (nullptr == m_pFrustum)
		return false;

	return m_pFrustum->isIn_WorldSpace(vPoint, fRange);
}

HRESULT CGameInstance::Add_Font(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pFontTag, const _tchar * pFontFilePath)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Add_Font(pDevice, pDeviceContext, pFontTag, pFontFilePath);
}

HRESULT CGameInstance::Render_Font(ID3D11DeviceContext* pDeviceContext, const _tchar* pFontTag, const _tchar* pText, _float2 vPosition, _fvector vColor)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Render_Font(pDeviceContext, pFontTag, pText, vPosition, vColor);
}

void CGameInstance::Release_Engine()
{
	if (0 != CGameInstance::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CGameInstance");

	// Release_Engine()이 static이라 클래스의 인스턴스에 종속되지 않으므로, 클래스의 멤버 변수에 접근할 수 없음..
	//_AResourceManager->

	// 하위 매니저 해제

	if (0 != CTimer_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CTimer_Manager");

	if (0 != CKey_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CKey_Manager");

	if (0 != CUI_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CUI_Manager");

	if (0 != CCollision_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CCollision_Manager");

	//	날리는 순서 상관은 없는데 현재 레벨에 쓰던 객체들을 다 날리고 레벨을 날리는것이 구조상? 맞는거 같은데 딱히 순서 상관은 없을듯?
	if (0 != CObject_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CObject_Manager");

	if (0 != CComponent_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CComponent_Manager");

	if (0 != CLevel_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CLevel_Manager");

	if (0 != CPipeLine::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CPipeLine");

	if (0 != CInput_Device::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CInput_Device");

	if (0 != CLight_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CLightManager");

	if (0 != CFrustum::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CFrustum");

	if (0 != CFont_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CFont_Manager");

	if (0 != CGraphic_Device::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Delete CGraphic_Device");
}

void CGameInstance::Free()
{
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pUI_Manager);
	Safe_Release(m_pKey_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pInput_Device);
}

#if defined (USE_IMGUI) /* ImGui 에서 사용할 함수들*/

const _uint & CGameInstance::Get_LevelIndex() const
{
	return m_pLevel_Manager->Get_LevelIndex();
}

map<const _tchar*, class CGameObject*>& CGameInstance::Get_Prototypes_Map()
{
	return m_pObject_Manager->Get_Prototypes_Map();
}

map<const _tchar*, class CLayer*>* CGameInstance::Get_Layers_Map()
{
	return m_pObject_Manager->Get_Layers_Map();
}

map<const _tchar*, class CComponent*>* CGameInstance::Get_Prototype_Components()
{
	return m_pComponent_Manager->Get_Prototype_Components();
}

#endif