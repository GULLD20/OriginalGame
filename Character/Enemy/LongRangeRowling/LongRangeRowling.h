#pragma once

#include "../Enemy.h"

#include "../../MyEffect/Effect2D/Enemy/AttackLine.h"

#include "../LongRangeAttack/LongRangeAttack.h"

///======================================================
///内容
///常に回転して4方向に向けて遠距離攻撃を行う砲台
///======================================================
class LongRangeRowling : public Enemy
{
public:
	//攻撃時間
	const static float ATTACK_TIME;
	//攻撃範囲
	const static float ATTACKRANGE;
	//攻撃間隔
	const static float ATTACK_INTERVAL;
	//戦闘範囲
	const static float BATTLE_RANGE;
	//弾の攻撃速度(1秒間の速度)
	const static float ATTACK_SPEED;

	//プレイヤーを吹っ飛ばす力(1秒間に)
	const static float BLOW_AWAY_POWER_PLAYER_TARGET;

	//エフェクトの個数
	//攻撃範囲
	const static int   NUMBER_OF_EFFECT_ATTACK_AREA;
	//攻撃が当たった時
	const static int   NUMBER_OF_EFFECT_HIT;
	//死亡
	const static int   NUMBER_OF_EFFECT_DETH;

	//スロウ状態の計算用の数値
	const static float SLOW_CAL;
	//通常状態の計算用の数値
	const static float NORMAL_CAL;

public:
	LongRangeRowling(const DirectX::SimpleMath::Vector3 &pos, float attack, float hp, float speed, float thisRange);
	~LongRangeRowling();

	//エフェクトの作成
	void CreateEffect() override;

	//派生クラス用更新処理
	void SubInitialize() override;
	//エフェクトの初期化
	void EffectInitialize() override;

	//派生クラス用更新処理
	void SubUpdate()override;
	//エフェクトの更新処理
	void EffectUpdate(const DX::StepTimer &timer)override;

	//生存時の描画処理
	void AliveDraw(const DirectX::SimpleMath::Matrix &viewMatrix, const DirectX::SimpleMath::Matrix &projection)override;

	//戦闘中処理
	void Battle()override;
	//攻撃処理
	void Attack()override;
	//停止処理
	void Stop()override;


	//移動判定
	void SelectMove();
	//戦闘開始判定
	void SelectBattle()override;
	//攻撃開始判定
	void SelectAttack()override;
	//停止判定
	void SelectStop()override;

	//プレイヤーのジャスト回避判定
	void PlayerJustAvoidance()override;

	//攻撃終了
	void EndAttack()override;

public:
	//攻撃の当たり判定
	void HitAttack();

private:
	//攻撃範囲
	std::unique_ptr<AttackLine>      m_pAttackLine;
	//攻撃クラス(弾)
	std::unique_ptr<LongRangeAttack> m_pLongRangeAttack;
	//回転角度
	float                            m_anglY;
};