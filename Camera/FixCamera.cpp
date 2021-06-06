#include "pch.h"
#include "FixCamera.h"


FixCamera::FixCamera(float fov, float aspect)
{
	DirectX::SimpleMath::Vector3 eye = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 10.0f);
	DirectX::SimpleMath::Vector3 target = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
	DirectX::SimpleMath::Vector3 up = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);

	m_ViewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(eye, target, up);

	// éÀâeçsóÒÇçÏê¨Ç∑ÇÈ
	m_pProjectionMatrix = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		fov,
		aspect,
		0.01f,
		10000.0f
	);
}


FixCamera::~FixCamera()
{
}

void FixCamera::Initialize()
{

}

void FixCamera::Update()
{

}

DirectX::SimpleMath::Vector3 FixCamera::GetCameraPos()
{
	return m_CameraPos;
}

void FixCamera::SetCameraPos(DirectX::SimpleMath::Vector3 cameraPos)
{
	m_CameraPos = cameraPos;
}


DirectX::SimpleMath::Matrix FixCamera::GetView()
{
	return m_ViewMatrix;
}

void FixCamera::SetView(DirectX::SimpleMath::Matrix view)
{
	m_ViewMatrix = view;
}


DirectX::SimpleMath::Matrix FixCamera::GetProjection()
{
	return m_pProjectionMatrix;
}

void FixCamera::SetProjection(DirectX::SimpleMath::Matrix proj)
{
	m_pProjectionMatrix = proj;
}