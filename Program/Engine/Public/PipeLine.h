#pragma once

//	장치에 바인드한다는게 없기 때문에 바인드 하는 기능
//	장치가 행렬을 보관한다는 기능을 똑같이 대체함

#include "Base.h"

BEGIN(Engine)

class CPipeLine final : public CBase
{
	DECLARE_SINGLETON(CPipeLine)
public:
	enum TRANSFORMSTATETYPE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };

private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	HRESULT			Set_Transform(TRANSFORMSTATETYPE eStateType, _fmatrix TransformMatrix);

	_matrix			Get_TransformMatrix(TRANSFORMSTATETYPE eStateType) { return XMLoadFloat4x4(&m_TransformMatrix[eStateType]); }

	//	vector 로 바꿔서 전치해줘야함, 쉐이더에 던져주기 편할려고 전치해주는거임
	_float4x4		Get_TransformFloat4x4(TRANSFORMSTATETYPE eStateType) { return m_TransformMatrix[eStateType]; }

	_float4x4		Get_TransformFloat4x4_TP(TRANSFORMSTATETYPE eStateType);

	_vector			Get_CamPosition() const { return XMLoadFloat4(&m_vCamPosition); }

public:
	//	역행렬을 구하는 작업, 계속해서 카메라의 위치를 받아옴
	void			Update();

private:
	_float4x4		m_TransformMatrix[2];
	_float4			m_vCamPosition;

public:
	virtual void	Free() override;
};

END