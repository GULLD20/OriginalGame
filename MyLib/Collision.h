///個人ライブラリー------------------------------
//必須---------
//環境
//DirectXTK
//インクルード
//SimpleMath
//-------------
//内容
//当たり判定
//3D
//四角と球
//2D
//四角と球
///----------------------------------------------

#pragma once

//インクルード
#include <SimpleMath.h>

class Collision
{
public:
	///構造体-----------------------------------
	//3D-------------------------------------
	//四角形
	struct Box3D
	{
		//中心位置
		DirectX::SimpleMath::Vector3 center;
		//(中心位置からの)長さ
		DirectX::SimpleMath::Vector3 size;
	};

	//球
	struct Sphere3D
	{
		//中心位置
		DirectX::SimpleMath::Vector3 center;
		//半径
		float radius;
	};

	//---------------------------------------
	//2D-------------------------------------
	//四角形
	struct Box2D
	{
		//中心位置
		DirectX::SimpleMath::Vector2 center;
		//(中心位置からの)長さ
		DirectX::SimpleMath::Vector2 size;
	};

	//球
	struct Sphere2D
	{
		//中心位置
		DirectX::SimpleMath::Vector2 center;
		//半径
		float radius;
	};

	//縦(Height)、横(Width)、奥行き(Depth)
	struct HWD
	{
		int h;
		int w;
		int d;
	};
	//---------------------------------------
	///-----------------------------------------

	//------------------------------------------
	enum
	{
		height,
		width,
		depth
	};
	//------------------------------------------

	//当たり判定--------------------------------
	//3D-------------------------------------
	//四角同士の当たり判定
	static bool HitCheckoxBox3D(const Box3D& box1, const Box3D& box2);
	//四角の当たり判定(plus方向にずらして判定)当たった位置を返す
	static DirectX::SimpleMath::Vector3 DivideEquallyBox_Plus(const Box3D& splitBox, const DirectX::SimpleMath::Vector3& baseFrontVec, const DirectX::SimpleMath::Matrix& rotation, const Box3D& targetBox, const int splitNum, const int select);
	//四角の当たり判定(minus方向にずらして判定)当たった位置を返す
	static DirectX::SimpleMath::Vector3 DivideEquallyBox_Minus(const Box3D& splitBox, const DirectX::SimpleMath::Vector3& baseFrontVec, const DirectX::SimpleMath::Matrix& rotation, const Box3D& targetBox, const int splitNum, const int select, Box3D& data);
	//四角の当たり判定(plus・minus交互に位置をずらして判定)当たった位置を返す
	static DirectX::SimpleMath::Vector3 DivideEquallyBox_Alternate(const Box3D& splitBox, const DirectX::SimpleMath::Vector3& pos, const DirectX::SimpleMath::Matrix& rotation, const Box3D& targetBox, const int splitNum, const int select, Box3D& data);

	//球同士の当たり判定
	static bool HitCheckoxSphere3D(const Sphere3D& sphere1, const Sphere3D& sphere2);
	//---------------------------------------

	//2D-------------------------------------
	//四角同士のの当たり判定
	static bool HitCheckoxBox2D(const Box2D& box1, const Box2D& box2);

	//球同士の当たり判定
	static bool HitCheckSphere2D(const Sphere2D& sphere1, const Sphere2D& sphere2);
	//---------------------------------------
	//------------------------------------------



	//先生が組んだもの--------------------------
public:
	// カプセルの構造体
	struct Capsule3D
	{
		DirectX::SimpleMath::Vector3 startPos; // 中間部の線分の開始点
		DirectX::SimpleMath::Vector3 endPos; // 中間部の線分の終了点
		float range;   // カプセルの半径
	};

	//球とカプセルの衝突判定関数
	static bool HitCheck_Sphere2Capsule(const Sphere3D& sphere, const Capsule3D& capsule);

	// カプセルとカプセルの衝突判定関数
	static bool HitCheck_Capsule2Capsule(const Capsule3D& capsule1, const Capsule3D& capsule2);

	// 球とボックスの衝突判定関数
	static bool HitCheck_Sphere2Box(const Sphere3D& sphere, const Box3D& box);

	private:
	//------------------------------//
	// 衝突判定に必要な補助関数群	//
	//------------------------------//

