#pragma once

#include "Model.h"

#include <SimpleMath.h>
#include <Keyboard.h>
#include <Effects.h>
#include "../../DeviceResources.h"
#include <CommonStates.h>
#include "../../Scene/Projection.h"
#include "../../Scene/GameContext.h"

#include "../../MyEffect/Effect2D/HP/HPGuage.h"

#include "../Character/Character.h"

class Weapon;

class DebugCapsule;

class HPGuage;

enum class WeaponType;

class ADX2;

struct AvoidanceData
{
	//回避時間計測
	float TimeCount;
	//回避時の速さ
	float spead;
	//クールタイム
	float coolTimeCount;
	//回避したか判定
	bool Check;
};

class Player:public Character
{
public:  //enum・定数
	//方向
	enum Dir
	{
		None = 0,
		Front,
		Back,
		Left,
		Right
	};

	//プレイヤーの大きさの上と下
	const static DirectX::SimpleMath::Vector3 PLAYER_TOP_DOWN;

	//回避時間
	const static float                        AVOIDANCETIME;
	//回避のクールタイム
	const static float                        AVOIDANCECOOLTIME;
	//吹っ飛ばされる時間
	const static float                        BLOW_AWAY_TIME;
	//無敵時間
	const static float                        INVINCIBLE_TIME;

	//エネミーを吹っ飛ばす力(1秒間に)
	const static float                        BLOWAWAYPOWER_ENEMYTARGET;

public:  //コンスト・デスト
	Player(const DirectX::SimpleMath::Vector3 &pos, float attack, float hp, float speed, float thisRang);
	~Player();

public:  //継承した関数
	//初期化処理
	void Initialize()override;
	//更新処理
	void Update(const DX::StepTimer &timer)override;
	//描画処理
	void Draw(const DirectX::SimpleMath::Matrix &viewMatrix)override;
	//リセット処理
	void Reset()override;

	//ダメージ処理
	void Damage(float attack, const DirectX::SimpleMath::Matrix &rotation, bool blowAway , float power)override;

	//吹っ飛び処理
	virtual void BlowAwayMove()override;

public:  //このクラスで作成された関数
	//モデルと武器の作成
	void Create(DX::DeviceResources* &DeviceResources, const WeaponType &type);

	//攻撃が当たった処理
	void HitAttack(int id);
	//エネミーと当たっているとき
	void HItEnemy(int id);
	//Matrixの設定
	void MatrixSetting();
private:  //このクラス内でのみ呼び出される関数
	//無敵時間の更新処理
	void InvincibleUpdate();
	//角度の制限
	DirectX::SimpleMath::Vector3 LimitsAngle(const DirectX::SimpleMath::Vector3 &angle);


	//移動処理
	void Move();

	//攻撃処理
	void Attack();
	//攻撃の効果音の再生
	void PlayAttackSE();
	//攻撃時の移動速度を求める
	DirectX::SimpleMath::Vector3 CalAttackMoveVel(const DirectX::SimpleMath::Vector3 &front, const DirectX::SimpleMath::Vector3 &enemyPos);
	//自分の正面にいる敵の中で一番近い敵の位置を取得
	DirectX::SimpleMath::Vector3 EnemyDistanceClosePos(std::vector<int> allKey,const DirectX::SimpleMath::Vector3 &frontVecN);

	//回避のUpdate処理(Updateで呼び出す処理	)
	void AvoidanceUpdate();
	//回避の移動処理
	void AvoidanceMove();
	//ジャスト回避
	void JusrAvoidance();
	//プレイヤーの回転
	void Rotation();

	//キー入力
	void InputKey();
private:  //メンバ変数
	//武器
	std::unique_ptr<Weapon>                         m_pWeapon;

	//モデル
	std::unique_ptr<DirectX::Model>                 m_pModel;

	//影のモデル
	std::unique_ptr<DirectX::Model>                 m_shadow;

	//HPゲージ
	std::unique_ptr <HPGuage>                       m_pHPGauge;
	//攻撃中か判定
	bool                                            m_attack;
	//移動しているか判定
	bool                                            m_moveCheck;

	//攻撃キーを入力
	bool                                            m_attackKeyDown;

	//回避に関係するデータ
	AvoidanceData                                   m_avoidance;

	//1フレーム前のコンボ
	int                                             m_oldCombo;

	//SEの再生判定
	bool                                            m_playSE;

	//無敵時間時のフレーム計算
	int                                             m_invincibleFrameCount;

	//吹っ飛ぶ方向の角度(行列)
	DirectX::SimpleMath::Matrix                     m_blowAwayRotation;
	//吹っ飛ぶ力(1秒間)
	float                                           m_blowAwayPower;
	//吹っ飛び時間の計測
	float                                           m_blowAwayTime;

	//1フレームにかかった時間(合計すると1秒になる)
	float                                           m_elapsedTime;

	//加速度
	DirectX::SimpleMath::Vector3                    m_acceleration;
	//速度
	DirectX::SimpleMath::Vector3                    m_velocity;
	//キー入力した方向のベクトル
	DirectX::SimpleMath::Vector3                    m_inputKeyVec;

	//std::unique_ptr<DebugCapsule>                 m_pDebugCapsulePlayer;
};