#include "../pch.h"
#include "Map.h"

#include "../MyLib/MyLib.h"

#include "../Scene/GameContext.h"

//壁の当たりは判定の大きさ
const  DirectX::SimpleMath::Vector3 Map::WALL_BOX_SIZE = DirectX::SimpleMath::Vector3(1.5f, 15.0f, 1.5f);

//移動限界時のエフェクトの数
const int Map::LIMIT_EFFECT_NUM = 2;

//エフェクトの角度(*0.49はエフェクト表示時に0.5だと完全に見えなくなるため少しずらす)
//右
const float Map::EFFECT_ANGLE_R = DirectX::XM_PI*0.49f;
//左
const float Map::EFFECT_ANGLE_L = DirectX::XM_PI*-0.49f;

Map::Map()
{
}


Map::~Map()
{
	if (m_pGroundModel != nullptr)
	{
		m_pGroundModel.reset(nullptr);
	}

	if (m_pWallModel != nullptr)
	{
		m_pWallModel.reset();
	}

	if (m_pSpace != nullptr)
	{
		m_pSpace.reset();
	}

	if (m_MapDataList.size() != 0)
	{
		m_MapDataList.clear();
	}

	if (m_wallBoxList.size() != 0)
	{
		m_wallBoxList.clear();
	}

	if (m_pHitWallList.size() != 0)
	{
		for (int i = 0; i < static_cast<int>(m_pHitWallList.size()); i++)
		{
			m_pHitWallList[i]->Lost();
		}

		m_pHitWallList.clear();
	}
}

//作成
void Map::Create()
{
	DX::DeviceResources* deviceResources = GameContext<DX::DeviceResources>::Get();

	// エフェクトファクトリの作成
	DirectX::EffectFactory * factory = new DirectX::EffectFactory(deviceResources->GetD3DDevice());
	// テクスチャの読み込みパス指定
	factory->SetDirectory(L"Resources/CMO");

	// ファイルを指定してモデルデータ読み込み
	//地面
	m_pGroundModel = DirectX::Model::CreateFromCMO(
		deviceResources->GetD3DDevice(),
		L"Resources/CMO/mapPart1.cmo",
		*factory
	);

	//壁
	m_pWallModel = DirectX::Model::CreateFromCMO(
		deviceResources->GetD3DDevice(),
		L"Resources/CMO/mapPart2.cmo",
		*factory
	);

	//天球
	m_pSpace = DirectX::Model::CreateFromCMO(
		deviceResources->GetD3DDevice(),
		L"Resources/CMO/space.cmo",
		*factory
	);

	delete factory;

	//壁に当たった時のエフェクトの作成
	for (int i = 0; i < LIMIT_EFFECT_NUM; i++)
	{
		m_pHitWallList.push_back(std::make_unique<HitWall>());
		m_pHitWallList[i]->Create(deviceResources, L"Resources\\Textures\\LilimitWall.png", 1);
		m_pHitWallList[i]->InitializeNormal(0.0f, DirectX::SimpleMath::Vector3::Zero);
		m_pHitWallList[i]->SetScale(DirectX::SimpleMath::Vector3(2.0f, 2.0f, 2.0f));
	}
}

//初期化処理
void Map::Initialize()
{
	//地面の作成
	CreatGround();
	//壁の作成
	CreatWall();

	//天球の大きさの設定
	m_spaceWorld = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(3.0f, 3.0f, 3.0f));
	//天球の回転角度の初期化
	m_spaceAngleY = 0.0f;
}

