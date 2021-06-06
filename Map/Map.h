#pragma once

#include "Model.h"

#include <Effects.h>
#include "../../DeviceResources.h"
#include <CommonStates.h>
#include "../../Scene/Projection.h"
#include "../../Scene/GameContext.h"

#include <GeometricPrimitive.h>
#include <SimpleMath.h>

#include <vector>

#include "../MyLib/Collision.h"

#include "../MyEffect/Effect2D/HitWall/HitWall.h"

class CollisionMesh;

class Map
{
public:
	enum CheckLimitEffect
	{
		CheckRight = 0,
		CheckLeft,
		CheckZ
	};

	//マップのデータ(種類、位置、当たり判定の大きさ)
	struct MapData
	{
		int selectData;
		DirectX::SimpleMath::Vector3 pos;
		DirectX::SimpleMath::Vector3 size;
	};

	//二つの角の位置
	struct TwoCornerPos
	{
		DirectX::SimpleMath::Vector3 pos1;
		DirectX::SimpleMath::Vector3 pos2;
	};

	//壁の当たりは判定の大きさ
	const static DirectX::SimpleMath::Vector3 WALL_BOX_SIZE;
	//移動限界時のエフェクトの数
	const static int                          LIMIT_EFFECT_NUM;
	//エフェクトの角度
	//右
	const static float                        EFFECT_ANGLE_R;
	//左
	const static float                        EFFECT_ANGLE_L;

public:
	Map();
	~Map();

	//作成
	void Create();
	//初期化処理
	void Initialize();
	//描画処理
	void Draw(const DirectX::SimpleMath::Matrix &viewMatrix);
	//タイトル画面でのマップの表示
	void TitleDraw(const DirectX::SimpleMath::Matrix &viewMatrix);
	//リセット処理
	void Reset();
	//壁の当たり判定
	DirectX::SimpleMath::Vector3 HitWallBox(const Collision::Box3D& target);
	//間に壁があるか
	DirectX::SimpleMath::Vector3 LineOfSight(const DirectX::SimpleMath::Vector3 &pos, const DirectX::SimpleMath::Vector3 &baseFront, const DirectX::SimpleMath::Matrix &rotation);
	//移動限界の壁
	DirectX::SimpleMath::Vector3 Limit(const DirectX::SimpleMath::Vector3 &pos, float size);
	//移動限界の壁のエフェクト(プレイヤーしか呼び出さない)
	void LimitEffect(const DirectX::SimpleMath::Vector3 &pos, float size);
	//エフェクト用の位置を設定
	void LimitEffectSetPos(const DirectX::SimpleMath::Vector3 &playerPos, const DirectX::SimpleMath::Vector3 &wallPos, const CheckLimitEffect &check);
	//当たった方向の壁の二つの角の位置を返す
	TwoCornerPos GetHitWallCorner (const DirectX::SimpleMath::Vector3 &pos) const;

	//マップデータの設定
	void SetMapData(std::vector<std::vector<int>> mapDataList);
private:
	//地面の作成
	void CreatGround();
	//壁の作成
	void CreatWall();
private:
	//マップデータ
	std::vector<std::vector<int>>                   m_MapDataList;

	///地面
	//モデル
	std::unique_ptr<DirectX::Model>                 m_pGroundModel;
	//ワールド行列
	DirectX::SimpleMath::Matrix                     m_groundWorld;
	///壁
	//当たり判定
	std::vector<Collision::Box3D>                   m_wallBoxList;
	//モデル
	std::unique_ptr<DirectX::Model>                 m_pWallModel;

	//マップの縦横の広さの半分
	float                                           m_halfHightSize;
	float                                           m_halfWidthSize;

	//選択されたマップ
	int                                             m_selectMap;

	///天球
	//モデル
	std::unique_ptr<DirectX::Model>                 m_pSpace;
	//ワールド行列
	DirectX::SimpleMath::Matrix                     m_spaceWorld;
	//y軸の回転角度
	float                                           m_spaceAngleY;

	//プレイヤー壁に当たった時のエフェクト(0番目には横(X軸)の位置を1番目には縦(Z軸)の位置を入れる)
	std::vector<std::unique_ptr<HitWall>>           m_pHitWallList;
};

