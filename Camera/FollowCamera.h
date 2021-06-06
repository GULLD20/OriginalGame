#pragma once

#include "SimpleMath.h"

class FollowCamera
{
public:
	//揺れる範囲(最大)
	const static int MAX_SHAKE_RANGE;
	//範囲の収縮する値
	const static float CONTRACTION_RANGE;
	//揺れる幅
	const static float SHAPED_WIDTH;
public:
	FollowCamera();
	~FollowCamera();

	void Initialize();
	void Update();

	//開始揺れ(引数は揺れる方向)
	void StartShake(const DirectX::SimpleMath::Vector3 &vec);
	//揺れ処理
	void Shake();

	//ビュー行列の取得
	DirectX::SimpleMath::Matrix GetView()const;
	//ビュー行列の設定
	void SetView(const DirectX::SimpleMath::Matrix &view);

	//プロジェクション行列の取得
	DirectX::SimpleMath::Matrix GetProjection()const;
	//プロジェクション行列の設定
	void SetProjection(const DirectX::SimpleMath::Matrix &proj);

	//カメラの見る対象を設定
	void SetTargetPosition(const DirectX::SimpleMath::Vector3 &targetPos);

	//カメラの位置の取得
	DirectX::SimpleMath::Vector3 GetEysPos()const;
	//カメラの位置の設定
	void SetEyePosition(const DirectX::SimpleMath::Vector3 &eyePos);

private:
	//ビュー行列
	DirectX::SimpleMath::Matrix  m_viewMatrix;
	//射影行列
	DirectX::SimpleMath::Matrix  m_pProjectionMatrix;

	//注子視点
	DirectX::SimpleMath::Vector3 m_targetPos;

	//視点の距離
	DirectX::SimpleMath::Vector3 m_eyePos;

	DirectX::SimpleMath::Vector3 m_up;

	// 参照視点
	DirectX::SimpleMath::Vector3 m_refEyePos;

	// 参照注視点
	DirectX::SimpleMath::Vector3 m_refTargetPos;

	//揺れる方向(幅)
	DirectX::SimpleMath::Vector3 m_shapedWidth;
	//揺れる時のカメラの位置
	DirectX::SimpleMath::Vector3 m_shakeCameraPos;
	//揺れ判定
	bool                         m_shake;
	//実際のカメラの揺れ
	float                        m_shakeRange;
	//揺れの反転
	bool                         m_shakeReverseCheck;
	//揺れの範囲
	float                        m_rangeContraction;
};