#pragma once

#include "Component.h"

BEGIN(Engine)

#define	THEREISNONEXT 100

class ENGINE_DLL CModel final : public CComponent
{
public:
	//	�޽��� �о�ö����� �����ϰ���� �ɼ��� �־ �װ� ���� �޽��� ���� �ҷ��ö� � Ÿ������ ���
	//	TYPE_ANIM : �ִϸ��̼� �ִ� �޽�, TYPE_NONANIM : �ִϸ��̼� ���� �޽�
	enum MODELTYPE { TYPE_ANIM, TYPE_NONANIM, TYPE_END };

public:
	//	OffsetMatrix, PivotMatrix(TransformMatrix)�� �����Ȱ���
	//	CombinedTransformationMatrix �� ��� �ٲ�(�ִϸ��̼�), ��ӹ޾ƿ;��ϴµ� ��� �޾ƿ������� �ּҷ� �޾ƿ���
	typedef struct tagBoneMatrixPtr
	{
		_float4x4*			pOffsetMatrix;
		_float4x4*			pCombinedMatrix;
	}BONEMATRIX_PTR;

private:
	explicit CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint					Get_NumMaterials() const { return m_iNumMaterials; }
	BONEMATRIX_PTR			Get_BoneMatrixPtr(const char* pBoneName);
	_float4x4				Get_TransformMatrix() { return m_TransformMatrix; }

	_uint					Get_CurrentAnimIndex() { return m_iCurrentAnim; }

	_bool					Get_LimitAnim();
	void					Set_LimitAnimFalse();

	_uint					Get_CurrentKeyFrame();

public:
	virtual HRESULT			NativeConstruct_Prototype(MODELTYPE eModelType, const char* pModelFilePath, const char* pModelFileName, _fmatrix TransformMatrix);
	virtual HRESULT			NativeConstruct(void* pArg);

public:
	HRESULT					Render(class CShader* pShader, _uint iPassIndex, _uint iMaterialIndex, const char* pBoneValueName = nullptr);
	//	�������� �̸� pValueName
	HRESULT					Bind_OnShader(class CShader* pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char* pValueName);
	//	Ŭ���̾�Ʈ���� Ư�� �ִϸ��̼����� ������ ����� �ϴ� �Լ�
	HRESULT					SetUp_AnimIndex(_uint iAnimIndexm_bisInfinity, _bool isInfinity, _uint iNextIndex = 0);
	//	������ �������ִ� �۾�(�θ�κ��� ������ �Ĺ��ε� ����)
	HRESULT					Update_CombinedTransformationMatrices(_double TimeDelta);

	HRESULT					Object_Active(const char* pMeshName, _bool bChoose);
	void					Set_AllOn();
	void					Set_AllOff();

private:
	const aiScene*							m_pScene = nullptr;
	Importer								m_Importer;

private:
	_uint									m_iNumMeshContainers = 0;
	vector<class CMeshContainer*>*			m_pMeshContainers;
	typedef vector<class CMeshContainer*>	MESHCONTAINERS;
	MODELTYPE								m_eModelType = TYPE_END;

	//	���� ��ȯX, �ε��Ҷ� ���û��¿��� ��ȯ�ϱ� ���Ѱ�
	_float4x4								m_TransformMatrix;

	_bool									m_bAllOn = false;
	_bool									m_bisInfinity = false;
	_uint									m_iNextAnimIndex = 0;
	_bool									m_bLimitAnim = false;

private:
	_uint									m_iNumMaterials = 0;
	vector<MESHMATERIALDESC*>				m_Materials;
	typedef vector<MESHMATERIALDESC*>		MATERIALS;

private:
	//	������ ������ ������ �����̳ʿ� �� ��Ƶ� ����
	vector<class CHierarchyNode*>			m_HierarchyNodes;
	typedef vector<class CHierarchyNode*>	HIERARCHYNODES;

private:
	_uint									m_iCurrentAnim = 0;
	//	�Ű�ü �ȿ� ��� �ִϸ��̼��� �ִ��� �����ϱ� ���� ����
	_uint									m_iNumAnimations;

	//	�ִϸ��̼� Ŭ�������� ��Ƶ� vector �����̳�
	vector<class CAnimation*>				m_Animations;
	typedef vector<class CAnimation*>		ANIMATIONS;

private:
	HRESULT					Ready_MeshContainers();
	//	�⺻���� �ؽ��ĸ� �ε��۾��ϴ� �Լ�
	HRESULT					Ready_Materials(const char* pModelFilePath);
	//	������ �����ϱ� ���� �Լ�
	HRESULT					Ready_HierarchyNodes(aiNode* pNode, CHierarchyNode* pParent, _uint iDepth);
	HRESULT					Ready_OffsetMatrices();
	//	�ִϸ��̼� ������ �� �о �ʿ��Ѱ��� ����
	HRESULT					Ready_Animation();
	HRESULT					Link_ChannelToNode();

private:
	//	�ε��� �ϳ� �����ҷ��� _uint* pOut = nullptr
	//	���⼭ �̸� ���� �ָ� ã�� �۾��� ��
	CHierarchyNode*			Find_HierarchyNode(const char* pName, _uint* pOut = nullptr);

public:
	//	�޽������� ���(pModelFilePath), �����̸�(pModelFileName)
	//	fbx ������ �ִ� ��� �ȿ� �ؽ��������� ���� �־��ְ� �� ������ ���� �ε带 �����ؼ� ��ζ� �����̸��� ���� �޾ƿ�
	static CModel*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, MODELTYPE eModelType, const char* pModelFilePath, const char* pModelFileName, _fmatrix TransformMatrix);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};

END