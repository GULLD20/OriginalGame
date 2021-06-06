#include "../../pch.h"

#include "../../DeviceResources.h"

#include "Enemy.h"

#include "../../DataManager/DataManager.h"
#include "../List/CharacterMapList.h"

#include "../../Map/Map.h"

//敵の大きさ上と下
const DirectX::SimpleMath::Vector3 Enemy::ENEMYSIZE_TOP_DOWN = DirectX::SimpleMath::Vector3(0.0f,0.5f,0.0f);

//吹っ飛ばされる時間
const float Enemy::BLOW_AWAY_TIME = 0.25f;

//無敵時間
const float Enemy::INVINCIBLE_TIME = 4.0f;

//スロウ状態の計算用の数値
const float Enemy::SLOW_CAL = 0.025f;
//通常状態の計算用の数値
const float Enemy::NORMAL_CAL = 1.0f;

//スロウ時間
const float Enemy::SLOW_TIME = 3.0f;

Enemy::Enemy(const DirectX::SimpleMath::Vector3 &pos, float attack, float hp, float speed, float thisRange)
	:Character(GetNextID(),pos, attack, hp, speed, thisRange)
	,m_pModel(nullptr)
{
}


Enemy::~Enemy()
{
	if (m_pModel != nullptr)
	{
		m_pModel.reset(nullptr);
	}

	if (m_pHPGuage != nullptr)
	{
		m_pHPGuage->Reset();
		m_pHPGuage.reset(nullptr);
	}

	if (m_pName != nullptr)
	{
		m_pName->Reset();
		m_pName.reset(nullptr);
	}

	if (m_pEnemyHit != nullptr)
	{
		m_pEnemyHit->Lost();
		m_pEnemyHit.reset(nullptr);
	}

	if (m_pEnemyDeth != nullptr)
	{
		m_pEnemyDeth->Lost();
		m_pEnemyDeth.reset(nullptr);
	}

	if (m_pOtherEnemyList.size() > 0)
	{
		for (int i = 0; i < static_cast<int>(m_pOtherEnemyList.size()); i++)
		{
			delete m_pOtherEnemyList[i];
			m_pOtherEnemyList[i] = nullptr;
		}

		m_pOtherEnemyList.clear();
	}
}

void Enemy::CreateModel(const wchar_t* fileName)
{
	DX::DeviceResources* deviceResources = GameContext<DX::DeviceResources>::Get();

	// エフェクトファクトリの作成
	DirectX::EffectFactory* factory = new DirectX::EffectFactory(deviceResources->GetD3DDevice());
	// テクスチャの読み込みパス指定
	factory->SetDirectory(L"Resources/CMO");

	//キャラクターのモデルの読み込み
	m_pModel = DirectX::Model::CreateFromCMO(
		deviceResources->GetD3DDevice(),
		fileName,
		*factory
	);

	//影のモデル
	m_shadow = DirectX::Model::CreateFromCMO(
		deviceResources->GetD3DDevice(),
		L"Resources/CMO/Shadow.cmo",
		*factory
	);

	delete factory;
}

void Enemy::Initialize()
{
	//目的地をしていする変数
	m_wanderingAssignment = 0;

	//スピード計算用
	m_speedCal = NORMAL_CAL;
	//生存判定
	m_alive = true;
	//プレイヤーを発見したか判定
	m_discovery = false;
	//移動状態
	m_walk = false;

	//ラープ処理を調整するための変数
	m_lerpAdjustment = 0.0f;

	//
	m_withinRange = false;

	//描画範囲内か判定
	m_checkCamera = false;

	//1回の攻撃に使うフレームの確認
	m_attackTime = 0.0f;

	//攻撃中かの判定
	m_attack_start_end = false;

	//攻撃間隔
	m_attackInterval = 0.0f;
	//1フレームにかかった時間
	m_elapsedTime = 0.0f;

	//1フレーム前のなす角
	m_oldAco = 0.0f;

	//無敵状態
	m_invincible = false;
	//無敵時間
	m_invincibleTime = 0.0f;
	//1回の攻撃で敵に1回だけダメージ処理をさせるため
	m_hitAttack = false;

	m_justAvoidance = false;
	m_justAvoidanceTime = 0.0f;
	//m_justAvoidanceFlameControl = 0;

	//プレイヤーのIDの初期化
	m_playerID = 0;

	//攻撃が当たった時のエフェクトのアルファ値
	m_hitEffectAlpha = 0.0f;

	//死亡エフェクトのアルファ値
	m_dethEffectAlpha = 1.0f;

	//タグの設定
	m_tag = ENEMY_TAG;

	//プレイヤーの攻撃のコンボが最後か
	m_playerAttackCombLast = false;

	//エネミーの吹っ飛び判定の初期化
	m_blowAway = true;

	//吹っ飛ぶ方向の角度(行列)
	m_blowAwayRotation = DirectX::SimpleMath::Matrix::Identity;
	//吹っ飛ぶ力(1秒間)
	m_blowAwayPower = 0.0f;
	//吹っ飛び時間の計測
	m_blowAwayTime = 0.0f;

	//派生クラスの初期化処理
	SubInitialize();
}

