#include "../pch.h"
#include "FollowCamera.h"

//揺れる範囲(最大)
const int FollowCamera::MAX_SHAKE_RANGE = 1;

//範囲の収縮する値
const float FollowCamera::CONTRACTION_RANGE = 0.025f;
//揺れる幅
const float FollowCamera::SHAPED_WIDTH = 0.45f;

FollowCamera::FollowCamera()
{
	m_eyePos = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 10.0f);
	m_targetPos = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
	m_up = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);

	m_viewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(m_eyePos, m_targetPos, m_up);

	m_refEyePos = DirectX::SimpleMath::Vector3(5.0f, 5.0f, 5.0f);
	m_refTargetPos= DirectX::SimpleMath::Vector3(5.0f, 5.0f, 5.0f);
}


FollowCamera::~FollowCamera()
{
}

void FollowCamera::Initialize()
{
	//揺れる方向
	m_shapedWidth = DirectX::SimpleMath::Vector3::Zero;
	//揺れる時のカメラの位置
	m_shakeCameraPos = DirectX::SimpleMath::Vector3::Zero;;
	//揺れ判定
	m_shake = false;
	m_shakeRange = 0.0f;
	m_shakeReverseCheck = false;
	m_rangeContraction = 0.0f;
}

void FollowCamera::Update()
{
	DirectX::SimpleMath::Vector3 up(0.0f, 1.0f, 0.0f);

	//m_EysPos += (m_refEyePos - m_EysPos) * 0.05f;
	//m_TargetPos += (m_refTargetPos - m_TargetPos) * 0.05f;

	m_shakeCameraPos = DirectX::SimpleMath::Vector3::Zero;

	if(m_shake)
	{
		Shake();
	}

	m_viewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(m_eyePos + m_shakeCameraPos, m_targetPos + m_shakeCameraPos, up);
}

//開始揺れ(引数は揺れる方向)
void FollowCamera::StartShake(const DirectX::SimpleMath::Vector3 &vec)
{
	m_shapedWidth = vec;
	m_shake = true;
}

//揺れ処理
void FollowCamera::Shake()
{
	//揺れる範囲を
	if (m_shakeReverseCheck)
	{
		m_shakeRange += SHAPED_WIDTH;
	}
	else
	{
		m_shakeRange -= SHAPED_WIDTH;
	}

	//
	if (m_shakeRange >= 0.0f)
	{
		m_shakeCameraPos = DirectX::SimpleMath::Vector3::Lerp(DirectX::SimpleMath::Vector3::Zero, m_shapedWidth, m_shakeRange);
	}
	else
	{
		m_shakeCameraPos = DirectX::SimpleMath::Vector3::Lerp(-m_shapedWidth, DirectX::SimpleMath::Vector3::Zero, MAX_SHAKE_RANGE + m_shakeRange);
	}

	//反転処理と揺れの範囲を狭める
	if (MAX_SHAKE_RANGE - m_rangeContraction < m_shakeRange)
	{

		m_shakeReverseCheck = !m_shakeReverseCheck;
		//範囲の収縮
		m_rangeContraction += CONTRACTION_RANGE;

		//位置の修正
		m_shakeRange = MAX_SHAKE_RANGE - m_rangeContraction;
	}
	//反転処理
	else if(-MAX_SHAKE_RANGE + m_rangeContraction > m_shakeRange)
	{
		//反転
		m_shakeReverseCheck = !m_shakeReverseCheck;
		//範囲の収縮
		m_rangeContraction += CONTRACTION_RANGE;
		//揺れ幅の調整
		m_shakeRange = -MAX_SHAKE_RANGE + m_rangeContraction;
	}

	//揺れ終了
	if (m_rangeContraction >= MAX_SHAKE_RANGE)
	{
		m_shake = false;
	}
}

//ビュー行列の取得
DirectX::SimpleMath::Matrix FollowCamera::GetView() const
{
	return m_viewMatrix;
}

//ビュー行列の設定
void FollowCamera::SetView(const DirectX::SimpleMath::Matrix &view)
{
	m_viewMatrix = view;
}


//プロジェクション行列の取得
DirectX::SimpleMath::Matrix FollowCamera::GetProjection() const
{
	return m_pProjectionMatrix;
}

//プロジェクション行列の設定
void FollowCamera::SetProjection(const DirectX::SimpleMath::Matrix &proj)
{
	m_pProjectionMatrix = proj;
}


//カメラの見る対象を設定
void FollowCamera::SetTargetPosition(const DirectX::SimpleMath::Vector3 &targetPos)
{
	m_targetPos = targetPos;
}

//カメラの位置の取得
DirectX::SimpleMath::Vector3 FollowCamera::GetEysPos() const
{
	return m_eyePos;
}

//カメラの位置の設定
void FollowCamera::SetEyePosition(const DirectX::SimpleMath::Vector3 &eyePos)
{
	m_eyePos = eyePos;
}