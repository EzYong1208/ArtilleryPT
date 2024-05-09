#include "RenderObject.h"
#include "GameInstance.h"
#include "AGameObject.h"

CRenderObject::CRenderObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);


	SetUp_Components();
}

void CRenderObject::SetTexture(string TagName)
{
	//Prototype_Component_Texture_Sky ../Bin/Resources/Textures/SkyBox/Sky004.dds
	//	For.Com_Texture
	//if (FAILED(__super::Add_Component(
	//	m_iCurrentLevel,
	//	TEXT("Prototype_Component_Texture_UI_Coin"),
	//	TEXT("Com_Texture"),
	//	(CComponent**)&m_pTextureCom)))
	//	return E_FAIL;
}

void CRenderObject::SetTransform(_float4x4 Transform)
{
	//m_pTransform = CTransform::Create(m_pDevice, m_pDeviceContext);

	//m_pTransform->Set_State(CTransform::STATE_POSITION, Transform);
	//m_pTransform->Set_WorldMatrix(Transform);
}

// 기존 CTT 등에서는 각 게임오브젝트들의 LateTick에서 	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this); 추가해서
// Renderer의 각 Render_Priortity 등의 함수에서 그 렌더러 그룹의 오브젝트들의 Render 함수를 호출하는 식이었음
// 이 부분을 어떻게 해야할지 고민해야함!
HRESULT CRenderObject::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	m_pTransform->Bind_OnShader(m_pShaderCom, "g_WorldMatrix");
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	m_pVIBufferCom->Render(m_pShaderCom, 0);

	return S_OK;
}

void CRenderObject::Add_RenderGroup(AGameObject* pRenderObject)
{
	m_pTransform->Scaled(XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.0f));
	m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX, m_fY, 0.f, 1.f));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, pRenderObject);
}

void CRenderObject::SetCoord_Size(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	m_fX = fX;
	m_fY = fY;
	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(1280.f, 640.f, 0.f, 1.f)));
}

HRESULT CRenderObject::NativeConstruct(string TextureKey, _float4x4 Transform)
{
	SetUp_Components();

	SetTexture(TextureKey);
	SetTransform(Transform);

	return S_OK;
}

HRESULT CRenderObject::SetUp_Components()
{
	//	For.Com_Renderer
	if (FAILED(Add_Component(
		0,
		TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	//	For.Com_Shader
	if (FAILED(Add_Component(
		0,
		TEXT("Prototype_Component_Shader_VTXTEX"),
		TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	//	For.Com_VIBuffer
	if (FAILED(Add_Component(
		0,
		TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	//	For.Com_Transform
	if (FAILED(Add_Component(
		0,
		TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"),
		(CComponent**)&m_pTransform)))
		return E_FAIL;

	// temp
	//	For.Com_Texture
	if (FAILED(Add_Component(
		0,
		TEXT("Prototype_Component_Texture_Default"),
		TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderObject::Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, CComponent** ppOut, void* pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CComponent* pComponent = pGameInstance->Clone_Component(iLevelIndex, pPrototypeTag, pArg);

	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(pComponentTag, pComponent);

	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CRenderObject* CRenderObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	//GameInstance clone 부탁

	//	setting
	CRenderObject* pInstance = new CRenderObject(pDevice, pDeviceContext);

	string TextureKey = "";
	_float4x4 Transform;
	ZeroMemory(&Transform, sizeof(_float4x4));

	pInstance->NativeConstruct(TextureKey, Transform);

	return pInstance;
}

void CRenderObject::Free()
{
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTextureCom);
}
