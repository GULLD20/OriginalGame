#include "../../../../pch.h"
#include "Sword.h"
#include "../../DataManager/DataManager.h"

//剣のサイズ
const  DirectX::SimpleMath::Vector3 Sword::SIZE = DirectX::SimpleMath::Vector3(0.2f, 1.5f, 0.2f);

//剣の基本位置(移動中)
const DirectX::SimpleMath::Vector3 Sword::BASE_POSITION = DirectX::SimpleMath::Vector3(0.7f, 1.0f, -0.5f);

//剣の基本位置(攻撃中)
const DirectX::SimpleMath::Vector3 Sword::BASE_POSITION_ATTACK = DirectX::SimpleMath::Vector3(0.7f, 0.0f, -0.5f);

//コンボの数
const int Sword::MAX_COMBO = 4;
//攻撃時間
const float Sword::ATTACK_FLAME =10.0f;

//武器の回転(コンボ)
const std::vector<DirectX::SimpleMath::Vector3> Sword::ANGLE =
{

	//剣
	DirectX::SimpleMath::Vector3(0.0f,(DirectX::XM_PI* 0.5f),-DirectX::XM_PI* 0.3f),

	DirectX::SimpleMath::Vector3(-DirectX::XM_PI,(DirectX::XM_PI* 0.5f),-DirectX::XM_PI* 0.5f),

	DirectX::SimpleMath::Vector3(0.0f,(DirectX::XM_PI* 0.5f),-(DirectX::XM_PI* 0.5f)) ,

	DirectX::SimpleMath::Vector3(-DirectX::XM_PI* 3.0f,(DirectX::XM_PI* 0.5f),(-DirectX::XM_PI* 0.5f))
};

Sword::Sword()
{
}


Sword::~Sword()
{
	if (m_pSlashEffect != nullptr)
	{
		m_pSlashEffect.reset(nullptr);
	}
}

void Sword::Create(DX::DeviceResources* &DeviceResources)
{
	// エフェクトファクトリの作成
	DirectX::EffectFactory * factory = new DirectX::EffectFactory(DeviceResources->GetD3DDevice());
	// テクスチャの読み込みパス指定
	factory->SetDirectory(L"Resources/CMO");

	// ファイルを指定してモデルデータ読み込み
	m_pModel = DirectX::Model::CreateFromCMO(
		DeviceResources->GetD3DDevice(),
		L"Resources/CMO/testSword.cmo",
		*factory
	);

	delete factory;

	m_pSlashEffect = std::make_unique<SlashEffect>();
	m_pSlashEffect->Creat();
}

//初期化処理
void Sword::Initialize()
{
	m_weapon.attack = false;
	m_weapon.combo = 0;
	m_weapon.angle = ANGLE[m_weapon.combo];
	m_weapon.flame = 0.0f;
	m_weapon.vector = DirectX::SimpleMath::Vector3::Zero;
	m_weapon.betweenAttacksTime = 0.0f;
	m_weapon.afterTheAttack = false;

	m_pSlashEffect->Initialzie();
}

//更新処理
void Sword::Update(float elapsedSeconds)
{
	//攻撃処理(回転角度)
	if (m_weapon.attack)
	{
		//攻撃モーション処理
		Attack();

		//攻撃後の状態
		if (!m_weapon.attack)
		{
			m_weapon.afterTheAttack = true;
		}
	}
	//攻撃後
	if (m_weapon.afterTheAttack)
	{
		//攻撃後の一定時間後(現在は0.5秒)
		if (m_weapon.betweenAttacksTime >= BETWEENATTACKSTIME)
		{
			ResetWeaponData();
		}
		m_weapon.betweenAttacksTime += elapsedSeconds;
	}

	m_pSlashEffect->SetWeaponMatrix(m_weapon.matrix);
	m_pSlashEffect->Update();
}

//描画処理
void Sword::Draw(ID3D11DeviceContext1* context, const DirectX::CommonStates& State, const DirectX::SimpleMath::Matrix &viewMatrix, const DirectX::SimpleMath::Matrix &proj)
{
	// モデル描画
	m_pModel->Draw(context, State, m_weapon.matrix, viewMatrix, proj);

	if (m_weapon.attack)
	{
		//エフェクトの描画
		m_pSlashEffect->Draw(viewMatrix);
	}
}

//リセット
void Sword::Reset()
{
}

//攻撃開始
void Sword::StartAttack()
{
	if (m_weapon.afterTheAttack)
	{
		m_weapon.combo++;

		if (m_weapon.combo >= MAX_COMBO)
		{
			m_weapon.combo = 0;
		}
	}

	m_weapon.attack = true;
	m_weapon.afterTheAttack = false;
}

//攻撃終了
void Sword::EndAttack()
{
	//攻撃モーションが終了した時
	if (m_weapon.flame >= 1.0f)
	{
		//攻撃終了処理
		m_weapon.flame = 0.0f;
		m_weapon.attack = false;
		m_weapon.afterTheAttack = true;
		m_weapon.betweenAttacksTime = 0.0f;
	}
}

//攻撃中か取得
bool Sword::GetAttackSTate() const
{
	return m_weapon.attack;
}

//Matrixを設定する
void Sword::MatrixSetting()
{
	m_weapon.matrix = DirectX::SimpleMath::Matrix::CreateRotationY(ANGLE[m_weapon.combo].y);

	//武器の位置の調整
	m_weapon.matrix *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 2.0f, 0.0f));

	//回転処理
	m_weapon.matrix *= DirectX::SimpleMath::Matrix::CreateRotationX(m_weapon.angle.x);
	m_weapon.matrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(ANGLE[m_weapon.combo].z);

	//ベクトルを出す
	DirectX::SimpleMath::Matrix vecMatrix;

	vecMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(ANGLE[m_weapon.combo].y);
	vecMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(m_weapon.angle.x);
	vecMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(ANGLE[m_weapon.combo].z);

	m_weapon.vector= DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f), vecMatrix);


	//プレイヤーの位置に調整
	m_weapon.matrix *= m_playerMatrix;
}

void Sword::ResetWeaponData()
{
	m_weapon.combo = 0;
	m_weapon.angle = ANGLE[m_weapon.combo];
	m_weapon.flame = 0.0f;
	m_weapon.attack = false;
	m_weapon.betweenAttacksTime = 0.0f;
	m_weapon.afterTheAttack = false;
}

//攻撃処理(回転)
void Sword::Attack()
{
	//最大コンボ数より大きい場合コンボを最初に戻す
	if (m_weapon.combo + 1 == MAX_COMBO)
	{
		m_weapon.angle = DirectX::SimpleMath::Vector3::Lerp(ANGLE[m_weapon.combo], ANGLE[0], m_weapon.flame);
	}
	else
	{
		m_weapon.angle = DirectX::SimpleMath::Vector3::Lerp(ANGLE[m_weapon.combo], ANGLE[m_weapon.combo + 1], m_weapon.flame);
	}

	if (m_weapon.combo <= 1)
	{
		m_weapon.flame += 1.0f / ATTACK_FLAME;
	}
	else
	{
		m_weapon.flame += 1.0f / (ATTACK_FLAME * 2.0f);
	}

	EndAttack();
}