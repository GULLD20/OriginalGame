#pragma once

#include <GeometricPrimitive.h>
#include <SimpleMath.h>
#include <Effects.h>
#include <CommonStates.h>
#include <vector>
#include "Model.h"
#include <string>
#include "../../DeviceResources.h"

#include "../../Scene/Projection.h"
#include "../../Scene/GameContext.h"

#include "../../DebugObj/DebugCapsule.h"

#include "../../MyEffect/Effect3D/Effect3D.h"

#include "../Character/Character.h"

#include "../../MyEffect/Effect2D/Enemy/EnemyDeth.h"
#include "../../MyEffect/Effect2D/Enemy/EnemyHit.h"

class DX::DeviceResources;

class Enemy: public Character
{
public:

public:
	//敵の大きさ上と下
	const static DirectX::SimpleMath::Vector3  ENEMYSIZE_TOP_DOWN;


	//吹っ飛ばされる時間
	const static float                         BLOW_AWAY_TIME;

	//無敵時間
	const static float                         INVINCIBLE_TIME;

	//スロウ状態の計算用の数値
	const static float                         SLOW_CAL;
	//通常状態の計算用の数値
	const static float                         NORMAL_CAL;

	//スロウ時間
	const static float                         SLOW_TIME;
public: //コンスト　デスト
	Enemy(const DirectX::SimpleMath::Vector3 &pos, float attack, float hp, float speed, float thisRange);
	virtual ~Enemy();

public: //基底クラスを継承した関数

	//基底クラスで書く関数
	//モデルの作成
	void CreateModel(const wchar_t* fileName);
	//初期化処理
	void Initialize()override;
	//更新処理
	void Update(const DX::StepTimer &timer)override;
	//描画処理
	void Draw(const DirectX::SimpleMath::Matrix &viewMatrix)override;
	//リセット処理
	void Reset()override;

	//ダメージ処理
	void Damage(float attack, const DirectX::SimpleMath::Matrix &rotation, bool blowAway, float power)override;

	//吹っ飛び処理
	void BlowAwayMove()override;

public:
	//各Update関数-----------------
	//State関連の更新処理
	void StateUpdate();
	//State関連のメインのUpdate
	void StateMainUpdate();
	//プレイヤーのジャスト回避時間の更新処理
	void JustAvoidanceUpdate();
	//無敵時間の更新処理
	void InvincibleUpdate();
	//死亡時の更新処理
	bool DethUpdate(DX::StepTimer timer);
	//Mapとの当たり判定
	void HitMap();
	//-----------------------------

	//死亡時の描画処理
	void DethDraw(const DirectX::SimpleMath::Matrix &viewMatrix, const DirectX::SimpleMath::Matrix &projection);

	//回転処理
	void Rotation(float angle);

	//正面に移動処理
	void Move();

	//Matrixの設定
	void MatrixSetting();

	//ターゲットの位置の設定
	void TargetosSet();

	//ステートの設定
	void SelectState();

	//データを設定する
	void DataSetting();

	//プレイヤーのIDを設定
	void SetPlayerID(int id);

	//自分以外のEnemyを設定
	void SetOtherEnemy(const std::vector<std::unique_ptr<Enemy>> &enemyList, int enemyNum);

	//範囲にいるか(カメラ描画範囲)
	void CameraWithinRange();

	//死亡判定
	bool IsDeth();

	//カメラの位置の取得
	void SetEysPos(const DirectX::SimpleMath::Vector3 &pos);

	//HPの画像サイズの設定
	void HPTextureSize();

	//スロウ状態の開始
	void StartSlow();

	//スロウ状態か取得
	bool GetSlow() const { return m_justAvoidance; };

public:
	//派生クラスで書く関数
	//エフェクトの作成
	virtual void CreateEffect() = 0;
	//派生クラス用更新処理
	virtual void SubInitialize() = 0;
	//エフェクトの初期化
	virtual void EffectInitialize() = 0;

