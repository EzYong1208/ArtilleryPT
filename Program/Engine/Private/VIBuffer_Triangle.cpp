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

	//	CVIBuffer �ּ� ����
	m_VBDesc.ByteWidth = sizeof(VTXCOL) * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_DYNAMIC;					//	�߰��Ҷ����� ���������� ������ �� �ְ� �������۷� ����
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.StructureByteStride = sizeof(VTXCOL);
	//	�������۷� ����� CPUAccessFlags ��������
	//	D3D11_CPU_ACCESS_WRITE�� ���� ������ �� �ְ� ����
	m_VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	//	���� ä���ֱ� ���ؼ� ���ؽ� ���� 3���� �Ҵ���
	VTXCOL*		pVertices = new VTXCOL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXCOL) * m_iNumVertices);

	pVertices[0].vPosition = _float3(-0.5f, -0.5f, 0.f);
	//	���� �켱 ������� �׸�
	pVertices[0].vColor = _float4(1.f, 1.f, 1.f, 1.f);

	pVertices[1].vPosition = _float3(0.0f, 0.5f, 0.f);
	pVertices[1].vColor = _float4(1.f, 1.f, 1.f, 1.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vColor = _float4(1.f, 1.f, 1.f, 1.f);		//	�� 3�� ����

	//	���� ���� ä���ְ� ����
	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubResourceData.pSysMem = pVertices;

	if (FAILED(Create_VertexBuffer()))
		return E_FAIL;

	//	�� ä����� �����Ҵ��Ѱ� �ʿ䰡 �������� ����
	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	m_iNumPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	//	LINESTRIP �������� �׸�����
	//	0-1 �׸���, 1-2 �׸���, 2-0�ٽ� ������ �׸��� �̷���
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));
	m_IBDesc.ByteWidth = sizeof(FACEINDICES16) * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	_ushort*	pIndices = new _ushort[4];
	ZeroMemory(pIndices, sizeof(_ushort) * 4);

	//	�׸����� �ϴ� ���� �迭���� �ε���
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

	//	�������� ���ؽ����۸� ������ �� �ִ�
	//	��ġ�� �������� ���ؽ� ���۸� ���ε� �� �� �ִµ� ���° ���Ժ��� ä�ﲫ��(�翬�� 0����)
	m_pDeviceContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_eTopology);		//	Ʈ���̾ޱ� ����Ʈ(TRIANGLELIST) ä���ִ���

	if (FAILED(pShader->Set_InputLayout(iPassIndex)))
		return E_FAIL;

	//	Apply()�� �׸��� ������ �ض�
	//	Apply ���Ŀ� ���̴� �������� ���� ������ ����� commit�� �ȵ�
	if (FAILED(pShader->Apply(iPassIndex)))
		return E_FAIL;

	//	���� ����ϰ��� �ϴ� �ε��� ���� 2, m_iNumPrimitive�� 3��
	//	���� ����Ʈ �������� �׷����� �ƹ�ư 4�� �ٲ���ϴµ� �ָ��ؼ� �� 4�� ����
	//	m_pDeviceContext->DrawIndexed(m_iNumIndicesPerPrimitive * m_iNumPrimitive, 0, 0);
	m_pDeviceContext->DrawIndexed(4, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer_Triangle::Update(const _float3 * pPoints)
{
	//	�������ۿ��� �����Ѵٴ� ��� �߰�

	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	//	��(��)�� ����, Map() UnMap(), Lock() UnLock() �Լ� ��ü
	//	�ؽ��ĵ� �̰ɷ� �Ȱ��� Lock, UnLock ������
	D3D11_MAPPED_SUBRESOURCE		SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	//	� Ư�� ���۸� Lock �Ѵ�(Map) �� �Լ��� ��� �޸� ������ ���ؼ� Map �Ѵ�
	//	Map(������ �� �Ұ���(m_pVB), �߰������� �Ҵ��ϴ� �༮���� ������ ����(Ȥ�� �������� �����Ҽ� ����, ���ؽ����۴� ������ 0),
	//	MapType�� � ������� Map UnMap �Ұų�(*�ؿ� �ּ�����), MapFlag(�ʿ����, 0),
	//	SubResource(����ü�� �ּҸ� �ָ� ����ü �ȿ� �ִ� pData ���̵� ������, ���ؽ������� ������ �ּҸ� ��))
	m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	for (_uint i = 0; i < 3; ++i)
	{
		//	VTXCOL Ÿ������ ĳ����
		//	���� �� ���󰡼� �ٽ� ������� ����
		((VTXCOL*)SubResource.pData)[i].vPosition = pPoints[i];
		((VTXCOL*)SubResource.pData)[i].vColor = _float4(1.f, 1.f, 1.f, 1.f);
	}

	m_pDeviceContext->Unmap(m_pVB, 0);

	/*
	typedef 
	enum D3D11_MAP
    {	
        D3D11_MAP_READ	= 1,
        D3D11_MAP_WRITE	= 2,				//	���� ������ ����(�����ս� ����)
        D3D11_MAP_READ_WRITE = 3,
        D3D11_MAP_WRITE_DISCARD	= 4,		//	���� �ɶ� ������ ���� �� ���� �������� ������
        D3D11_MAP_WRITE_NO_OVERWRITE = 5	//	�������� �״�� ����(����ִ�, Ư�� ���� ��ü�ϰ� ������)
    } 	D3D11_MAP;
	*/
	//	�������ϸ� D3D11_MAP_WRITE_DISCARD�� ������ �������� �����ؾ߰ٴ� �ϸ� D3D11_MAP_WRITE_NO_OVERWRITE

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