//描画処理
void Map::Draw(const DirectX::SimpleMath::Matrix &viewMatrix)
{
	DX::DeviceResources* deviceResources = GameContext<DX::DeviceResources>::Get();
	ID3D11DeviceContext1* deviceContext = deviceResources->GetD3DDeviceContext();
	DirectX::CommonStates* CommonState = GameContext<DirectX::CommonStates>().Get();
	Projection* projection = GameContext<Projection>().Get();

	// モデル描画
	//天球の回転
	m_spaceAngleY += 0.01f;
	m_spaceWorld = DirectX::SimpleMath::Matrix::CreateRotationY(m_spaceAngleY);
	m_pSpace->Draw(deviceContext, *CommonState, m_spaceWorld, viewMatrix, projection->GetMatrix());

	//地面の描画
	m_pGroundModel->Draw(deviceContext, *CommonState, m_groundWorld, viewMatrix, projection->GetMatrix());

	//壁描画用のワールド行列
	DirectX::SimpleMath::Matrix world;

	//壁の描画
	for (int i = 0; i < static_cast<int>(m_wallBoxList.size()); i++)
	{
		//壁の位置にワールド行列を調整
		world = DirectX::SimpleMath::Matrix::CreateTranslation(m_wallBoxList[i].center);
		m_pWallModel->Draw(deviceContext, *CommonState, world, viewMatrix, projection->GetMatrix());
	}


	//移動限界のエフェクトの表示
	for (int i = 0; i < LIMIT_EFFECT_NUM; i++)
	{
		m_pHitWallList[i]->SetRenderState(DirectX::SimpleMath::Vector3::One, viewMatrix, projection->GetMatrix());
		m_pHitWallList[i]->Render();
	}
}

//タイトル画面でのマップの表示
void Map::TitleDraw(const DirectX::SimpleMath::Matrix &viewMatrix)
{
	DX::DeviceResources* deviceResources = GameContext<DX::DeviceResources>::Get();
	ID3D11DeviceContext1* deviceContext = deviceResources->GetD3DDeviceContext();
	DirectX::CommonStates* CommonState = GameContext<DirectX::CommonStates>().Get();
	Projection* projection = GameContext<Projection>().Get();

	// モデル描画
	//天球の回転
	m_spaceAngleY += 0.01f;
	m_spaceWorld = DirectX::SimpleMath::Matrix::CreateRotationY(m_spaceAngleY);
	m_pSpace->Draw(deviceContext, *CommonState, m_spaceWorld, viewMatrix, projection->GetMatrix());

	//地面の描画
	m_pGroundModel->Draw(deviceContext, *CommonState, m_groundWorld, viewMatrix, projection->GetMatrix());

	//壁描画用のワールド行列
	DirectX::SimpleMath::Matrix world;

	//壁の描画
	for (int i = 0; i < static_cast<int>(m_wallBoxList.size()); i++)
	{
		//壁の位置にワールド行列を調整
		world = DirectX::SimpleMath::Matrix::CreateTranslation(m_wallBoxList[i].center);
		m_pWallModel->Draw(deviceContext, *CommonState, world, viewMatrix, projection->GetMatrix());
	}
}

//リセット処理
void Map::Reset()
{
	if (m_MapDataList.size() != 0)
	{
		m_MapDataList.clear();
	}

	if (m_wallBoxList.size() != 0)
	{
		m_wallBoxList.clear();
	}
}

//壁の当たり判定
DirectX::SimpleMath::Vector3 Map::HitWallBox(const Collision::Box3D& target)
{
	DirectX::SimpleMath::Vector3 rePos = target.center;

	for (int i = 0; i < static_cast<int>(m_wallBoxList.size()); i++)
	{
		if (Collision::HitCheckoxBox3D(target, m_wallBoxList[i]))
		{
			if (target.center.x <= (m_wallBoxList[i].center.x + m_wallBoxList[i].size.x)
				&& target.center.x >= (m_wallBoxList[i].center.x - m_wallBoxList[i].size.x))
			{
				if (target.center.z >= (m_wallBoxList[i].center.z + m_wallBoxList[i].size.z))
				{
					rePos = DirectX::SimpleMath::Vector3(
						target.center.x,
						target.center.y,
						(m_wallBoxList[i].center.z + m_wallBoxList[i].size.z + target.size.z)
					);
				}
				else if (target.center.z <= (m_wallBoxList[i].center.z - m_wallBoxList[i].size.z))
				{
					rePos = DirectX::SimpleMath::Vector3(
						target.center.x,
						target.center.y,
						(m_wallBoxList[i].center.z - m_wallBoxList[i].size.z - target.size.z)
					);
				}
			}
			else if (target.center.z <= (m_wallBoxList[i].center.z + m_wallBoxList[i].size.z)
				&& target.center.z >= (m_wallBoxList[i].center.z - m_wallBoxList[i].size.z))
			{
				if (target.center.x >= (m_wallBoxList[i].center.x + m_wallBoxList[i].size.x))
				{
					rePos = DirectX::SimpleMath::Vector3(
						(m_wallBoxList[i].center.x + m_wallBoxList[i].size.x + target.size.x),
						target.center.y,
						target.center.z);
				}
				else if (target.center.x <= (m_wallBoxList[i].center.x - m_wallBoxList[i].size.x))
				{
					rePos = DirectX::SimpleMath::Vector3(
						(m_wallBoxList[i].center.x - m_wallBoxList[i].size.x - target.size.x),
						target.center.y,
						target.center.z);
				}
			}
		}
	}

	return rePos;
}

