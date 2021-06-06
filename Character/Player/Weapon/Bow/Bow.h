#pragma once
#include "../Weapon.h"

#include <SimpleMath.h>

#include <vector>

class Bow :	public Weapon
{
public:
	//弓のサイズ
	const static DirectX::SimpleMath::Vector3              SIZE;
	//弓の基本位置(移動中)
	const static DirectX::SimpleMath::Vector3              BASE_POSITION;
	//弓の基本位置(攻撃中)
	const static DirectX::SimpleMath::Vector3              BASE_POSITION_ATTACK;
	//武器の回転(コンボ)
	const static std::vector<DirectX::SimpleMath::Vector3> ANGLE;
	//コンボの数
	const static int                                       MAX_COMBO;
	//攻撃時間
	const static float                                     ATTACK_FLAME;

public:
	Bow();
	~Bow();

	void Create(DX::DeviceResources* &DeviceResources) override;
	//初期化処理
	void Initialize() override;
	//更新処理
	void Update(float elapsedSeconds) override;
	//描画処理
	void Draw(ID3D11DeviceContext1* context, const DirectX::CommonStates& State, const DirectX::SimpleMath::Matrix &viewMatrix, const DirectX::SimpleMath::Matrix &proj) override;
	//リセット
	void Reset() override;

	//攻撃開始
	void StartAttack()override;
	//攻撃中か取得
	bool GetAttackSTate()const override;

	//Matrixを設定する
	void MatrixSetting() override;
private:
	//攻撃処理(回転)
	void Attack() override;
	//攻撃終了
	void EndAttack() override;

	//武器の関連データの初期化
	void ResetWeaponData() override;

private:

};

