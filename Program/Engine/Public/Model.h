#pragma once

#include "Component.h"

BEGIN(Engine)

#define	THEREISNONEXT 100

class ENGINE_DLL CModel final : public CComponent
{
public:
	//	메쉬를 읽어올때부터 세팅하고싶은 옵션이 있어서 그걸 위해 메쉬를 모델을 불러올때 어떤 타입인지 명시
	//	TYPE_ANIM : 애니메이션 있는 메쉬, TYPE_NONANIM : 애니메이션 없는 메쉬
	enum MODELTYPE { TYPE_ANIM, TYPE_NONANIM, TYPE_END };

public:
	//	OffsetMatrix, PivotMatrix(TransformMatrix)는 고정된거임
	//	CombinedTransformationMatrix 는 계속 바뀜(애니메이션), 계속받아와야하는데 계속 받아오지말고 주소로 받아오자
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
	//	전역변수 이름 pValueName
	HRESULT					Bind_OnShader(class CShader* pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char* pValueName);
	//	클라이언트에서 특정 애니메이션으로 돌리는 기능을 하는 함수
	HRESULT					SetUp_AnimIndex(_uint iAnimIndexm_bisInfinity, _bool isInfinity, _uint iNextIndex = 0);
	//	뼈들을 갱신해주는 작업(부모로부터 누적된 컴바인드 상태)
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

	//	월드 변환X, 로드할때 로컬상태에서 변환하기 위한거
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
	//	노드들을 생성한 다음에 컨테이너에 쭉 담아둘 생각
	vector<class CHierarchyNode*>			m_HierarchyNodes;
	typedef vector<class CHierarchyNode*>	HIERARCHYNODES;

private:
	_uint									m_iCurrentAnim = 0;
	//	신객체 안에 몇개의 애니메이션이 있는지 저장하기 위한 변수
	_uint									m_iNumAnimations;

	//	애니메이션 클래스들을 모아둘 vector 컨테이너
	vector<class CAnimation*>				m_Animations;
	typedef vector<class CAnimation*>		ANIMATIONS;

private:
	HRESULT					Ready_MeshContainers();
	//	기본적인 텍스쳐를 로드작업하는 함수
	HRESULT					Ready_Materials(const char* pModelFilePath);
	//	노드들을 생성하기 위한 함수
	HRESULT					Ready_HierarchyNodes(aiNode* pNode, CHierarchyNode* pParent, _uint iDepth);
	HRESULT					Ready_OffsetMatrices();
	//	애니메이션 정보를 다 읽어서 필요한것을 세팅
	HRESULT					Ready_Animation();
	HRESULT					Link_ChannelToNode();

private:
	//	인덱스 하나 리턴할려고 _uint* pOut = nullptr
	//	여기서 이름 같은 애를 찾는 작업을 함
	CHierarchyNode*			Find_HierarchyNode(const char* pName, _uint* pOut = nullptr);

public:
	//	메쉬파일의 경로(pModelFilePath), 파일이름(pModelFileName)
	//	fbx 파일이 있는 경로 안에 텍스쳐파일을 같이 넣어주고 그 파일을 직접 로드를 해줘해서 경로랑 파일이름을 따로 받아옴
	static CModel*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, MODELTYPE eModelType, const char* pModelFilePath, const char* pModelFileName, _fmatrix TransformMatrix);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};

END