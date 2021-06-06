#include "../../../pch.h"

#include "LongRangeAttack.h"

#include <GeometricPrimitive.h>
#include <Effects.h>

#include "../../../DeviceResources.h"
#include "../../../Scene/GameContext.h"

//当たり判定の中心から先端の長さ(Vector3)
const DirectX::SimpleMath::Vector3 LongRangeAttack::BASE_CENTER_TOP = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.4f);
//当たり判定の円の大きさ
const float LongRangeAttack::CIRCLE_SIZE = 0.2f;

//コンスト・デスト
LongRangeAttack::LongRangeAttack()
	:m_pModel(nullptr)
	, m_speed(0.0f)
	, m_attackRange(0)
	, m_speedCal(0.0f)
{

}

LongRangeAttack::~LongRangeAttack()
{
	if (m_pModel != nullptr)
	{
		m_pModel.reset(nullptr);
	}

	//要素があったら
	if (m_bulletList.size() > 0)
	{
		m_bulletList.clear();
	}
}

//モデルの作成
void LongRangeAttack::CreatModel()
{
	DX::DeviceResources* deviceResources = GameContext<DX::DeviceResources>::Get();

	// エフェクトファクトリの作成
	DirectX::EffectFactory* factory = new DirectX::EffectFactory(deviceResources->GetD3DDevice());
	// テクスチャの読み込みパス指定
	factory->SetDirectory(L"Resources/CMO");

	// ファイルを指定してモデルデータ読み込み
	m_pModel = DirectX::Model::CreateFromCMO(
		deviceResources->GetD3DDevice(),
		L"Resources/CMO/LongRangeAttack.cmo",
		*factory
	);

	delete factory;
}

//初期化処理
void LongRangeAttack::Initialize(float speed,int attackRange)
{
	//攻撃範囲の指定
	m_attackRange = attackRange;
	//速度の設定
	m_speed = speed;
	//データの確保
	m_bulletList.resize(100);
	//初期化
	for (int i = 0; i < static_cast<int>(m_bulletList.size()); i++)
	{
		m_bulletList[i].active = false;
		m_bulletList[i].pos = DirectX::SimpleMath::Vector3::Zero;
		m_bulletList[i].startPos = DirectX::SimpleMath::Vector3::Zero;
		m_bulletList[i].capsule3D =
		{
			BASE_CENTER_TOP,
			-BASE_CENTER_TOP,
			CIRCLE_SIZE ,
		};
	}

	//速度計算用変数の初期化
	m_speedCal = 0.0f;

	//返したカプセルデータ(当たり判定のデータ)の要素位置の初期化
	m_capsuleSelect = 0;
}

//更新処理
void LongRangeAttack::Update(float elapsedTime)
{
	m_capsuleSelect = -1;

	for (int i = 0; i < static_cast<int>(m_bulletList.size()); i++)
	{
		if (m_bulletList[i].active)
		{
			//移動処理
			Move(i, elapsedTime);
			//攻撃終了判定
			AttackEnd(i);
			//当たり判定の設定
			CapsuleDataSetting(i);
			//ワールド行列の設定
			MatrixSetting(i);
		}
	}
}

//描画処理
void LongRangeAttack::Draw(const DirectX::SimpleMath::Matrix &viewMatrix, const DirectX::SimpleMath::Matrix &projection)
{
	//描画に使うデータを取得
	DX::DeviceResources* deviceResources = GameContext<DX::DeviceResources>::Get();
	ID3D11DeviceContext1* deviceContext = deviceResources->GetD3DDeviceContext();
	DirectX::CommonStates* CommonState = GameContext<DirectX::CommonStates>().Get();

	for (int i = 0; i < static_cast<int>(m_bulletList.size()); i++)
	{
		if (m_bulletList[i].active)
		{
			//モデルの描画
			m_pModel->Draw(deviceContext, *CommonState, m_bulletList[i].matrix, viewMatrix, projection);
		}
	}
}

