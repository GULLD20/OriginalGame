//----------------------------------------
// Effect3D.h
//----------------------------------------
#pragma once

#include "../../DeviceResources.h"
#include <SimpleMath.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>

class Effect3D
{
public:
	void Create(DX::DeviceResources* &deviceResources,const wchar_t* fileName, const DirectX::SimpleMath::Vector3 &pos = DirectX::SimpleMath::Vector3::Zero);
	void Update(const DirectX::SimpleMath::Vector3 &thisPos, const DirectX::SimpleMath::Vector3 &targetPos, const DirectX::SimpleMath::Matrix &matrix);
	void Render();
	void Reset();

	void SetRenderState(const DirectX::SimpleMath::Vector3 &camera, const DirectX::SimpleMath::Matrix &view, const DirectX::SimpleMath::Matrix &proj);

	void RotationY(const DirectX::SimpleMath::Vector3 &thisPos, const DirectX::SimpleMath::Vector3 &targetPos);

	//画像の大きさの設定
	void SetTextureScaleTL(const DirectX::SimpleMath::Vector2 &size);
	void SetTextureScaleBR(const DirectX::SimpleMath::Vector2 &size);
	//画像の切り取り位置を設定
	void SetTextureSizeTL(const DirectX::SimpleMath::Vector2 &size);
	void SetTextureSizeBR(const DirectX::SimpleMath::Vector2 &size);
private:


	void Draw(const DirectX::SimpleMath::Matrix &world,const DirectX::SimpleMath::Matrix &view, const DirectX::SimpleMath::Matrix &proj);

private:
	DX::DeviceResources*                                                     m_pDeviceResources;
	DirectX::SimpleMath::Matrix                                              m_world;
	DirectX::SimpleMath::Matrix                                              m_view;
	DirectX::SimpleMath::Matrix                                              m_proj;
	// エフェクト
	std::unique_ptr<DirectX::AlphaTestEffect>                                m_pBatchEffect;
	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_pBatch;
	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout>                                m_pInputLayout;

	std::unique_ptr<DirectX::CommonStates>                                   m_pStates;
	// テクスチャハンドル
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                         m_pTexture;

	DirectX::SimpleMath::Vector3                                             m_position;
	DirectX::SimpleMath::Vector3                                             m_camera;

	//画像の大きさ
	//上と左
	DirectX::SimpleMath::Vector2                                             m_textureScaleTL;
	//下と右
	DirectX::SimpleMath::Vector2                                             m_textureScaleBR;

	//画像の切り取り位置
	//上と左
	DirectX::SimpleMath::Vector2                                             m_textureSizeTL;
	//下と右
	DirectX::SimpleMath::Vector2                                             m_textureSizeBR;
};