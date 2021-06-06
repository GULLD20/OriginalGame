#pragma once

#include "../Enemy.h"

#include "../../MyEffect/Effect2D/Enemy/AttackLine.h"

///======================================================
///内容
///プレイヤーに向かって突進を続ける
///======================================================
class Goblin : public Enemy
{
public:

public:
	//攻撃時間
	const static float ATTACK_TIME;
	//攻撃範囲
	const static float ATTACKRANGE;
	//攻撃間隔
	const static int   ATTACK_INTERVAL;
	//戦闘範囲
	const static float BATTLE_RANGE;

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
	Goblin(const DirectX::SimpleMath::Vector3 &pos, float attack, float hp, float speed, float thisRange);
	~Goblin();

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
	void SelectMove()override;
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

private:
	//攻撃範囲
	std::unique_ptr<AttackLine> m_pAttackLine;
};