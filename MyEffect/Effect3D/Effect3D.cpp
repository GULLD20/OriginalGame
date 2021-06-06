//----------------------------------------
// Effect3D.cpp
//----------------------------------------

#include "../../pch.h"
#include "Effect3D.h"

#include "../../DeviceResources.h"
#include <SimpleMath.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>

void Effect3D::Create(DX::DeviceResources* &deviceResources,const wchar_t* fileName,const DirectX::SimpleMath::Vector3 &pos)
{
	m_pDeviceResources = deviceResources;
	auto device = m_pDeviceResources->GetD3DDevice();
	// エフェクトの作成
	m_pBatchEffect = std::make_unique<DirectX::AlphaTestEffect>(device);
	m_pBatchEffect->SetAlphaFunction(D3D11_COMPARISON_EQUAL);
	m_pBatchEffect->SetReferenceAlpha(255);
	// 入力レイアウト生成
	void const* shaderByteCode;
	size_t byteCodeLength;
	m_pBatchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	device->CreateInputLayout(DirectX::VertexPositionTexture::InputElements,
		DirectX::VertexPositionTexture::InputElementCount,
		shaderByteCode, byteCodeLength, m_pInputLayout.GetAddressOf());


	// プリミティブバッチの作成
	m_pBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>>(m_pDeviceResources->GetD3DDeviceContext());

	m_pStates = std::make_unique<DirectX::CommonStates>(device);
	// テクスチャのロード
	DirectX::CreateWICTextureFromFile(device, fileName, nullptr,
		m_pTexture.GetAddressOf());
	m_position = pos;
}

void Effect3D::Update(const DirectX::SimpleMath::Vector3 &thisPos, const DirectX::SimpleMath::Vector3 &targetPos, const DirectX::SimpleMath::Matrix &matrix)
{
	//自分から対象に向けたベクトル
	DirectX::SimpleMath::Vector3 enemy_target_Vector;
	DirectX::SimpleMath::Vector3 baceFrontVec = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);

	enemy_target_Vector = targetPos - thisPos;

	//自分から対象に向けたベクトルの正規化
	enemy_target_Vector.Normalize();

	//二つのベクトルの内積を求める
	float dot = enemy_target_Vector.Dot(baceFrontVec);
	//求めた内積のなす角を求める
	float angle = std::acos(dot);

	//二つのベクトルの外積を求める
	DirectX::SimpleMath::Vector3 cros = enemy_target_Vector.Cross(baceFrontVec);

	//外積が0の時(二つのベクトルが重なっているとき)外積の値をDirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f)で固定する
	if (cros.Length() == 0.0f) cros = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);

	//求めた外積と内積をQuaternionに変換
	DirectX::SimpleMath::Quaternion qua = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(-cros, angle);

	//変換したQuaternionを使って回転行列を作成
	DirectX::SimpleMath::Matrix Rotmat = DirectX::SimpleMath::Matrix::Transform(DirectX::SimpleMath::Matrix::Identity, qua);

	//回転行列を代入
	m_world = Rotmat * matrix;
	m_world *= DirectX::SimpleMath::Matrix::CreateTranslation(thisPos);
}

void Effect3D::Render()
{
	//m_world = DirectX::SimpleMath::Matrix::CreateBillboard(m_position, m_camera, DirectX::SimpleMath::Vector3::UnitY);
	Draw(m_world, m_view, m_proj);
}

void Effect3D::Reset()
{
	// 入力レイアウト
	m_pInputLayout.Reset();;

	// テクスチャハンドル
	m_pTexture.Reset();
}

void Effect3D::SetRenderState(const DirectX::SimpleMath::Vector3 &camera,const DirectX::SimpleMath::Matrix &view, const DirectX::SimpleMath::Matrix &proj)
{
	m_camera = camera;
	m_view = view;
	m_proj = proj;
}

