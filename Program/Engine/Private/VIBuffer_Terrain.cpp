#include "..\Public\VIBuffer_Terrain.h"
#include "Transform.h"
#include "Frustum.h"
#include "QuadTree.h"
#include "PipeLine.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{

}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain & rhs)
	: CVIBuffer(rhs)
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
	, m_pIndices(rhs.m_pIndices)
	, m_pQuadTree(rhs.m_pQuadTree)
{
	Safe_AddRef(m_pQuadTree);
}

HRESULT CVIBuffer_Terrain::NativeConstruct_Prototype(const _tchar* pHeightMap)
{

#pragma region VERTEX_BUFFER

	//	���� ������� ���ؼ� ������ ��ġ�� ����
	HANDLE		hFile = CreateFile(pHeightMap, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_ulong					dwByte = 0;
	BITMAPFILEHEADER		fh;
	BITMAPINFOHEADER		ih;

	ReadFile(hFile, &fh, sizeof(BITMAPFILEHEADER), &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof(BITMAPINFOHEADER), &dwByte, nullptr);

	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;

	_ulong*		pPixel = new _ulong[m_iNumVerticesX * m_iNumVerticesZ];
	ReadFile(hFile, pPixel, sizeof(_ulong) * m_iNumVerticesX * m_iNumVerticesZ, &dwByte, nullptr);

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_iNumIndicesPerPrimitive = 3;
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	m_iNumVertexBuffers = 1;

	m_VBDesc.ByteWidth = sizeof(VTXNORTEX) * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.StructureByteStride = sizeof(VTXNORTEX);

	VTXNORTEX*		pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	m_pVerticesPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = _float3((float)j, (float)(pPixel[iIndex] & 0x000000ff) / 10.f, (float)i);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexUV = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
		}
	}

	//	���� ���� ä���ְ� ����
	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubResourceData.pSysMem = pVertices;

	//	�� ä����� �����Ҵ��Ѱ� �ʿ䰡 �������� ����
	Safe_Delete_Array(pPixel);
	CloseHandle(hFile);

#pragma endregion