void Enemy::Update(const DX::StepTimer &timer)
{
	//1フレームにかかった時間の設定
	m_elapsedTime = static_cast<float>(timer.GetElapsedSeconds());

	//プレイヤーのジャスト回避時間の更新処理
	JustAvoidanceUpdate();

	//Stateに合わせた更新処理
	StateUpdate();

	//派生クラスの更新処理
	SubUpdate();

	//無敵時間の更新処理
	InvincibleUpdate();

	//プレイヤーのジャスト回避判定
	PlayerJustAvoidance();

	//停止処理
	Stop();

	MatrixSetting();
	//エフェクトの更新処理
	EffectUpdate(timer);

	//Mapとの当たり判定
	HitMap();

	//データの更新処理
	DataSetting();

	//自分の当たり判定を設定
	m_charaData.thisCapsule =
	{
		m_charaData.pos + ENEMYSIZE_TOP_DOWN,
		m_charaData.pos - ENEMYSIZE_TOP_DOWN,
		GetThisRange(),
	};
}

void Enemy::Draw(const DirectX::SimpleMath::Matrix &viewMatrix)
{
	//描画範囲内
	if (m_checkCamera)
	{
		//プロジェクション行列の取得
		Projection* projection = GameContext<Projection>().Get();

		//死亡判定
		//生きているなら
		if (m_alive)
		{
			AliveDraw(viewMatrix, projection->GetMatrix());
		}
		//死んでいるなら
		if (!m_alive)
		{
			DethDraw(viewMatrix, projection->GetMatrix());

		}
	}
}

void Enemy::Reset()
{

}

//ダメージ処理
void Enemy::Damage(float attack, const DirectX::SimpleMath::Matrix &rotation, bool blowAway, float power)
{
	//無敵状態の時はダメージ処理をしない
	if (m_invincible) return;

	//体力を攻撃力分減らす
	m_charaData.hp -= attack;

	//吹っ飛び処理に必要なデータを設定
	m_blowAwayRotation = rotation;
	m_blowAwayPower = power;

	//発見状態
	m_discovery = true;

	//trueの時は最後のコンボ
	m_playerAttackCombLast = blowAway;

	//最後のコンボの時に吹っ飛び状態にする
	if (m_playerAttackCombLast)
	{
		m_charaData.state = State::BlowAway;
	}

	//エフェクトの透明度
	m_hitEffectAlpha = 1.0f;
	m_pEnemyHit->SetMainPos(m_charaData.pos);
	//プレイヤーに向けたベクトルを設定
	DirectX::SimpleMath::Vector3 vec = GameContext<CharacterMapList>::Get()->GetEntityFromID(m_playerID)->GetPos() - m_charaData.pos;
	vec.Normalize();
	m_pEnemyHit->SetVector(vec);
}





//各Update関数-----------------
//State関連のメインのUpdate
void Enemy::StateMainUpdate()
{
	switch (m_charaData.state)
	{
	case State::Idle:
		break;

	case State::Move:
		//正面に移動処理
		Move();
		break;

	case State::Battle:
		Battle();
		break;

	case State::Attack:
		//攻撃処理
		Attack();
		break;

	case State::BlowAway:
		//ぶっ飛び処理
		BlowAwayMove();
	default:
		break;
	}
}

//State関連の更新処理
void Enemy::StateUpdate()
{
	if (!m_alive) return;

	if (m_charaData.state != State::Battle &&
		m_charaData.state != State::Attack)
	{
		//状態の設定
		SelectState();

		//ターゲットの位置の設定
		TargetosSet();

		//回転処理
		Rotation(m_angle);
	}

	StateMainUpdate();
}

//プレイヤーのジャスト回避時間の更新処理
void Enemy::JustAvoidanceUpdate()
{
	if (!m_alive) return;

	//ジャスト回避中
	if (m_justAvoidance)
	{
		m_justAvoidanceTime += m_elapsedTime;

		//3秒たったら
		if (m_justAvoidanceTime >= SLOW_TIME)
		{
			//通常状態に戻す
			m_justAvoidance = false;
			m_justAvoidanceTime = 0.0f;
			m_speedCal = NORMAL_CAL;
		}
	}
}