//間に壁があるか
DirectX::SimpleMath::Vector3 Map::LineOfSight(const DirectX::SimpleMath::Vector3 &pos, const DirectX::SimpleMath::Vector3 &baseFront, const DirectX::SimpleMath::Matrix &rotation)
{
	Collision::Box3D splite
	{
		pos,
		DirectX::SimpleMath::Vector3(0.5f,0.5f,10.0f),
	};

	DirectX::SimpleMath::Vector3 boxPos;

	for (int i = 0; i < static_cast<int>(m_wallBoxList.size()); i++)
	{
		boxPos = Collision::DivideEquallyBox_Plus(splite, baseFront, rotation, m_wallBoxList[i], 20, 2);

		if (boxPos != DirectX::SimpleMath::Vector3::One * 100)
		{
			break;
		}
	}

	return boxPos;
}

//外側の限界
DirectX::SimpleMath::Vector3 Map::Limit(const DirectX::SimpleMath::Vector3 &pos, float size)
{
	//returnで返す値
	DirectX::SimpleMath::Vector3 retPos = pos;

	//横の判定
	if (pos.x + size >= m_halfWidthSize)
	{
		retPos.x = m_halfWidthSize - size;
	}
	else if (pos.x- size <= -m_halfWidthSize)
	{
		retPos.x = -m_halfWidthSize + size;
	}

	//縦の判定
	if (pos.z+ size >= m_halfHightSize)
	{
		retPos.z = m_halfHightSize - size;
	}
	else if (retPos.z- size <= -m_halfHightSize)
	{
		retPos.z = -m_halfHightSize + size;
	}

	return retPos;
}

//移動限界の壁のエフェクト(プレイヤーしか呼び出さない)
void Map::LimitEffect(const DirectX::SimpleMath::Vector3 &pos, float size)
{
	//壁の位置
	DirectX::SimpleMath::Vector3 wallPos = pos;
	//横か縦(Z軸)のどちらにデータを入れるか判定
	bool next = false;
	//前後と左右どちらの位置を設定するか判定
	CheckLimitEffect check;

	if (std::abs((pos.x - m_halfWidthSize)) <= m_halfWidthSize)
	{
		wallPos.x = m_halfWidthSize;

		check = CheckLimitEffect::CheckLeft;
	}
	else if (std::abs((pos.x + m_halfWidthSize)) <= m_halfWidthSize)
	{
		wallPos.x = -m_halfWidthSize;

		check = CheckLimitEffect::CheckRight;
	}

	//データの設定(横のデータの設定)
	LimitEffectSetPos(pos, wallPos, check);
	next = true;

	wallPos = pos;
	if (std::abs((pos.z - m_halfHightSize)) <= m_halfHightSize)
	{
		wallPos.z = m_halfHightSize;

		check = CheckLimitEffect::CheckZ;
	}

	else if (std::abs((pos.z + m_halfHightSize)) <= m_halfHightSize)
	{
		wallPos.z = -m_halfHightSize;

		check = CheckLimitEffect::CheckZ;
	}

	//データの設定(横のデータの設定)
	LimitEffectSetPos(pos, wallPos,check);
}