//リセット
void LongRangeAttack::Reset()
{

}

//攻撃開始処理
void LongRangeAttack::Shot(const DirectX::SimpleMath::Vector3 &pos, const DirectX::SimpleMath::Matrix &rotation)
{
	for (int i = 0; i < static_cast<int>(m_bulletList.size()); i++)
	{
		if (!m_bulletList[i].active)
		{
			//使っている状態に変更
			m_bulletList[i].active = true;
			//初期位置の指定
			m_bulletList[i].pos = pos;
			m_bulletList[i].startPos= pos;
			//角度の指定
			m_bulletList[i].rotation = rotation;
			break;
		}
	}
}

//攻撃が当たったなら
void LongRangeAttack::HitAttack()
{
	//攻撃終了処理
	m_bulletList[m_capsuleSelect].active = false;
}

//当たり判定のGet
Collision::Capsule3D LongRangeAttack::GetCapsuleData(const DirectX::SimpleMath::Vector3 &playerPos)
{
	//returnで返すデータを指定する
	int select = -1;

	//ベクトルの長さ
	//毎回計算して出た長さ
	float lengthNow = 100.0f;
	//最も短い長さ
	float lengthRet = 100.0f;

	for (int i = 0; i < static_cast<int>(m_bulletList.size()); i++)
	{
		//使っているなら
		if (m_bulletList[i].active)
		{
			//弾からプレイヤーに向けたベクトルの長さ
			lengthNow = (playerPos - m_bulletList[i].pos).Length();

			//プレイヤーに一番近いデータを残す
			if (lengthRet > lengthNow)
			{
				lengthRet = lengthNow;
				//returnで返すデータを指定
				select = i;
			}
		}
	}

	//何も指定できなかったら
	if (select == -1)
	{
		return Collision::Capsule3D
		{
			DirectX::SimpleMath::Vector3(0.0f,100.0f,0.0f),
			DirectX::SimpleMath::Vector3(0.0f,100.0f,0.0f),
		    0.0f,
		};
	}

	//渡したデータの要素位置を保存
	m_capsuleSelect = select;

	//データを指定してデータを返す
	return m_bulletList[select].capsule3D;
}

//移動処理
void LongRangeAttack::Move(int select ,float elapsedTime)
{
	//移動ベクトル
	DirectX::SimpleMath::Vector3 vec;
	//移動ベクトルの計算
	vec = DirectX::SimpleMath::Vector3::Transform(
		DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f),
		m_bulletList[select].rotation)
		*m_speed
		*m_speedCal;

	//移動処理
	m_bulletList[select].pos += (vec * elapsedTime);
}

//攻撃終了処理
void LongRangeAttack::AttackEnd(int select)
{
	//弾の発射位置から現在の位置までのベクトルの長さ
	float length = (m_bulletList[select].pos - m_bulletList[select].startPos).Length();

	//弾の飛距離が限界を超えたら
	if (length >= m_attackRange)
	{
		//攻撃終了処理
		m_bulletList[select].active = false;
	}
}

//当たり判定の回転
void LongRangeAttack::CapsuleDataSetting(int select)
{
	//当たり判定の大きさを設定
	DirectX::SimpleMath::Vector3 center_top =
		DirectX::SimpleMath::Vector3::Transform(BASE_CENTER_TOP, m_bulletList[select].rotation);

	m_bulletList[select].capsule3D =
	{
		m_bulletList[select].pos + center_top,
		m_bulletList[select].pos - center_top,
		CIRCLE_SIZE,
	};
}

//行列の設定
void LongRangeAttack::MatrixSetting(int select)
{
	//角度の設定
	m_bulletList[select].matrix = m_bulletList[select].rotation;
	//位置の設定
	m_bulletList[select].matrix *= DirectX::SimpleMath::Matrix::CreateTranslation(m_bulletList[select].pos);
}