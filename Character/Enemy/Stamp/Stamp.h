#pragma once

#include "../Enemy.h"

#include "../../MyEffect/Effect2D/Enemy/AttackArea.h"


///======================================================
///内容
///ジャンプしてプレイヤーの上から落下して攻撃する敵
///======================================================
class Stamp : public Enemy
{
public:

public:
	//攻撃範囲
	const static float ATTACKRANGE;
	//攻撃時のジャンプの時間
	const static float ATTACKJUMPTIME;
	//攻撃時の移動の時間
	const static float ATTACKMOVETIME;
	//攻撃時の落下の時間
	const static float ATTACKFALLTIME;
	//攻撃間隔
	const static int   ATTACK_INTERVAL;
	//戦闘範囲
	const static float BATTLE_RANGE;
	//攻撃時の最大の高さの位置
	const static float ATTACKTOPPOS;

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
	Stamp(const DirectX::SimpleMath::Vector3 &pos, float attack, float hp, float speed, float thisRange);
	~Stamp();

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
	//攻撃時の処理
	//ジャンプ
	void AttackJump();
	//ジャンプ後の移動
	void AttackMove();
	//落下
	void AttackFall();
private:
	//攻撃範囲
	std::unique_ptr<AttackArea>  m_pAttackArea;

	//攻撃開始判定
	bool                         m_startAttack;
	//ジャンプ時の目的位置
	DirectX::SimpleMath::Vector3 m_jumpDestination;
	//攻撃時の移動開始位置
	DirectX::SimpleMath::Vector3 m_startMovePos;

	//攻撃のジャンプの時間計測
	float                        m_attackJumpTime;
	//攻撃の移動の時間計測
	float                        m_attackMoveTime;
	//攻撃の落下の時間計測
	float                        m_attackFallTime;

	//攻撃時の各判定
	//ジャンプ終了
	bool                         m_attackJump;
	//攻撃終了
	bool                         m_attackMove;
	//落下終了
	bool                         m_attackFall;

	//攻撃時にデータの初期化に1フレーム余裕を持たせるため
	int                          m_oneFrameWait;

	//攻撃終了判定
	bool                         m_endAttack;
};