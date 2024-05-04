#pragma once

#include "Base.h"

//	하나의 노드를 대표하는 클래스
//	뼈들의 상속 구조를 표현해주고 있는 노드들
//	노드가 무조건 뼈의 정보를 의미하지는 않음, 메쉬의 실제 정보일 수도 있고, 노드 중에도 초기상태를 정의해주기 위한 행렬도 있음, 무조건 다 뼈가 아님!

BEGIN(Engine)

class CHierarchyNode final : public CBase
{
public:
	CHierarchyNode();
	virtual ~CHierarchyNode() = default;

public:
	_uint						Get_Depth() const { return m_iDepth; }
	const char*					Get_Name() { return m_szName; }

	_fmatrix					Get_OffsetMatrix() { return XMLoadFloat4x4(&m_OffsetMatrix); }
	_float4x4*					Get_OffsetMatrixPtr() { return &m_OffsetMatrix; }

	_fmatrix					Get_CombinedTransformationMatrix() { return XMLoadFloat4x4(&m_CombinedTransformationMatrix); }
	_float4x4*					Get_CombinedTransformationMatrixPtr() { return &m_CombinedTransformationMatrix; }

public:
	void						Set_OffsetMatrix(_float4x4 OffsetMatrix) { XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix))); }
	void						Set_TransformationMatrix(_fmatrix TransformationMatrix) { XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix); }

public:
	HRESULT						NativeConstruct(const char* pName, _float4x4 TransformationMatrix, _uint iDepth, CHierarchyNode* pParent);
	//	최상위 부모 행렬로부터 부모의 행렬을 곱해가면서 자식의 컴바인드 행렬을 업데이트 해줌
	void						Update_CombinedTransformationMatrix();

private:
	//	부모노드
	//	최초로드를 신객체의 RootNode로 접근, 타고 들어가서 자식만들고 자식 만들고
	//	자식들에게 너의 부모가 누구였어 라고 정의하기 위해서
	CHierarchyNode*				m_pParent = nullptr;
	//	노드마다 이름, Create 할때마다 이름을 던져주자
	char						m_szName[MAX_PATH] = "";
	//	뼈가 정의되어 있는 로컬스페이스와, 정점의 로컬스페이스가 다를 수 있음
	//	뼈들의 로컬스페이스에서 정점의 로컬스페이스로 바꿔주는 행렬
	_float4x4					m_OffsetMatrix;
	//	부모기준으로 표현된 나만의 상태행렬
	//	자식뼈의 상태를 정리해둔 행렬
	_float4x4					m_TransformationMatrix;
	//	부모의 상태행렬을 곱해놓은 최종행렬
	_float4x4					m_CombinedTransformationMatrix;
	//	부모로부터 몇단계 들어온 녀석이냐(재귀함수라서)
	//	자식관계를 몇번 타고 내려갔는지 확인할려고
	_uint						m_iDepth;

public:
	//	장치객체는 딱히 필요없을 듯 하다(그리는 작업 등은 하지 않을거)
	static CHierarchyNode*		Create(const char* pName, _float4x4 TransformationMatrix, _uint iDepth, CHierarchyNode* pParent);
	virtual void				Free() override;
};

END