#pragma once

#include "../../../DeviceResources.h"
#include "../../StepTimer.h"
#include <SimpleMath.h>
#include "../../Myeffect.h"
#include <Mouse.h>
#include <Keyboard.h>

#include <list>

class HitWall
{
public:
	struct ConstBuffer
	{
		DirectX::SimpleMath::Matrix		matWorld;
		DirectX::SimpleMath::Matrix		matView;
		DirectX::SimpleMath::Matrix		matProj;
		DirectX::SimpleMath::Vector4	Time;
		DirectX::SimpleMath::Vector4	Mouse;
		DirectX::SimpleMath::Vector4	Mode;
	};
	static const std::vector<D3D11_INPUT_ELEMENT_DESC> INPUT_LAYOUT;
	void Create(DX::DeviceResources* &deviceResources, const wchar_t* fileName, int count);
	void Initialize(float life, const DirectX::SimpleMath::Vector3 &pos);
	void InitializeNormal(float life, const DirectX::SimpleMath::Vector3 &pos);
	void InitializeCorn(float life, const DirectX::SimpleMath::Vector3 &pos, const DirectX::SimpleMath::Vector3 &dir);
	void Update(const DX::StepTimer &timer);
	void Render();
	void Lost();

	void SetRenderState(const DirectX::SimpleMath::Vector3 &camera, const DirectX::SimpleMath::Matrix &view, const DirectX::SimpleMath::Matrix &proj);
	void Draw(const DirectX::SimpleMath::Matrix &world, const DirectX::SimpleMath::Matrix &view, const DirectX::SimpleMath::Matrix &proj);

	void RenderModel();

	//位置の設定
	//プレイヤーの位置
	void  SetPlayerPos(DirectX::SimpleMath::Vector3 pos) { m_playerPos = pos; };
	//壁の位置
	void  SetWallPos(DirectX::SimpleMath::Vector3 pos) { m_wallPos = pos; };

	//角度の設定
	void SetRange(float angle) { m_angle = angle; };

	//壁の位置
	void  SetScale(DirectX::SimpleMath::Vector3 scale) { m_scale= scale; };


private:
	//MyEffect*					                                                  m_myEffect[10];
	std::list<MyEffect*>		                                                  m_pEffectList;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                              m_pTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                              m_pTexture2;


	DX::StepTimer                                                                 m_timer;


	DX::DeviceResources*			                                              m_pDeviceResources;
	Microsoft::WRL::ComPtr<ID3D11Buffer>	                                      m_pBuffer;
	std::unique_ptr<DirectX::CommonStates>                                        m_pStates;

	// 頂点シェーダ
	Microsoft::WRL::ComPtr<ID3D11VertexShader>                                    m_pVertexShader;
	// ピクセルシェーダ
	Microsoft::WRL::ComPtr<ID3D11PixelShader>                                     m_pPixelShader;
	// ジオメトリシェーダ
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>                                  m_pGeometryShader;

	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>> m_pBatch;
	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout>                                     m_pInputLayout;
	std::vector<DirectX::VertexPositionColorTexture>                              m_vertexList;

	DirectX::SimpleMath::Vector3                                                  m_centerPosition;

	DirectX::SimpleMath::Matrix                                                   m_billboardTranslation;
	DirectX::SimpleMath::Matrix                                                   m_view;
	DirectX::SimpleMath::Matrix                                                   m_proj;

	std::unique_ptr<DirectX::EffectFactory>                                       m_pFxFactory;


	Microsoft::WRL::ComPtr<ID3D11Texture2D>                                       m_pCapture;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	                              m_pRtv;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                              m_pSrv;

	DirectX::Keyboard::KeyboardStateTracker                                       m_keyStateTracker;

	int                                                                           m_menuId = 0;
	int                                                                           m_colorMode = 0;

	//プレイヤーの位置
	DirectX::SimpleMath::Vector3                                                  m_playerPos;

	//壁の位置
	DirectX::SimpleMath::Vector3                                                  m_wallPos;

	//角度
	float                                                                         m_angle;

	DirectX::SimpleMath::Vector3                                                  m_scale;

	//行列
	DirectX::SimpleMath::Matrix                                                   m_world;
};