	//エフェクトの更新処理
	virtual void EffectUpdate(const DX::StepTimer &timer) =0;
	//派生クラス用更新処理
	virtual void SubUpdate() = 0;

	//生存時の描画処理
	virtual void AliveDraw(const DirectX::SimpleMath::Matrix &viewMatrix, const DirectX::SimpleMath::Matrix &projection) = 0;

	//戦闘中処理
	virtual void Battle() = 0;
	//攻撃処理
	virtual void Attack() = 0;
	//停止処理
	virtual void Stop() = 0;

	//移動判定
	virtual void SelectMove() = 0;
	//戦闘開始判定
	virtual void SelectBattle() = 0;
	//攻撃開始判定
	virtual void SelectAttack() = 0;
	//停止判定
	virtual void SelectStop() = 0;

	//プレイヤーのジャスト回避判定
	virtual void PlayerJustAvoidance() = 0;

	//攻撃終了
	virtual void EndAttack() = 0;

public:
	//相手を発見
	bool                                            m_discovery;
	//エネミーが生きているか判定
	bool                                            m_alive;

	//モデル
	std::unique_ptr<DirectX::Model>                 m_pModel;

	//影のモデル
	std::unique_ptr<DirectX::Model>                 m_shadow;
	//スピード計算用
	float                                           m_speedCal;
	//徘徊位置を指定する
	int                                             m_wanderingAssignment;
	//範囲内にいるか確認
	bool                                            m_withinRange;
	//歩いているか判定
	bool                                            m_walk;
	//待機フレーム
	int                                             m_waitFrameCount;
	//待機中の角度変更用(Lerp操作用)
	float                                           m_lerpAdjustment;
	//プレイヤーの範囲内(描画判定)
	bool                                            m_checkCamera;
	//1秒間の回転角度
	float                                           m_angle;

	//1フレーム前のなす角
	float                                           m_oldAco;

	//HP表示
	std::unique_ptr <Effect3D>                      m_pHPGuage;
	//名前表示
	std::unique_ptr <Effect3D>                      m_pName;
	//攻撃が当たった時の演出(プレイヤーの攻撃がエネミーに当たったら)
	std::unique_ptr<EnemyHit>                       m_pEnemyHit;
	//死亡演出
	std::unique_ptr<EnemyDeth>                      m_pEnemyDeth;

	//攻撃が当たった時のエフェクトのアルファ値
	float                                           m_hitEffectAlpha;
	//死亡エフェクトのアルファ値
	float                                           m_dethEffectAlpha;

	//カメラの位置
	DirectX::SimpleMath::Vector3                    m_eyePos;

	//ターゲットの位置
	DirectX::SimpleMath::Vector3                    m_targetPos;

	//攻撃の開始・終了判定
	bool                                            m_attack_start_end;

	float                                           m_attackTime;

	//攻撃の間隔
	float                                           m_attackInterval;
	//1フレームにかかった時間
	float                                           m_elapsedTime;

	//ジャスト回避
	//スロウ状態か
	bool                                            m_justAvoidance;
	//スロウ状態が終了するまでの時間
	float                                           m_justAvoidanceTime;

	//プレイヤーのID
	int                                             m_playerID;
	//全てのEnemy
	std::vector<Enemy*>                             m_pOtherEnemyList;

	//エネミーの吹っ飛び判定(trueなら吹っ飛ぶ・falseなら吹っ飛ばない   エネミーによって変える)
	bool                                            m_blowAway;

	//プレイヤーの攻撃のコンボが最後か
	bool                                            m_playerAttackCombLast;

	//吹っ飛ぶ方向の角度(行列)
	DirectX::SimpleMath::Matrix                     m_blowAwayRotation;
	//吹っ飛ぶ力(1秒間)
	float                                           m_blowAwayPower;
	//吹っ飛び時間の計測
	float                                           m_blowAwayTime;
};