#include "SimpleMath.h"

#define FIXCAMERA_H
#ifdef FIXCAMERA_H
#pragma once

class GridFloor;

class FixCamera
{
public:
	FixCamera(float fov, float aspect);
	~FixCamera();

	void Initialize();
	void Update();

	DirectX::SimpleMath::Vector3 GetCameraPos();
	void SetCameraPos(DirectX::SimpleMath::Vector3 cameraPos);

	DirectX::SimpleMath::Matrix GetView();
	void SetView(DirectX::SimpleMath::Matrix view);

	DirectX::SimpleMath::Matrix GetProjection();
	void SetProjection(DirectX::SimpleMath::Matrix proj);

private:
	//ビュー行列
	DirectX::SimpleMath::Matrix m_ViewMatrix;
	//射影行列
	DirectX::SimpleMath::Matrix m_pProjectionMatrix;

	//カメラ位置
	DirectX::SimpleMath::Vector3 m_CameraPos = DirectX::SimpleMath::Vector3(1.0f,1.0f,1.0f);
};

#endif