void Effect3D::Draw(const DirectX::SimpleMath::Matrix &world, const  DirectX::SimpleMath::Matrix &view, const DirectX::SimpleMath::Matrix &proj)
{
	auto context = m_pDeviceResources->GetD3DDeviceContext();

	// 頂点情報（板ポリゴンの頂点）
	DirectX::VertexPositionTexture vertex[4] =
	{
		DirectX::VertexPositionTexture(DirectX::SimpleMath::Vector3(m_textureScaleTL.x, m_textureScaleTL.y, 0.0f), DirectX::SimpleMath::Vector2(m_textureSizeTL.x, m_textureSizeTL.y)),
		DirectX::VertexPositionTexture(DirectX::SimpleMath::Vector3(m_textureScaleBR.x,  m_textureScaleTL.y, 0.0f),DirectX::SimpleMath::Vector2(m_textureSizeBR.x, m_textureSizeTL.y)),
		DirectX::VertexPositionTexture(DirectX::SimpleMath::Vector3(m_textureScaleBR.x, m_textureScaleBR.y, 0.0f), DirectX::SimpleMath::Vector2(m_textureSizeBR.x,m_textureSizeBR.y)),
		DirectX::VertexPositionTexture(DirectX::SimpleMath::Vector3(m_textureScaleTL.x,m_textureScaleBR.y, 0.0f), DirectX::SimpleMath::Vector2(m_textureSizeTL.x, m_textureSizeBR.y)),
	};
	// テクスチャサンプラーの設定（クランプテクスチャアドレッシングモード）
	ID3D11SamplerState* samplers[1] = { m_pStates->LinearClamp() };
	context->PSSetSamplers(0, 1, samplers);
	ID3D11BlendState* blendstate = m_pStates->NonPremultiplied();

	CD3D11_DEFAULT default_state;

	CD3D11_BLEND_DESC bddesc(default_state);

	blendstate->GetDesc(&bddesc);
	bddesc.AlphaToCoverageEnable = TRUE;
	m_pDeviceResources->GetD3DDevice()->CreateBlendState(&bddesc, &blendstate);

	// 透明判定処理
	context->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);
	// 深度バッファに書き込み参照する
	context->OMSetDepthStencilState(m_pStates->DepthDefault(), 0);
	// カリングは左周り（反時計回り）
	//context->RSSetState(m_pStates->CullCounterClockwise());
	context->RSSetState(m_pStates->CullNone());
	// 不透明のみ描画する設定
	m_pBatchEffect->SetAlphaFunction(D3D11_COMPARISON_NOT_EQUAL);
	m_pBatchEffect->SetReferenceAlpha(0);
	m_pBatchEffect->SetWorld(m_world);
	m_pBatchEffect->SetView(m_view);
	m_pBatchEffect->SetProjection(m_proj);
	m_pBatchEffect->SetTexture(m_pTexture.Get());
	m_pBatchEffect->Apply(context);
	context->IASetInputLayout(m_pInputLayout.Get());

	// 半透明部分を描画
	m_pBatch->Begin();
	m_pBatch->DrawQuad(vertex[0], vertex[1], vertex[2], vertex[3]);
	m_pBatch->End();

	//作成したデータを削除
	blendstate->Release();
}

void Effect3D::RotationY(const DirectX::SimpleMath::Vector3 &thisPos, const DirectX::SimpleMath::Vector3 &targetPos)
{
	m_position = thisPos;

	DirectX::SimpleMath::Vector3 targetPos_thisPos_vec = targetPos - thisPos;

	targetPos_thisPos_vec.Normalize();

	float dot = targetPos_thisPos_vec.Dot(DirectX::SimpleMath::Vector3::Forward);

	if (dot >= 1.0f)
	{
	dot = 1.0f;
	}

	float angle = std::acos(dot);
	float cross = targetPos_thisPos_vec.x*	DirectX::SimpleMath::Vector3::Forward.z - targetPos_thisPos_vec.z*	DirectX::SimpleMath::Vector3::Forward.x;

	float AngleY;

	if (cross <= 0)
	{
	AngleY = -angle;
	}
	else if(cross >= 0)
	{
	AngleY = angle;
	}

	m_world = DirectX::SimpleMath::Matrix::CreateRotationY(AngleY);
	m_world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_position);
}

//画像の大きさの設定
void Effect3D::SetTextureScaleTL(const DirectX::SimpleMath::Vector2 &size)
{
	m_textureScaleTL = size;
}

void Effect3D::SetTextureScaleBR(const DirectX::SimpleMath::Vector2 &size)
{
	m_textureScaleBR = size;
}

//画像の切り取り位置を設定
void Effect3D::SetTextureSizeTL(const DirectX::SimpleMath::Vector2 &size)
{
	m_textureSizeTL = size;
}
void Effect3D::SetTextureSizeBR(const DirectX::SimpleMath::Vector2 &size)
{
	m_textureSizeBR = size;
}