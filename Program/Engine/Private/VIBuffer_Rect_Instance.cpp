#include "..\Public\VIBuffer_Rect_Instance.h"
#include "Shader.h"

CVIBuffer_Rect_Instance::CVIBuffer_Rect_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_Rect_Instance::CVIBuffer_Rect_Instance(const CVIBuffer_Rect_Instance & rhs)
	: CVIBuffer(rhs)
	, m_pVBInstance(rhs.m_pVBInstance)
	, m_VBInstDesc(rhs.m_VBInstDesc)
	, m_VBInstSubResource(rhs.m_VBInstSubResource)
	, m_iNumInstance(rhs.m_iNumInstance)
{
	Safe_AddRef(m_pVBInstance);
}

HRESULT CVIBuffer_Rect_Instance::NativeConstruct_Prototype(_uint iNumInstance)
{
#pragma region VERTEX_BUFFER
	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_iNumIndicesPerPrimitive = 3;
	m_iNumVertices = 4;
	m_iNumVertexBuffers = 2;

	//	CVIBuffer 주석 참고
	m_VBDesc.ByteWidth = sizeof(VTXTEX) * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.StructureByteStride = sizeof(VTXTEX);

	//	값을 채워주기 위해서 버텍스 정점 4개를 할당함
	VTXTEX*		pVertices = new VTXTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXTEX) * m_iNumVertices);

	//	가로 세로 사이즈 다 1
	//	정점은 이 이미지의 어떤 부분을 참조해야하느냐
	//	이미지 상의 좌표(api 좌표랑 같음)
	//	텍스쳐 좌표를 상대적 좌표로 쓰자! 이미지의 가로 사이즈를 무조건 1이라는 비율값으로 사용하자!
	//	아틀라스 이미지를 거의 사용하지 않는다
	//	UV좌표(상대적 좌표)
	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);		//	로컬 스페이스에다 찍고 있음
	pVertices[0].vTexUV = _float2(0.f, 0.f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexUV = _float2(1.f, 0.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexUV = _float2(1.f, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexUV = _float2(0.f, 1.f);				//	점 4개 찍음

	//	직접 값을 채워주고 대입
	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubResourceData.pSysMem = pVertices;

	if (FAILED(Create_VertexBuffer()))
		return E_FAIL;

	//	값 채우고나면 동적할당한거 필요가 없어지니 삭제
	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	m_iNumInstance = iNumInstance;
	//	갯수(인스턴스) * 2개, 멤버변수에다 받아온 갯수를 저장
	m_iNumPrimitive = m_iNumInstance * 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));
	m_IBDesc.ByteWidth = sizeof(FACEINDICES16) * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	FACEINDICES16*	pIndices = new FACEINDICES16[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iNumPrimitive);

	_uint		iNumFaces = 0;

	//	for 문을 돌면서 인스턴스 하나당 삼각형 2개씩 정의되면 될거같다
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		pIndices[iNumFaces]._0 = 0;
		pIndices[iNumFaces]._1 = 1;
		pIndices[iNumFaces]._2 = 2;
		++iNumFaces;

		pIndices[iNumFaces]._0 = 0;
		pIndices[iNumFaces]._1 = 2;
		pIndices[iNumFaces]._2 = 3;
		++iNumFaces;
	}

	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubResourceData.pSysMem = pIndices;

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