//エフェクト用の位置を設定
void Map::LimitEffectSetPos(const DirectX::SimpleMath::Vector3 &playerPos, const DirectX::SimpleMath::Vector3 &wallPos, const CheckLimitEffect &check)
{
	//移動限界のエフェクトの表示
	int i = 0;

	if (check == CheckLimitEffect::CheckZ)
	{
		i++;
	}

	//移動限界のエフェクトの角度(横)
	if (check == CheckLimitEffect::CheckRight)
	{
		//横しか角度を変えないため0を指定
		m_pHitWallList[0]->SetRange(EFFECT_ANGLE_R);
	}
	else if(check == CheckLimitEffect::CheckLeft)
	{
		//横しか角度を変えないため0を指定
		m_pHitWallList[0]->SetRange(EFFECT_ANGLE_L);
	}

	//壁の位置
	m_pHitWallList[i]->SetWallPos(wallPos);
	//プレイヤーの位置
	m_pHitWallList[i]->SetPlayerPos(playerPos);
}

//当たった方向の壁の二つの角の位置を返す
Map::TwoCornerPos Map::GetHitWallCorner(const DirectX::SimpleMath::Vector3 &pos) const
{
	TwoCornerPos ret =
	{
		DirectX::SimpleMath::Vector3::Zero,
		DirectX::SimpleMath::Vector3::Zero,
	};

	if (pos.x >= m_halfWidthSize)
	{
		ret.pos1 = DirectX::SimpleMath::Vector3(m_halfWidthSize, 0.0f, -m_halfHightSize);
		ret.pos2 = DirectX::SimpleMath::Vector3(m_halfWidthSize, 0.0f, m_halfHightSize);
	}

	else if (pos.x <= -m_halfWidthSize)
	{
		ret.pos1 = DirectX::SimpleMath::Vector3(-m_halfWidthSize,0.0f, -m_halfHightSize);
		ret.pos2 = DirectX::SimpleMath::Vector3(-m_halfWidthSize, 0.0f, m_halfHightSize);
	}

	if (pos.z >= m_halfHightSize)
	{
		ret.pos1 = DirectX::SimpleMath::Vector3(m_halfWidthSize, 0.0f, m_halfHightSize);
		ret.pos2 = DirectX::SimpleMath::Vector3(-m_halfWidthSize, 0.0f, m_halfHightSize);
	}

	else if (pos.z <= -m_halfHightSize)
	{
		ret.pos1 = DirectX::SimpleMath::Vector3(m_halfWidthSize, 0.0f, -m_halfHightSize);
		ret.pos2 = DirectX::SimpleMath::Vector3(-m_halfWidthSize, 0.0f, -m_halfHightSize);
	}

	return ret;
}

//マップデータの設定
void Map::SetMapData(std::vector<std::vector<int>> mapDataList)
{
	m_MapDataList = mapDataList;
	//縦と横の広さを格納する(半分のデータを入れるために0.5をかける)
	m_halfHightSize = (m_MapDataList.size()*0.5f);
	m_halfWidthSize = (m_MapDataList[0].size()*0.5f);
}

//地面の作成
void Map::CreatGround()
{
	//高さは変えないためyは1
	//マップモデルのサイズを本来の大きさにするために＊2をする
	m_groundWorld = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(m_halfWidthSize*2.0f, 1.0f, m_halfHightSize*2.0f));
}

//壁の作成
void Map::CreatWall()
{
	//壁の位置
	//X座標
	float x = -m_halfWidthSize;
	//Z座標
	float z = -m_halfHightSize;

	for (int i = 0; i < static_cast<int>(m_MapDataList.size()); i++)
	{
		for (int j = 0; j < static_cast<int>(m_MapDataList[i].size()); j++)
		{
			//壁の時
			if (m_MapDataList[i][j] == 1)
			{
				Collision::Box3D box =
				{
					//壁の位置
					DirectX::SimpleMath::Vector3(x, 0.0f, z),
					//壁の当たり判定の大きさ
					WALL_BOX_SIZE,
				};

				//壁のデータを設定
				m_wallBoxList.push_back(box);
			}
			x++;
		}
		//位置の調整
		x = -m_halfWidthSize;
		z++;
	}
}