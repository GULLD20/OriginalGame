#pragma once

#include <vector>
#include <Model.h>
#include <SimpleMath.h>

#include "../../../MyLib/Collision.h"

class LongRangeAttack
{
public:
	struct Bullet
	{
		//弾の位置
		DirectX::SimpleMath::Vector3 pos;
		//弾の発射時の位置
		DirectX::SimpleMath::Vector3 startPos;
		//ワールド行列
		DirectX::SimpleMath::Matrix matrix;
		//回転のみの行列
		DirectX::SimpleMath::Matrix rotation;
		//当たり判定
		Collision::Capsule3D capsule3D;
		//使っているかどうか
		bool active;
	};

public:
	//移動速度
	const static float                        SPEED;
	//当たり判定の中心から先端の長さ(Vector3)
	const static DirectX::SimpleMath::Vector3 BASE_CENTER_TOP;
	//当たり判定の円の大きさ
	const static float                        CIRCLE_SIZE;

public:
	//コンスト・デスト
	LongRangeAttack();
	~LongRangeAttack();

	//モデルの作成
	void CreatModel();
	//初期化処理
	void Initialize(float speed ,int attackRange);
	//更新処理
	void Update(float elapsedTime);
	//描画処理
	void Draw(const DirectX::SimpleMath::Matrix &viewMatrix, const DirectX::SimpleMath::Matrix &projection);
	//リセット
	void Reset();

	//攻撃開始処理
	void Shot(const DirectX::SimpleMath::Vector3 &pos, const DirectX::SimpleMath::Matrix &rotation);

	//攻撃が当たったなら
	void HitAttack();

	//当たり判定のGet
	Collision::Capsule3D GetCapsuleData(const DirectX::SimpleMath::Vector3 &playerPos);

	void SetSpeedCal(float speedCal) { m_speedCal = speedCal; };

private:
	//移動処理
	void Move(int select, float elapsedTime);
	//攻撃終了処理
	void AttackEnd(int select);
	//当たり判定の設定
	void CapsuleDataSetting(int select);
	//行列の設定
	void MatrixSetting(int select);

private:
	//弾
	std::vector<Bullet>             m_bulletList;
	//モデル
	std::unique_ptr<DirectX::Model> m_pModel;
	//速度
	float                           m_speed;
	//攻撃範囲
	int                             m_attackRange;
	//スピード計算用
	float                           m_speedCal;
	//返したカプセルデータ(当たり判定のデータ)の要素位置
	int                             m_capsuleSelect;
};