#pragma once

#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

class CMeshContainer final : public CVIBuffer
{
private:
	explicit CMeshContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMeshContainer(const CMeshContainer& rhs);
	virtual ~CMeshContainer() = default;

public:
	_uint						Get_NumBones() const { return m_iNumBones; }
	aiMesh*						Get_AIMesh() { return m_pAIMesh; }
	const char*					Get_Name() { return m_pAIMesh->mName.data; }
	_bool						Get_Check() { return m_bChoose; }

	void						Set_Check(_bool bChoose);

public:
	virtual HRESULT				NativeConstruct_Prototype(aiMesh* pAIMesh);
	virtual HRESULT				NativeConstruct(void* pArg);

public:
	HRESULT						Ready_VertexIndexBuffer(CModel::MODELTYPE eMeshtype, _fmatrix TransformMatrix);
	HRESULT						Add_HierarchyNode(_uint iNodeIndex);
	HRESULT						SetUp_Matrices(_float4x4* pBoneMatrices, const vector<CHierarchyNode*>* pNodes, _fmatrix TransformMatrix);

private:
	HRESULT						Ready_NonAnimMeshContainer(aiMesh* pAIMesh, _fmatrix TransformMatrix);
	HRESULT						Ready_AnimMeshContainer(aiMesh* pAIMesh);
	//	스키닝, 스킨드, 뼈에 살을 붙인다
	//	이 피부들이 어떤 뼈의 정보를 가져와야 하는지. 그 정점을 그 뼈에 붙인다. 
	//	피부를 입힌다는 개념.이걸 통틀어서 스킨드라고 부른다
	HRESULT						Ready_SkinnedInfo(VTXANIMMODEL* pVertices);

private:
	char							m_szName[MAX_PATH] = "";
	_uint							m_iMaterialIndex = 0;
	//	몇개의 뼈대가 이 메쉬컨테이너?메쉬?에 영향을 주는지
	_uint							m_iNumBones = 0;
	aiMesh*							m_pAIMesh = nullptr;

	_bool							m_bChoose = false;

	//	현재 메쉬에 영향을 주는 노드들의 인덱스
	vector<_uint>					m_HierarchyNodeIndices;

public:
	static CMeshContainer*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, aiMesh* pAIMesh);
	virtual CComponent*			Clone(void* pArg) override;
	virtual void				Free() override;
};

END