//無敵時間の更新処理
void Enemy::InvincibleUpdate()
{
	if (!m_alive) return;

	if (m_invincible)
	{
		//無敵時間の判定
		if (m_invincibleTime >= INVINCIBLE_TIME)
		{
			m_invincible = false;
			m_invincibleTime = 0.0f;
		}
		else
		{
			m_invincibleTime += m_elapsedTime;
		}
	}
}

//Mapとの当たり判定
void Enemy::HitMap()
{
	if (!m_alive) return;

	//エネミーの当たり判定の設定
	Collision::Box3D enemy =
	{
		m_charaData.pos,
		DirectX::SimpleMath::Vector3(0.5f,1.0f,0.5f),
	};

	Map* map = GameContext<Map>::Get();
	//壁との当たり判定
	m_charaData.pos = map->HitWallBox(enemy);
	//移動限界
	m_charaData.pos = map->Limit(m_charaData.pos, m_charaData.thisRange);
}

//死亡時の更新処理
bool Enemy::DethUpdate(DX::StepTimer timer)
{
	//死亡状態なら
	if (!m_alive)
	{
		//見えるなら
		if (m_dethEffectAlpha > 0.0f)
		{
			//透明度の調整
			m_dethEffectAlpha -= m_elapsedTime;
			//透明度を渡す
			m_pEnemyDeth->SetAlpha(m_dethEffectAlpha);
			//死亡エフェクトの更新処理
			m_pEnemyDeth->Update(timer);
		}
		return true;
	}

	return false;
}
//-----------------------------

//各描画処理-------------------

//死亡時の描画処理
void Enemy::DethDraw(const DirectX::SimpleMath::Matrix &viewMatrix, const DirectX::SimpleMath::Matrix &projection)
{
	//見えないなら
	if (m_dethEffectAlpha < 0)
	{
		return;
	}

	m_pEnemyDeth->SetRenderState(m_eyePos, viewMatrix, projection);
	m_pEnemyDeth->Render();
}
//-----------------------------

//回転処理
void Enemy::Rotation(float angle)
{
	//エネミーからターゲットに向けたベクトル
	DirectX::SimpleMath::Vector3 enemy_target_vec = m_targetPos - m_charaData.pos;

	//エネミーの正面に向けたベクトル
	DirectX::SimpleMath::Vector3 frontVec = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Forward, m_charaData.rotation);

	//正規化
	enemy_target_vec.Normalize();
	frontVec.Normalize();

	//外積
	float cross = enemy_target_vec.x * frontVec.z - enemy_target_vec.z * frontVec.x;

	//回転角度
	//ONE_DEGREE * angleで1秒間に回転する値を求める
	//m_elapsedTimeをかけて回転速度の調整(1秒に合わせるために)
	//m_speedCalはスロウ状態の時と通常状態の時の速度を変えるため
	float rot = (((ONE_DEGREE * angle)*m_speedCal)* m_elapsedTime);

	//左右どちら側にいるかで回転する方向を指定
	if (cross < 0)
	{
		m_charaData.angle.y -= rot;
	}
	else if (cross > 0)
	{
		m_charaData.angle.y += rot;
	}
}

//正面に移動処理
void Enemy::Move()
{
	//移動速度を求める
	//エネミーの正面に向かって移動
	DirectX::SimpleMath::Vector3 moveVec =
		DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Forward, m_charaData.rotation)
		*m_charaData.speed;

	//m_speedCalはスロウ状態の時と通常状態の時の速度を変えるため
	m_charaData.pos += ((moveVec * m_speedCal)* m_elapsedTime);
}

//Matrixの設定
void Enemy::MatrixSetting()
{
	m_charaData.matrix = DirectX::SimpleMath::Matrix::CreateRotationY(m_charaData.angle.y);
	m_charaData.matrix *= DirectX::SimpleMath::Matrix::CreateRotationX(m_charaData.angle.x);
	m_charaData.matrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(m_charaData.angle.z);

	m_charaData.rotation = DirectX::SimpleMath::Matrix::CreateRotationY(m_charaData.angle.y);
	m_charaData.rotation *= DirectX::SimpleMath::Matrix::CreateRotationX(m_charaData.angle.x);
	m_charaData.rotation *= DirectX::SimpleMath::Matrix::CreateRotationZ(m_charaData.angle.z);

	m_charaData.matrix *= DirectX::SimpleMath::Matrix::CreateTranslation(m_charaData.pos);
}

//ターゲットの位置の設定
void Enemy::TargetosSet()
{
	m_targetPos = GameContext<CharacterMapList>::Get()->GetEntityFromID(m_playerID)->GetPos();
}

