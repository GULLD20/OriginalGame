#include "../../../pch.h"
#include "SlashEffect.h"

#include <CommonStates.h>
#include "../../../DeviceResources.h"

#include "../../../Scene/GameContext.h"
#include "../../../Scene/Projection.h"

//テーブル
static uint16_t s_indices[] =
{
	0, 1, 2,
	2, 1, 3,
};

//頂点の数
const int SlashEffect::VERTEX_NUM = 10;

SlashEffect::SlashEffect()
{
}


SlashEffect::~SlashEffect()
{
	// プリミティブバッチ
	m_pPrimitiveBatch.reset(nullptr);
	//頂点の位置と色
	m_pOrbitVertex.clear();
	// ベーシックエフェクト
	m_pBasicEffect.reset(nullptr);
	// 入力レイアウト
	m_pInputLayout.Reset();
}

//作成
void SlashEffect::Creat()
{
	DX::DeviceResources* deviceResources = GameContext<DX::DeviceResources>::Get();

	ID3D11DeviceContext1*  context = deviceResources->GetD3DDeviceContext();
	ID3D11Device1* device = deviceResources->GetD3DDevice();

	// プリミティブバッチ作成
	m_pPrimitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(context);
	// ポリゴン用エフェクト作成
	m_pBasicEffect = std::make_unique<DirectX::BasicEffect>(device);
	void const * shaderByteCode;
	size_t byteCodeLength;
	//頂点の色を有効にする
	m_pBasicEffect->SetVertexColorEnabled(true);
	// シェーダー取得
	m_pBasicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	// 入力レイアウトの作成（頂点データの定義する）
	device->CreateInputLayout(DirectX::VertexPositionColor::InputElements,
		DirectX::VertexPositionColor::InputElementCount,
		shaderByteCode, byteCodeLength,
		m_pInputLayout.GetAddressOf());
}

//初期化
void SlashEffect::Initialzie()
{
	//頂点の数を設定する
	m_pOrbitVertex.resize(VERTEX_NUM);

	//頂点ごとの色、透明度を決める
	for (int i = 0; i<VERTEX_NUM; i++)
	{
		//
		m_pOrbitVertex[i].color.x = 1.0f;
		m_pOrbitVertex[i].color.y = 1.0f;
		//m_pOrbitVertex[i].color.z = 0.5f;

		//透明度の設定
		//先端に近いほど透明度が高くなる
		//先端から離れているほど透明度が低くなる
		if (i % 2 == 0)
		{
			m_pOrbitVertex[i].color.w = 1.0f - i / static_cast<float>(VERTEX_NUM);
		}
		else
		{
			m_pOrbitVertex[i].color.w = 0.25f - i* 0.25f / static_cast<float>(VERTEX_NUM);
		}
	}
}

//更新
void SlashEffect::Update()
{
	//斬撃の先端と末端
	DirectX::SimpleMath::Vector3 posT = DirectX::SimpleMath::Vector3::Up * 0.75f;
	DirectX::SimpleMath::Vector3 posB = DirectX::SimpleMath::Vector3::Up * -0.5f;

	for (int i = VERTEX_NUM - 4; i >= 0; i -= 2)
	{
		m_pOrbitVertex[i + 2].position = m_pOrbitVertex[i].position;
		m_pOrbitVertex[i + 3].position = m_pOrbitVertex[i + 1].position;
	}

	m_pOrbitVertex[0].position = DirectX::SimpleMath::Vector3::Transform(posT, m_weaponMatrix);
	m_pOrbitVertex[1].position = DirectX::SimpleMath::Vector3::Transform(posB, m_weaponMatrix);
}

//描画
void SlashEffect::Draw(const DirectX::SimpleMath::Matrix &view)
{
	DX::DeviceResources* deviceResources = GameContext<DX::DeviceResources>::Get();
	DirectX::CommonStates* state = GameContext<DirectX::CommonStates>::Get();

	ID3D11DeviceContext1*  context = deviceResources->GetD3DDeviceContext();


	// TODO: Add your rendering code here.
	DirectX::SimpleMath::Matrix world;

	// ワールド行列設定
	m_pBasicEffect->SetWorld(world);
	// ビュー行列設定
	m_pBasicEffect->SetView(view);
	// プロジェクション行列設定
	m_pBasicEffect->SetProjection(GameContext<Projection>::Get()->GetMatrix());
	// エフェクトの設定
	m_pBasicEffect->Apply(context);
	// 深度ステンシルステートの設定
	context->OMSetDepthStencilState(state->DepthDefault(), 0);
	// ブレンドステートの設定
	context->OMSetBlendState(state->NonPremultiplied(), nullptr, 0xffffffff);
	// ラスタライザステートを設定
	context->RSSetState(state->CullNone());
	// 入力レイアウトの設定
	context->IASetInputLayout(m_pInputLayout.Get());

	// 描画開始
	m_pPrimitiveBatch->Begin();
	// 頂点情報から描画
	for (int i = 0; i<VERTEX_NUM - 2; i += 2)
	{
		//D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
		//D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
		m_pPrimitiveBatch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, s_indices, 6, &m_pOrbitVertex[i], 4);
	}
	// 描画終了
	m_pPrimitiveBatch->End();
}

//リセット
void SlashEffect::Reset()
{

}

//武器の刃の中心と刃の中心から先端に向けた長さ
void SlashEffect::SetWeaponData(const DirectX::SimpleMath::Vector3 &weaponC, const DirectX::SimpleMath::Vector3 &weaponC_T)
{
	//武器の中心と中心から先端
	m_weaponC =weaponC;
	m_weaponC_T = weaponC_T;
}

//武器のマトリックス
void SlashEffect::SetWeaponMatrix(const DirectX::SimpleMath::Matrix &matrix)
{
	m_weaponMatrix = matrix;
}