#pragma once

#include "Base.h"

//	특정 애니메이션에서 사용되는 뼈

BEGIN(Engine)

class CChannel final : public CBase
{
public:
	CChannel();
	virtual ~CChannel() = default;

public:
	const vector<KEYFRAME*>*	Get_KeyFrames() const { return &m_KeyFrames; }
	//	현재 내 키프레임을 얻어오는 작업
	_uint						Get_CurrentKeyFrame() const { return m_iCurrentKeyFrame; }
	const char*					Get_Name() const { return m_szName; }

	void						Set_CurrentKeyFrame(_uint iKeyFrameIndex) { m_iCurrentKeyFrame = iKeyFrameIndex; }
	void						Set_TransformationMatrix(_fmatrix TransformationMatrix);
	void						Set_HierarchyNodePtr(class CHierarchyNode* pNode);

public:
	void						Reserve(_uint iNumKeyFrames) { m_KeyFrames.reserve(iNumKeyFrames); m_iNumKeyFrames = iNumKeyFrames; }
	//	이름을 보관하는 기능
	void						Add_KeyFrame(KEYFRAME* pKeyFrame) { m_KeyFrames.push_back(pKeyFrame); }

public:
	HRESULT						NativeConstruct(const char* pName);

private:
	//	이름
	char						m_szName[MAX_PATH] = "";

	//	m_KeyFrames는 공유되어도 현재 키프레임은 달라야함
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