//ステートの設定
void Enemy::SelectState()
{
	if (m_charaData.state == State::BlowAway)
	{
		return;
	}

	//ステートの初期化
	m_charaData.state = State::Idle;

	//移動判定
	SelectMove();
	//戦闘開始判定
	SelectBattle();
	//攻撃開始判定
	SelectAttack();
	//停止判定
	SelectStop();
}

//データを設定する
void Enemy::DataSetting()
{
	SetPos(m_charaData.pos);
	SetHP(m_charaData.hp);
}

//プレイヤーのIDを設定
void Enemy::SetPlayerID(int id)
{
	m_playerID = id;
}

//自分以外のEnemyを設定
void Enemy::SetOtherEnemy(const std::vector<std::unique_ptr<Enemy>> &enemyList, int enemyNum)
{
	//-1するのは自分を含めないため
	m_pOtherEnemyList.resize(enemyNum-1);

	int j = 0;
	for (int i = 0; i < enemyNum;i++)
	{
		//自分以外の敵を格納)
		if (enemyList[i]->GetID() != GetID())
		{
			m_pOtherEnemyList[j] = enemyList[i].get();
			j++;
		}
	}
}

//範囲にいるか(カメラ描画範囲)
void Enemy::CameraWithinRange()
{
	Collision::Box3D player
	{
		GameContext<CharacterMapList>::Get()->GetEntityFromID(m_playerID)->GetPos(),
		//描画範囲の設定
		DirectX::SimpleMath::Vector3(60.0f,60.0f,60.0f),
	};

	Collision::Box3D enemy
	{
		m_charaData.pos,
		DirectX::SimpleMath::Vector3::One,
	};

	if (Collision::HitCheckoxBox3D(player,enemy))
	{
		m_checkCamera = true;
	}
	else
	{
		m_checkCamera = true;
	}
}

//死亡判定
bool Enemy::IsDeth()
{
	//HPが0になった時
	if (m_charaData.hp <= 0.0f)
	{
		//死亡状態に変更
		m_alive = false;
		m_charaData.state = State::Deth;
	}

	//死亡状態の処理
	if (!m_alive)
	{
		//死亡エフェクトのデータを設定
		//位置を設定
		m_pEnemyDeth->SetMainPos(m_charaData.pos);
	}

	//死亡エフェクトが終了したら
	if (m_dethEffectAlpha <= 0.0f)
	{

		return true;
	}

	return false;
}

//カメラの位置の取得
void Enemy::SetEysPos(const DirectX::SimpleMath::Vector3 &pos)
{
	m_eyePos = pos;
}

//HPの画像サイズの設定
void Enemy::HPTextureSize()
{
	//最大のHPの１％の値を求める
	float hpOnePercent = m_charaData.maxHp * 0.01f;

	//現在のHPが何％残っているか求める
	float hpRemainingGuage = m_charaData.hp / hpOnePercent;

	//画像の横の大きさの１％を求める
	float textuerSizeOnePercent = 2.0f * 0.01f;

	//求めたtextuerSizeOnePercentとRemainingGuageをかけた画像をどれくらい表示するか決める(X)
	float sizeX = textuerSizeOnePercent * hpRemainingGuage;

	m_pHPGuage->SetTextureScaleTL(DirectX::SimpleMath::Vector2(1.0f, 0.06f));
	m_pHPGuage->SetTextureScaleBR(DirectX::SimpleMath::Vector2(1.0f - sizeX, -0.06f));
}

void Enemy::BlowAwayMove()
{
	//ノックバック(powerの数値分1秒換算で進む)
	DirectX::SimpleMath::Vector3 vec = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Forward, m_blowAwayRotation) * m_blowAwayPower;

	if (m_playerAttackCombLast&& m_blowAway)
	{
		//移動処理
		m_charaData.pos += (vec*m_elapsedTime);
		//無敵状態
		m_invincible = true;
	}

	//吹っ飛び時間終了
	if (m_blowAwayTime >= BLOW_AWAY_TIME)
	{
		m_charaData.state = State::Idle;
		m_blowAwayRotation = DirectX::SimpleMath::Matrix::Identity;
		m_blowAwayPower = 0.0f;
		m_blowAwayTime = 0.0f;
		m_playerAttackCombLast = false;
	}
	else
	{
		m_blowAwayTime += m_elapsedTime;
	}
}

//スロウ状態の開始
void Enemy::StartSlow()
{
	//スロウ状態に変更
	m_justAvoidance = true;
	m_justAvoidanceTime = 0.0f;
	m_speedCal = SLOW_CAL;
}