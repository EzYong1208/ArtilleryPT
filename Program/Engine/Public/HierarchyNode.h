#pragma once

#include "Base.h"

//	�ϳ��� ��带 ��ǥ�ϴ� Ŭ����
//	������ ��� ������ ǥ�����ְ� �ִ� ����
//	��尡 ������ ���� ������ �ǹ������� ����, �޽��� ���� ������ ���� �ְ�, ��� �߿��� �ʱ���¸� �������ֱ� ���� ��ĵ� ����, ������ �� ���� �ƴ�!

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
	//	�ֻ��� �θ� ��ķκ��� �θ��� ����� ���ذ��鼭 �ڽ��� �Ĺ��ε� ����� ������Ʈ ����
	void						Update_CombinedTransformationMatrix();

private:
	//	�θ���
	//	���ʷε带 �Ű�ü�� RootNode�� ����, Ÿ�� ���� �ڽĸ���� �ڽ� �����
	//	�ڽĵ鿡�� ���� �θ� �������� ��� �����ϱ� ���ؼ�
	CHierarchyNode*				m_pParent = nullptr;
	//	��帶�� �̸�, Create �Ҷ����� �̸��� ��������
	char						m_szName[MAX_PATH] = "";
	//	���� ���ǵǾ� �ִ� ���ý����̽���, ������ ���ý����̽��� �ٸ� �� ����
	//	������ ���ý����̽����� ������ ���ý����̽��� �ٲ��ִ� ���
	_float4x4					m_OffsetMatrix;
	//	�θ�������� ǥ���� ������ �������
	//	�ڽĻ��� ���¸� �����ص� ���
	_float4x4					m_TransformationMatrix;
	//	�θ��� ��������� ���س��� �������
	_float4x4					m_CombinedTransformationMatrix;
	//	�θ�κ��� ��ܰ� ���� �༮�̳�(����Լ���)
	//	�ڽİ��踦 ��� Ÿ�� ���������� Ȯ���ҷ���
	_uint						m_iDepth;

public:
	//	��ġ��ü�� ���� �ʿ���� �� �ϴ�(�׸��� �۾� ���� ���� ������)
	static CHierarchyNode*		Create(const char* pName, _float4x4 TransformationMatrix, _uint iDepth, CHierarchyNode* pParent);
	virtual void				Free() override;
};

END