#pragma once

#include "Base.h"

//	Ư�� �ִϸ��̼ǿ��� ���Ǵ� ��

BEGIN(Engine)

class CChannel final : public CBase
{
public:
	CChannel();
	virtual ~CChannel() = default;

public:
	const vector<KEYFRAME*>*	Get_KeyFrames() const { return &m_KeyFrames; }
	//	���� �� Ű�������� ������ �۾�
	_uint						Get_CurrentKeyFrame() const { return m_iCurrentKeyFrame; }
	const char*					Get_Name() const { return m_szName; }

	void						Set_CurrentKeyFrame(_uint iKeyFrameIndex) { m_iCurrentKeyFrame = iKeyFrameIndex; }
	void						Set_TransformationMatrix(_fmatrix TransformationMatrix);
	void						Set_HierarchyNodePtr(class CHierarchyNode* pNode);

public:
	void						Reserve(_uint iNumKeyFrames) { m_KeyFrames.reserve(iNumKeyFrames); m_iNumKeyFrames = iNumKeyFrames; }
	//	�̸��� �����ϴ� ���
	void						Add_KeyFrame(KEYFRAME* pKeyFrame) { m_KeyFrames.push_back(pKeyFrame); }

public:
	HRESULT						NativeConstruct(const char* pName);

private:
	//	�̸�
	char						m_szName[MAX_PATH] = "";

	//	m_KeyFrames�� �����Ǿ ���� Ű�������� �޶����
	_uint						m_iCurrentKeyFrame = 0;
	_uint						m_iNumKeyFrames = 0;

	vector<KEYFRAME*>			m_KeyFrames;
	typedef vector<KEYFRAME*>	KEYFRAME;

	class CHierarchyNode*		m_pNode = nullptr;

public:
	static CChannel*			Create(const char* pName);
	virtual void				Free() override;
};

END