#include "..\Public\VIBuffer_Triangle.h"
#include "Shader.h"

CVIBuffer_Triangle::CVIBuffer_Triangle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_Triangle::CVIBuffer_Triangle(const CVIBuffer_Triangle & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Triangle::NativeConstruct_Prototype()
{
#pragma region VERTEX_BUFFER
	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_iNumIndicesPerPrimitive = 2;
	m_iNumVertices = 3;
	m_iNumVertexBuffers = 1;

	//	CVIBuffer 주석 참고
	m_VBDesc.ByteWidth = sizeof(VTXCOL) * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_DYNAMIC;					//	추가할때마다 정점정보를 세팅할 수 있게 동적버퍼로 세팅
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.StructureByteStride = sizeof(VTXCOL);
	//	동적버퍼로 만드니 CPUAccessFlags 잡아줘야함
	//	D3D11_CPU_ACCESS_WRITE로 값을 세팅할 수 있게 설정
	m_VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	//	값을 채워주기 위해서 버텍스 정점 3개를 할당함
	VTXCOL*		pVertices = new VTXCOL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXCOL) * m_iNumVertices);

	pVertices[0].vPosition = _float3(-0.5f, -0.5f, 0.f);
	//	색은 우선 흰색으로 그림
	pVertices[0].vColor = _float4(1.f, 1.f, 1.f, 1.f);

	pVertices[1].vPosition = _float3(0.0f, 0.5f, 0.f);
	pVertices[1].vColor = _float4(1.f, 1.f, 1.f, 1.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vColor = _float4(1.f, 1.f, 1.f, 1.f);		//	점 3개 찍음

	//	직접 값을 채워주고 대입
	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubResourceData.pSysMem = pVertices;

	if (FAILED(Create_VertexBuffer()))
		return E_FAIL;

	//	값 채우고나면 동적할당한거 필요가 없어지니 삭제
	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	m_iNumPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	//	LINESTRIP 형식으로 그릴것임
	//	0-1 그리고, 1-2 그리고, 2-0다시 꺼내서 그리고 이런식
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));
	m_IBDesc.ByteWidth = sizeof(FACEINDICES16) * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	_ushort*	pIndices = new _ushort[4];
	ZeroMemory(pIndices, sizeof(_ushort) * 4);

	//	그리고자 하는 정점 배열들의 인덱스
	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 0;

	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubResourceData.pSysMem = pIndices;

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Triangle::NativeConstruct(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Triangle::Render(CShader * pShader, _uint iPassIndex)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	ID3D11Buffer*	pVertexBuffers[] = {
		m_pVB
	};

	_uint			iStrides[] = {
		m_VBDesc.StructureByteStride
	};

	_uint			iOffsets[] = {
		0,
	};

	//	여러개의 버텍스버퍼를 세팅할 수 있다
	//	장치는 여러개의 버텍스 버퍼를 바인딩 할 수 있는데 몇번째 슬롯부터 채울껀지(당연히 0부터)
	m_pDeviceContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_eTopology);		//	트라이앵글 리스트(TRIANGLELIST) 채워주던거

	if (FAILED(pShader->Set_InputLayout(iPassIndex)))
		return E_FAIL;

	//	Apply()는 그리기 직전에 해라
	//	Apply 이후에 쉐이더 전역변수 값을 던지면 제대로 commit이 안됨
	if (FAILED(pShader->Apply(iPassIndex)))
		return E_FAIL;

	//	내가 사용하고자 하는 인덱스 갯수 2, m_iNumPrimitive는 3개
	//	라인 리스트 기준으로 그려놔서 아무튼 4로 바꿔야하는데 애매해서 걍 4로 넣음
	//	m_pDeviceContext->DrawIndexed(m_iNumIndicesPerPrimitive * m_iNumPrimitive, 0, 0);
	m_pDeviceContext->DrawIndexed(4, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer_Triangle::Update(const _float3 * pPoints)
{
	//	동적버퍼에만 갱신한다는 기능 추가

	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	//	맵(락)을 걸자, Map() UnMap(), Lock() UnLock() 함수 대체
	//	텍스쳐도 이걸로 똑같이 Lock, UnLock 가능함
	D3D11_MAPPED_SUBRESOURCE		SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	//	어떤 특정 버퍼를 Lock 한다(Map) 이 함수는 모든 메모리 영역에 대해서 Map 한다
	//	Map(누구를 락 할건지(m_pVB), 추가적으로 할당하는 녀석들이 있을수 있음(혹시 여러개를 공유할수 있음, 버텍스버퍼는 상관없어서 0),
	//	MapType를 어떤 방식으로 Map UnMap 할거냐(*밑에 주석참고), MapFlag(필요없음, 0),
	//	SubResource(구조체의 주소를 주면 구조체 안에 있는 pData 보이드 포인터, 버텍스버퍼의 공간의 주소를 줌))
	m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	for (_uint i = 0; i < 3; ++i)
	{
		//	VTXCOL 타입으로 캐스팅
		//	색깔값 다 날라가서 다시 흰색으로 세팅
		((VTXCOL*)SubResource.pData)[i].vPosition = pPoints[i];
		((VTXCOL*)SubResource.pData)[i].vColor = _float4(1.f, 1.f, 1.f, 1.f);
	}

	m_pDeviceContext->Unmap(m_pVB, 0);

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

	return S_OK;
}

HRESULT CVIBuffer_Triangle::Change_Color(_uint iTriangleIndex, _float4 vColor)
{
	return S_OK;
}

CVIBuffer_Triangle * CVIBuffer_Triangle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CVIBuffer_Triangle*	pInstance = new CVIBuffer_Triangle(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CVIBuffer_Triangle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Triangle::Clone(void * pArg)
{
	CVIBuffer_Triangle*	pInstance = new CVIBuffer_Triangle(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CVIBuffer_Triangle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Triangle::Free()
{
	__super::Free();
}