#pragma region INSTANCE_BUFFER
	ZeroMemory(&m_VBInstDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBInstDesc.ByteWidth = sizeof(VTXMATRIX) * m_iNumInstance;
	m_VBInstDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBInstDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBInstDesc.StructureByteStride = sizeof(VTXMATRIX);
	m_VBInstDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	VTXMATRIX*		pInstanceMatrix = new VTXMATRIX[m_iNumInstance];
	ZeroMemory(pInstanceMatrix, sizeof(VTXMATRIX) * m_iNumInstance);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{

		pInstanceMatrix[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pInstanceMatrix[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pInstanceMatrix[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		//	우선은 랜덤하게 포지션 잡아줌
		pInstanceMatrix[i].vPosition = _float4(rand() % 15, 0.f, rand() % 15, 1.f);
	}

	ZeroMemory(&m_VBInstSubResource, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBInstSubResource.pSysMem = pInstanceMatrix;

	if (FAILED(m_pDevice->CreateBuffer(&m_VBInstDesc, &m_VBInstSubResource, &m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pInstanceMatrix);

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Rect_Instance::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_ParticleDesc, pArg, sizeof(m_ParticleDesc));

	m_pSpeeds = new _float[m_iNumInstance];

	for (_uint i = 0; i < m_iNumInstance; ++i)
		m_pSpeeds[i] = rand() % _uint(m_ParticleDesc.fMaxSpeed - m_ParticleDesc.fMinSpeed + 1) + m_ParticleDesc.fMinSpeed;


	return S_OK;
}

HRESULT CVIBuffer_Rect_Instance::Render(CShader * pShader, _uint iPassIndex)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	ID3D11Buffer*	pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance
	};

	_uint			iStrides[] = {
		m_VBDesc.StructureByteStride,
		m_VBInstDesc.StructureByteStride
	};

	_uint			iOffsets[] = {
		0,
		0
	};

	m_pDeviceContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_eTopology);

	if (FAILED(pShader->Set_InputLayout(iPassIndex)))
		return E_FAIL;

	if (FAILED(pShader->Apply(iPassIndex)))
		return E_FAIL;

	//	DrawIndexedInstanced(인스턴스 하나당 이용하는 인스턴스 갯수(네모 하나 그리기 위해 몇개의 인덱스를 사용하고 있냐)
	//	나머지는 싹다 앞에서부터 써라 0, 0, 0
	m_pDeviceContext->DrawIndexedInstanced(6, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_Rect_Instance::Update(_double TimeDelta)
{
	if (nullptr == m_pDeviceContext)
		return;

	//	맵(락)을 걸자, Map() UnMap(), Lock() UnLock() 함수 대체
	//	텍스쳐도 이걸로 똑같이 Lock, UnLock 가능함
	D3D11_MAPPED_SUBRESOURCE		SubResource;

	//	어떤 특정 버퍼를 Lock 한다(Map) 이 함수는 모든 메모리 영역에 대해서 Map 한다
	//	Map(누구를 락 할건지(m_pVB), 추가적으로 할당하는 녀석들이 있을수 있음(혹시 여러개를 공유할수 있음, 버텍스버퍼는 상관없어서 0),
	//	MapType를 어떤 방식으로 Map UnMap 할거냐(*밑에 주석참고), MapFlag(필요없음, 0),
	//	SubResource(구조체의 주소를 주면 구조체 안에 있는 pData 보이드 포인터, 버텍스버퍼의 공간의 주소를 줌))
	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		_vector		vPosition = XMLoadFloat4(&(((VTXMATRIX*)SubResource.pData)[i].vPosition));

		vPosition += XMVector3Normalize(XMLoadFloat3(&m_ParticleDesc.vMoveDir)) * m_pSpeeds[i] * TimeDelta;

		if (XMVectorGetY(vPosition) < -10.f)
			vPosition = XMVectorSetY(vPosition, 0.f);

		XMStoreFloat4(&(((VTXMATRIX*)SubResource.pData)[i].vPosition), vPosition);
	}

	m_pDeviceContext->Unmap(m_pVBInstance, 0);

	/*
	typedef
	enum D3D11_MAP
	{
		D3D11_MAP_READ	= 1,
		D3D11_MAP_WRITE	= 2,				//	거의 사용되지 않음(퍼포먼스 구림)
		D3D11_MAP_READ_WRITE = 3,
		D3D11_MAP_WRITE_DISCARD	= 4,		//	맵을 걸때 기존에 값은 다 날라간 정보들을 삭제함
		D3D11_MAP_WRITE_NO_OVERWRITE = 5	//	기존값을 그대로 얻어옴(살아있다, 특정 값만 교체하고 싶을때)
	} 	D3D11_MAP;
	*/
	//	어지간하면 D3D11_MAP_WRITE_DISCARD가 좋은데 기존값을 유지해야겟다 하면 D3D11_MAP_WRITE_NO_OVERWRITE
}

CVIBuffer_Rect_Instance * CVIBuffer_Rect_Instance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iNumInstance)
{
	CVIBuffer_Rect_Instance*	pInstance = new CVIBuffer_Rect_Instance(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(iNumInstance)))
	{
		MSGBOX("Failed to Created CVIBuffer_Rect_Instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Rect_Instance::Clone(void * pArg)
{
	CVIBuffer_Rect_Instance*	pInstance = new CVIBuffer_Rect_Instance(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CVIBuffer_Rect_Instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Rect_Instance::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pSpeeds);
	Safe_Release(m_pVBInstance);
}