#pragma region INDEX_BUFFER
	m_iNumPrimitive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));
	m_IBDesc.ByteWidth = sizeof(FACEINDICES32) * m_iNumPrimitive;
	//m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.Usage = D3D11_USAGE_DYNAMIC;			//	�ε��� ���۸� ��������
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	FACEINDICES32*	pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	//	�̸� �Ҵ��� �α�
	m_pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(m_pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	_uint		iNumFace = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			pIndices[iNumFace]._0 = iIndices[0];
			pIndices[iNumFace]._1 = iIndices[1];
			pIndices[iNumFace]._2 = iIndices[2];

			//	iNumFace ������Ű�� ���� 3�� ������ ������ �����ͼ�
			//	�������� ���� �ִ� ���� ���� �ٸ���(� ���� ���� �������� ������ �������� �ִ�)
			//	����� ���ؼ� ��ֺ��͸� ���ϴ°� ������ ���⺤���̱� ������ ���̰� �ʿ����� �ʴ�

			_vector		vSour, vDest, vNormal;
			//	2��° �������� 1���� ������ ���� vSour ����
			vSour = XMLoadFloat3(&m_pVerticesPos[pIndices[iNumFace]._1]) - XMLoadFloat3(&m_pVerticesPos[pIndices[iNumFace]._0]);
			//	������ �������� 2���� ������ ���� vDest ���ϰ�
			vDest = XMLoadFloat3(&m_pVerticesPos[pIndices[iNumFace]._2]) - XMLoadFloat3(&m_pVerticesPos[pIndices[iNumFace]._1]);
			//	vSour�� vDest�� �������� vNormal�� ����
			//	XMVector3Cross�� ���ؼ� �����Ѵ����� XMVector3Normalize�� ��ֶ������ؼ� vNormal
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			//	�� ��ֶ������ؼ� �������ִ� �۾��� �ؾ���
			XMStoreFloat3(&pVertices[pIndices[iNumFace]._0].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._0].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFace]._1].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._1].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFace]._2].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._2].vNormal) + vNormal));
			++iNumFace;

			pIndices[iNumFace]._0 = iIndices[0];
			pIndices[iNumFace]._1 = iIndices[2];
			pIndices[iNumFace]._2 = iIndices[3];

			vSour = XMLoadFloat3(&m_pVerticesPos[pIndices[iNumFace]._1]) - XMLoadFloat3(&m_pVerticesPos[pIndices[iNumFace]._0]);
			vDest = XMLoadFloat3(&m_pVerticesPos[pIndices[iNumFace]._2]) - XMLoadFloat3(&m_pVerticesPos[pIndices[iNumFace]._1]);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumFace]._0].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._0].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFace]._1].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._1].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFace]._2].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._2].vNormal) + vNormal));
			++iNumFace;
		}
	}

	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubResourceData.pSysMem = pIndices;

	if (FAILED(Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	m_pQuadTree = CQuadTree::Create(m_iNumVerticesX, m_iNumVerticesZ);
	if (nullptr == m_pQuadTree)
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Terrain::NativeConstruct(void * pArg)
{
	return S_OK;
}

void CVIBuffer_Terrain::Culling(CTransform * pTransform)
{
	CFrustum*		pFrustum = GET_INSTANCE(CFrustum);
	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	//	���û��� ����� ���ϴ� �۾�
	if (FAILED(pFrustum->Transform_ToLocalSpace(pTransform)))
		return;

	_uint		iNumFaces = 0;

	//	XMVector3TransformCoord(pPipeLine->Get_CamPosition()) : ī�޶� ��ġ�� ��������� ������� ���ؼ� ������ġ�� �ٲ�
	//	����Ʈ���� ���ؼ� �����ø�
	m_pQuadTree->Culling(pFrustum, m_pVerticesPos, m_pIndices, &iNumFaces, XMVector3TransformCoord(pPipeLine->Get_CamPosition(), pTransform->Get_WorldMatrixInverse()));
	
	//for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	//{
	//	for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
	//	{
	//		_uint		iIndex = i * m_iNumVerticesX + j;

	//		_uint		iIndices[4] = {
	//			iIndex + m_iNumVerticesX,
	//			iIndex + m_iNumVerticesX + 1,
	//			iIndex + 1,
	//			iIndex
	//		};

	//		_bool		isIn[4] = {
	//			//	m_pVerticesPos(������ ��ġ)�� iIndices�� 0��°�� ����ü �ȿ� �ִ��� Ȯ���ؾ���
	//			//	�Ǵ��ϰ��� �ϴ� ���� ��ġ(XMLoadFloat3(&m_pVerticesPos[iIndices[0]])), fRange(����Ʈ 0)
	//			pFrustum->isIn_LocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[0]])),
	//			pFrustum->isIn_LocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[1]])),
	//			pFrustum->isIn_LocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[2]])),
	//			pFrustum->isIn_LocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[3]]))
	//		};

	//		//	������ �� �ﰢ��(3�� ���� �ϳ��� ���� ������)
	//		//	�켱�� üũ�ҷ��� &&�� �ص�, ������ ||
	//		if (true == isIn[0] &&
	//			true == isIn[1] &&
	//			true == isIn[2])
	//		{
	//			//	�ε����� �ٽ� �׾��ִ� �۾��� ��
	//			//	�������� ������ �߰��ϴ°� �ƴ϶� iNumFaces �ϳ� �������� �߰��ؼ� m_pIndices[iNumFaces] �� ä��� ++iNumFaces
	//			m_pIndices[iNumFaces]._0 = iIndices[0];
	//			m_pIndices[iNumFaces]._1 = iIndices[1];
	//			m_pIndices[iNumFaces]._2 = iIndices[2];
	//			++iNumFaces;
	//		}

	//		//	���� �ϴ� �ﰢ��
	//		if (true == isIn[0] &&
	//			true == isIn[2] &&
	//			true == isIn[3])
	//		{
	//			m_pIndices[iNumFaces]._0 = iIndices[0];
	//			m_pIndices[iNumFaces]._1 = iIndices[2];
	//			m_pIndices[iNumFaces]._2 = iIndices[3];
	//			++iNumFaces;
	//		}
	//	}
	//}

	RELEASE_INSTANCE(CPipeLine);
	RELEASE_INSTANCE(CFrustum);

	//	���� �ε��� ���� ��ü
	D3D11_MAPPED_SUBRESOURCE		SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	//	� Ư�� ���۸� Lock �Ѵ�(Map) �� �Լ��� ��� �޸� ������ ���ؼ� Map �Ѵ�
	//	Map(������ �� �Ұ���(m_pVB), �߰������� �Ҵ��ϴ� �༮���� ������ ����(Ȥ�� �������� �����Ҽ� ����, ���ؽ����۴� ������ 0),
	//	MapType�� � ������� Map UnMap �Ұų�(*�ؿ� �ּ�����), MapFlag(�ʿ����, 0),
	//	SubResource(����ü�� �ּҸ� �ָ� ����ü �ȿ� �ִ� pData ���̵� ������, ���ؽ������� ������ �ּҸ� ��))
	m_pDeviceContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	//	SubResource.pData �� ������ ���ο� ������ �޾ƾ���, �׷��� memcpy�� �������
	memcpy(SubResource.pData, m_pIndices, sizeof(FACEINDICES32) * iNumFaces);

	m_pDeviceContext->Unmap(m_pIB, 0);

	m_iNumPrimitive = iNumFaces;

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
}

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar* pHeightMap)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pHeightMap)))
	{
		MSGBOX("Failed to Created CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Terrain::Clone(void * pArg)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();

	Safe_Release(m_pQuadTree);

	if (false == m_isCloned)
		Safe_Delete_Array(m_pIndices);
}