	/// <summary>
	/// 点cと線分abの間の距離の平方を返す関数
	/// </summary>
	/// <param name="a">線分の始点</param>
	/// <param name="b">線分の終点</param>
	/// <param name="c">点</param>
	/// <returns>点cと線分abの間の距離の平方</returns>
	static float SqDistPointSegment(DirectX::SimpleMath::Vector3 a, DirectX::SimpleMath::Vector3 b, DirectX::SimpleMath::Vector3 c);

	// クランプ関数
	static inline float Clamp(float n, float min, float max);

	/// <summary>
	/// ２つの線分の最短距離の平方を返す関数
	/// </summary>
	/// <param name="p1">線分１の始点</param>
	/// <param name="q1">線分１の終点</param>
	/// <param name="p2">線分２の始点</param>
	/// <param name="q2">線分２の終点</param>
	/// <param name="s">線分１上の最短位置を表す係数</param>
	/// <param name="t">線分２上の最短位置を表す係数</param>
	/// <param name="c1">線分１上の最短距離の位置</param>
	/// <param name="c2">線分２上の最短距離の位置</param>
	/// <returns>２つの線分の最短距離の平方</returns>
	static float ClosestPtSegmentSegment(DirectX::SimpleMath::Vector3 p1, DirectX::SimpleMath::Vector3 q1
		, DirectX::SimpleMath::Vector3 p2, DirectX::SimpleMath::Vector3 q2
		, float &s, float &t
		, DirectX::SimpleMath::Vector3& c1, DirectX::SimpleMath::Vector3& c2);

	// 点とボックスの間の最短距離の平方を計算する関数
	static float SqDistPointBox(const DirectX::SimpleMath::Vector3& p, const Box3D& b);

	//------------------------------//
	// 線分と三角形の交差判定用		//
	//------------------------------//

	// 三角形の構造体（線分と三角形の交差判定用）
	struct Triangle
	{
		// 三角形の平面方程式
		DirectX::SimpleMath::Plane p;
		// 辺BCの平面方程式（重心座標の頂点aに対する重みuを与える）
		DirectX::SimpleMath::Plane edgePlaneBC;
		// 辺CAの平面方程式（重心座標の頂点bに対する重みvを与える）
		DirectX::SimpleMath::Plane edgePlaneCA;

		// コンストラクタ内で衝突判定を軽くするために前処理で計算しておく
		Triangle(DirectX::SimpleMath::Vector3 a, DirectX::SimpleMath::Vector3 b, DirectX::SimpleMath::Vector3 c)
		{
			DirectX::SimpleMath::Vector3 n = (c - a).Cross(b - a);
			p = DirectX::SimpleMath::Plane(a, n);
			DirectX::SimpleMath::Plane pp = DirectX::SimpleMath::Plane(b, n);
			edgePlaneBC = DirectX::SimpleMath::Plane(b, n.Cross(b - c));
			edgePlaneCA = DirectX::SimpleMath::Plane(c, n.Cross(c - a));
			p.Normalize(); edgePlaneBC.Normalize(); edgePlaneCA.Normalize();
			float bc_scale = 1.0f / (a.Dot(edgePlaneBC.Normal()) + edgePlaneBC.D());
			float ca_scale = 1.0f / (b.Dot(edgePlaneCA.Normal()) + edgePlaneCA.D());
			edgePlaneBC.x *= bc_scale; edgePlaneBC.y *= bc_scale; edgePlaneBC.z *= bc_scale; edgePlaneBC.w *= bc_scale;
			edgePlaneCA.x *= ca_scale; edgePlaneCA.y *= ca_scale; edgePlaneCA.z *= ca_scale; edgePlaneCA.w *= ca_scale;
		}
	};

	// 浮動小数点の誤差で当たりぬけするので少し余裕をもつ
#define EPSILON 1.0e-06F

	/// <summary>
	/// 線分と三角形の交差判定
	/// </summary>
	/// <param name="p">線分の始点</param>
	/// <param name="q">線分の終点</param>
	/// <param name="tri">三角形</param>
	/// <param name="s">線分と三角形の交差点</param>
	/// <returns>trueの場合交差している</returns>
	static bool IntersectSegmentTriangle(DirectX::SimpleMath::Vector3 p, DirectX::SimpleMath::Vector3 q, Triangle tri, DirectX::SimpleMath::Vector3* s);
	//------------------------------------------
};