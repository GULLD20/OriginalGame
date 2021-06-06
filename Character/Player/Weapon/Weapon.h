#pragma once

#include "Model.h"

#include <Effects.h>
#include "../../../DeviceResources.h"
#include <CommonStates.h>

#include <SimpleMath.h>

enum class WeaponType;

class Weapon
{
public:
	struct WeaponData
	{
		//武器のワールド行列
		DirectX::SimpleMath::Matrix		 matrix;
		//武器の原点から先端までのベクトル
		DirectX::SimpleMath::Vector3     vector;
		//攻撃状態
		bool                             attack;
		//武器の現在の角度
		DirectX::SimpleMath::Vector3     angle;
		//コンボ
		int                              combo;
		//フレーム
		float                            flame;
		//攻撃後の次のコンボまでの時間
		float                            betweenAttacksTime;
		//攻撃直後
		bool                             afterTheAttack;
	};

	const static float BETWEENATTACKSTIME;
public:
	Weapon();
	virtual ~Weapon();

	virtual void Create(DX::DeviceResources* &DeviceResources) = 0;
	//初期化処理
	virtual void Initialize() = 0;
	//更新処理
	virtual void Update(float elapsedSeconds) = 0;
	//描画処理
	virtual void Draw(ID3D11DeviceContext1* context, const DirectX::CommonStates& State, const DirectX::SimpleMath::Matrix &viewMatrix, const DirectX::SimpleMath::Matrix &proj) = 0;
	//リセット
	virtual void Reset() = 0;

	//攻撃開始
	virtual void StartAttack() = 0;

	//攻撃中か取得
	virtual bool GetAttackSTate() const = 0;

	//コンボ取得
	virtual int GetCombo()const { return m_weapon.combo; };

	//Matrixを設定する
	virtual void MatrixSetting() = 0;

	void SetPlayerMatrix(const DirectX::SimpleMath::Matrix &playerMatrix) { m_playerMatrix = playerMatrix; };

	//中心から先端に向けたベクトルの取得
	DirectX::SimpleMath::Vector3 GetVector()const { return m_weapon.vector; };
	//中心位置を取得
	DirectX::SimpleMath::Vector3 GetPos()const { return m_weapon.matrix.Translation(); };


private:
	//攻撃処理(回転)
	virtual void Attack() = 0;
	//攻撃終了
	virtual void EndAttack() = 0;


	//武器の関連データの初期化
	virtual void ResetWeaponData() = 0;

public:
	//武器の関連データ
	WeaponData                       m_weapon;
	//モデル
	std::unique_ptr<DirectX::Model>  m_pModel;
	//プレイヤーの行列
	DirectX::SimpleMath::Matrix      m_playerMatrix;
};