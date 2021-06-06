#pragma once

#include <vector>
#include <SimpleMath.h>
#include <CommonStates.h>
#include <PrimitiveBatch.h>
#include <Effects.h>
#include <VertexTypes.h>

class SlashEffect
{
public:
	//頂点の数
	const static int VERTEX_NUM;

public:
	SlashEffect();
	~SlashEffect();

	//作成
	void Creat();
	//初期化
	void Initialzie();
	//更新
	void Update();
	//描画
	void Draw(const DirectX::SimpleMath::Matrix &view);
	//リセット
	void Reset();

	//武器の刃の中心と刃の中心から先端に向けた長さ
	void SetWeaponData(const DirectX::SimpleMath::Vector3 &weaponC, const DirectX::SimpleMath::Vector3 &weaponC_T);
	//武器のマトリックス
	void SetWeaponMatrix(const DirectX::SimpleMath::Matrix &matrix);

private:
	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_pPrimitiveBatch;
	//頂点の位置と色
	std::vector<DirectX::VertexPositionColor>                              m_pOrbitVertex;
	// ベーシックエフェクト
	std::unique_ptr<DirectX::BasicEffect>                                  m_pBasicEffect;
	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout>                              m_pInputLayout;

	//武器のワールド行列
	DirectX::SimpleMath::Matrix	                                           m_weaponMatrix;

	//武器の中心と中心から先端
	DirectX::SimpleMath::Vector3                                           m_weaponC;
	DirectX::SimpleMath::Vector3                                           m_weaponC_T;
};

