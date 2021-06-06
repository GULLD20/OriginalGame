#include "../pch.h"

#include "Collision.h"
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

	//当たり判定--------------------------------
	//3D-------------------------------------
	//四角同士の当たり判定
	bool Collision::HitCheckoxBox3D(const Box3D& box1, const Box3D& box2)
	{
		//四角の中心位置の距離とサイズの合計を比較
		//比較するときは絶対値で行う

		if (fabsf(box1.center.x - box2.center.x) > (box1.size.x + box2.size.x))
			return false;

		if (fabsf(box1.center.y - box2.center.y) > (box1.size.y + box2.size.y))
			return false;

		if (fabsf(box1.center.z - box2.center.z) > (box1.size.z + box2.size.z))
			return false;

		return true;
	}

	//四角の当たり判定(plus方向にずらして判定)当たった位置を返す
	DirectX::SimpleMath::Vector3 Collision::DivideEquallyBox_Plus(const Box3D& splitBox, const DirectX::SimpleMath::Vector3& baseFrontVec, const DirectX::SimpleMath::Matrix& rotation, const Box3D& targetBox, const int splitNum, const int select)
	{
		//当たり判定用の四角の最大の大きさ
		DirectX::SimpleMath::Vector3 fullSize = DirectX::SimpleMath::Vector3::Transform(splitBox.size,rotation);

		//当たり判定用の四角を回転させた値
		Box3D fullBox =
		{
			splitBox.center + (DirectX::SimpleMath::Vector3::Transform(baseFrontVec,rotation)*(splitNum / 2.0f)),
			DirectX::SimpleMath::Vector3(std::abs(fullSize.x),std::abs(fullSize.y),std::abs(fullSize.z))
		};

		//処理をするか判定
		if (!HitCheckoxBox3D(targetBox, fullBox))
		{
			return DirectX::SimpleMath::Vector3(100.0f, 100.0f, 100.0f);
		}


		//分けた後の四角の大きさ(基準)
		DirectX::SimpleMath::Vector3 splitBaseSize;
		//分けた後の四角の大きさ(当たり判定に使用)
		DirectX::SimpleMath::Vector3 boxSize;
		//最初に向いている方向へのベクトル
		DirectX::SimpleMath::Vector3 baseVec;
		//正面に向けたベクトル(四角の位置をずらす)
		DirectX::SimpleMath::Vector3 frontVec;
		//当たり判定を行う変数の半径のベクトルを入れるための変数
		DirectX::SimpleMath::Vector3 radiusVec = DirectX::SimpleMath::Vector3::Zero;
		//分ける軸の1辺の長さ
		float length = 0.0f;

		HWD hwd = { 0,0,0 };

		//縦に分ける
		if (height == select)
		{
			hwd.w++;
		}
		//横に分ける
		else if (width == select)
		{
			hwd.h++;
		}
		//奥に分ける
		else if (depth == select)
		{
			hwd.d++;
		}

		baseVec = baseFrontVec;

		if (baseVec.x == 0.0f)
		{
			baseVec.x++;
		}
		if (baseVec.y == 0.0f)
		{
			baseVec.y++;
		}
		if (baseVec.z == 0.0f)
		{
			baseVec.z++;
		}

		//縦に分けるため四角の横の長さをsplitNumで割る(splitNumの数に分割する)
		//割った後の長さが四角のxの基準になる
		length = ((splitBox.size.x * hwd.w) / (splitNum)) + ((splitBox.size.y * hwd.h) / (splitNum)) + ((splitBox.size.z * hwd.d) / (splitNum));
		//当たり判定の基準を作成(縦に分けるため変更するのは横(x)のみ)
		splitBaseSize = DirectX::SimpleMath::Vector3((-splitBox.size.x* (hwd.w - 1)) + (length* hwd.w), (-splitBox.size.y* (hwd.h - 1)) + (length* hwd.h), (-splitBox.size.z* (hwd.d - 1)) + (length* hwd.d));

		//自分(pos)からPlus方向にに向かって位置をずらして当たり判定を行う
		//四角の位置をずらすために回す
		for (int i = 0; i < splitNum; i++)
		{
			//自分(posのデータを持っているクラス)の正面に四角の位置をずらすため
			//for文が回るたびに四角の位置がずれていく

			//分けるため1辺の長さを求めてそれから回転させる
			frontVec = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3((length * hwd.w*baseVec.x)*i, (length * hwd.h*baseVec.y)*i, (length * hwd.d*baseVec.z)*i), rotation);

			//1回だけ処理するため
			if (radiusVec == DirectX::SimpleMath::Vector3::Zero)
			{
				//当たり判定に使う変数の半径のベクトルを入れる
				radiusVec = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3((length * hwd.w), (length * hwd.h), (length * hwd.d)), rotation);
			}

			//当たり判定の大きさを回転させる(四角が自分(posのデータを持っているクラス)と同じ向きになるため)
			boxSize = DirectX::SimpleMath::Vector3::Transform(splitBaseSize, rotation);

			//当たり判定に使う四角
			Box3D box =
			{
				splitBox.center + frontVec,
				boxSize,
			};

			//当たり判定
			if (HitCheckoxBox3D(targetBox, box))
			{
				//当たったら位置を返す
				return box.center + radiusVec;
			}
		}

		//当たらなかった場合
		return DirectX::SimpleMath::Vector3(100.0f, 100.0f, 100.0f);
	}

	//四角の当たり判定(minus方向にずらして判定)当たった位置を返す
	DirectX::SimpleMath::Vector3 Collision::DivideEquallyBox_Minus(const Box3D& splitBox, const DirectX::SimpleMath::Vector3& baseFrontVec, const DirectX::SimpleMath::Matrix& rotation, const Box3D& targetBox, const int splitNum, const int select, Box3D& data)
	{
		//分けた後の四角の大きさ(基準)
		DirectX::SimpleMath::Vector3 splitBaseSize;
		//分けた後の四角の大きさ(当たり判定に使用)
		DirectX::SimpleMath::Vector3 boxSize;
		//最初に向いている方向へのベクトル
		DirectX::SimpleMath::Vector3 baseVec;
		//正面に向けたベクトル(四角の位置をずらす)
		DirectX::SimpleMath::Vector3 frontVec;
		//当たり判定を行う変数の半径のベクトルを入れるための変数
		DirectX::SimpleMath::Vector3 radiusVec = DirectX::SimpleMath::Vector3::Zero;
		//分ける軸の1辺の長さ
		float length = 0.0f;

		HWD hwd = { 0,0,0 };

		//縦に分ける
		if (height == select)
		{
			hwd.w++;
		}
		//横に分ける
		else if (width == select)
		{
			hwd.h++;
		}
		//奥に分ける
		else if (depth == select)
		{
			hwd.d++;
		}

		baseVec = baseFrontVec;

		if (baseVec.x == 0.0f)
		{
			baseVec.x++;
		}
		if (baseVec.y == 0.0f)
		{
			baseVec.y++;
		}
		if (baseVec.z == 0.0f)
		{
			baseVec.z++;
		}

		//縦に分けるため四角の横の長さをsplitNumで割る(splitNumの数に分割する)
		//割った後の長さが四角のxの基準になる
		length = ((splitBox.size.x * hwd.w) / (splitNum)) + ((splitBox.size.y * hwd.h) / (splitNum)) + ((splitBox.size.z * hwd.d) / (splitNum));
		//当たり判定の基準を作成(縦に分けるため変更するのは横(x)のみ)
		splitBaseSize = DirectX::SimpleMath::Vector3((-splitBox.size.x* (hwd.w - 1)) + (length* hwd.w), (-splitBox.size.y* (hwd.h - 1)) + (length* hwd.h), (-splitBox.size.z* (hwd.d - 1)) + (length* hwd.d));

		//自分(pos)からPlus方向にに向かって位置をずらして当たり判定を行う
		//四角の位置をずらすために回す
		for (int i = 0; i < splitNum; i++)
		{
			//自分(posのデータを持っているクラス)の正面に四角の位置をずらすため
			//for文が回るたびに四角の位置がずれていく

			//分けるため1辺の長さを求めてそれから回転させる
			frontVec = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3((length * hwd.w*baseVec.x)*i, (length * hwd.h*baseVec.y)*i, (length * hwd.d*baseVec.z)*i), rotation);

			//1回だけ処理するため
			if (radiusVec == DirectX::SimpleMath::Vector3::Zero)
			{
				//当たり判定に使う変数の半径のベクトルを入れる
				radiusVec = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3((length * hwd.w), (length * hwd.h), (length * hwd.d)), rotation);
			}

			//当たり判定の大きさを回転させる(四角が自分(posのデータを持っているクラス)と同じ向きになるため)
			boxSize = DirectX::SimpleMath::Vector3::Transform(splitBaseSize, rotation);

			//当たり判定に使う四角
			Box3D box =
			{
				splitBox.center + frontVec,
				boxSize,
			};

			//確認用(後ほど消す)
			data = box;

			//当たり判定
			if (HitCheckoxBox3D(targetBox, box))
			{
				//当たったら位置を返す
				return box.center + radiusVec;
			}
		}

		//当たらなかった場合
		return DirectX::SimpleMath::Vector3(100.0f, 100.0f, 100.0f);
	}

	//四角の当たり判定(plus・minus交互に位置をずらして判定)当たった位置を返す
	DirectX::SimpleMath::Vector3 Collision::DivideEquallyBox_Alternate(const Box3D& splitBox, const DirectX::SimpleMath::Vector3& pos, const DirectX::SimpleMath::Matrix& rotation, const Box3D& targetBox, const int splitNum, const int select, Box3D& data)
	{
		//分けた後の四角の大きさ(基準)
		DirectX::SimpleMath::Vector3 splitBaseSize;
		//分けた後の四角の大きさ(当たり判定に使用)
		DirectX::SimpleMath::Vector3 boxSize;
		//正面に向けたベクトル(四角の位置をずらす)
		DirectX::SimpleMath::Vector3 frontVec;
		//分ける軸の1辺の長さ
		float length = 0.0f;
		//自分(pos)を中心に交互(前後もしくは左右)に判定を行うため
		int count = 0;
		//当たり判定を行う変数の半径のベクトルを入れるための変数
		DirectX::SimpleMath::Vector3 radiusVec = DirectX::SimpleMath::Vector3::Zero;

		//縦に分ける
		if (height == select)
		{
			//縦に分けるため四角の横の長さをsplitNumで割る(横をsplitNumの数に分割する)
			//割った後の長さが四角のxの基準になる
			length = (splitBox.size.x * 2) / (splitNum * 2);
			//当たり判定の基準を作成(縦に分けるため変更するのは横(x)のみ)
			//
			splitBaseSize = DirectX::SimpleMath::Vector3(length, splitBox.size.y, splitBox.size.z);
		}
		//横に分ける
		else if (width == select)
		{
			//縦に分けるため四角の横の長さをsplitNumで割る(横をsplitNumの数に分割する)
			//割った後の長さが四角のyの基準になる
			length = (splitBox.size.y * 2) / (splitNum * 2);
			//当たり判定の基準を作成(横に分けるため変更するのは横(y)のみ)
			splitBaseSize = DirectX::SimpleMath::Vector3(splitBox.size.x, length, splitBox.size.z);
		}
		//奥に分ける
		else if (depth == select)
		{
			//縦に分けるため四角の横の長さをsplitNumで割る(横をsplitNumの数に分割する)
			//割った後の長さが四角のzの基準になる
			length = (splitBox.size.z * 2) / (splitNum * 2);
			//当たり判定の基準を作成(奥に分けるため変更するのは横(z)のみ)
			splitBaseSize = DirectX::SimpleMath::Vector3(splitBox.size.x, splitBox.size.y, length);
		}

		//自分(pos)から正面に向かって位置をずらして当たり判定を行う
		//四角の位置をずらすために回す
		for (int i = 0; i < splitNum * 2; i++)
		{
			if (i % 2 == 0)
			{
				//自分(posのデータを持っているクラス)の正面に四角の位置をずらすため
				//for文が回るたびに四角の位置がずれていく
				if (height == select)
				{
					//縦で分けるためxの長さを求めてそれから回転させる
					frontVec = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(length*count, 0.0f, 0.0f), rotation);

					//1回だけ処理するため
					if (radiusVec == DirectX::SimpleMath::Vector3::Zero)
					{
						//当たり判定に使う変数の半径のベクトルを入れる
						radiusVec = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(length * 1, 0.0f, 0.0f), rotation);
					}
				}
				else if (width == select)
				{
					//横で分けるためyの長さを求めてそれから回転させる
					frontVec = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, length*count, 0.0f), rotation);
					//1回だけ処理するため
					if (radiusVec == DirectX::SimpleMath::Vector3::Zero)
					{
						//当たり判定に使う変数の半径のベクトルを入れる
						radiusVec = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, length * 1, 0.0f), rotation);
					}
				}
				else if (depth == select)
				{
					//奥で分けるためzの長さを求めてそれから回転させる
					frontVec = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 0.0f, -length*count), rotation);
					//1回だけ処理するため
					if (radiusVec == DirectX::SimpleMath::Vector3::Zero)
					{
						//当たり判定に使う変数の半径のベクトルを入れる
						radiusVec = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 0.0f, -length * 1), rotation);
					}
				}
			}
			else
			{
				//自分(posのデータを持っているクラス)の正面に四角の位置をずらすため
				//for文が回るたびに四角の位置がずれていく
				if (height == select)
				{
					//縦で分けるためxの長さを求めてそれから回転させる
					frontVec = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(length*-count, 0.0f, 0.0f), rotation);
					//1回だけ処理するため
					if (radiusVec == DirectX::SimpleMath::Vector3::Zero)
					{
						//当たり判定に使う変数の半径のベクトルを入れる
						radiusVec = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(length*-1, 0.0f, 0.0f), rotation);
					}
				}
				else if (width == select)
				{
					//横で分けるためyの長さを求めてそれから回転させる
					frontVec = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, length*-count, 0.0f), rotation);
					//1回だけ処理するため
					if (radiusVec == DirectX::SimpleMath::Vector3::Zero)
					{
						//当たり判定に使う変数の半径のベクトルを入れる
						radiusVec = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, length*-1, 0.0f), rotation);
					}
				}
				else if (depth == select)
				{
					//奥で分けるためzの長さを求めてそれから回転させる
					frontVec = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 0.0f, -length*-count), rotation);
					//1回だけ処理するため
					if (radiusVec == DirectX::SimpleMath::Vector3::Zero)
					{
						//当たり判定に使う変数の半径のベクトルを入れる
						radiusVec = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 0.0f, -length*-1), rotation);
					}
				}
				count++;
			}

			//当たり判定の大きさを回転させる(四角が自分(posのデータを持っているクラス)と同じ向きになるため)
			boxSize = DirectX::SimpleMath::Vector3::Transform(splitBaseSize, rotation);

			//当たり判定に使う四角
			Box3D box =
			{
				pos + frontVec,
				boxSize,
			};

			//確認用(後ほど消す)
			data = box;

			//当たり判定
			if (HitCheckoxBox3D(targetBox, box))
			{
				//当たったら位置を返す(当たった時の四角の位置 + 半径のベクトル) 
				return box.center + radiusVec;
			}
		}

		//当たらなかった場合
		return DirectX::SimpleMath::Vector3(100.0f, 100.0f, 100.0f);

	}

	//球同士の当たり判定
	bool Collision::HitCheckoxSphere3D(const Sphere3D& sphere1, const Sphere3D& sphere2)
	{
		//球同士の距離を求める
		DirectX::SimpleMath::Vector3 d;
		d = sphere1.center - sphere2.center;
		//求めた距離の内積を求める(平方する)
		float dist2 = d.Dot(d);

		//半径の合計
		float radiusSum = sphere1.radius + sphere2.radius;
		//平方した距離が平方した半径の合計よりも小さい場合に球は交差している
		return dist2 <= radiusSum * radiusSum;
	}
	//---------------------------------------

	//2D-------------------------------------
	//四角同士のの当たり判定
	bool Collision::HitCheckoxBox2D(const Box2D& box1, const Box2D& box2)
	{
		if (fabsf(box1.center.x - box2.center.x) > (box1.size.x + box2.size.x))
			return false;

		if (fabsf(box1.center.y - box2.center.y) > (box1.size.y + box2.size.y))
			return false;

		return true;
	}

	//球同士の当たり判定
	bool Collision::HitCheckSphere2D(const Sphere2D& sphere1, const Sphere2D& sphere2)
	{
		DirectX::SimpleMath::Vector2 distance = sphere1.center - sphere2.center;
		float dist = distance.Dot(distance);

		if (dist < sphere1.radius + sphere2.radius)
			return false;

		return true;
	}
	//---------------------------------------
	//------------------------------------------


	//先生が組んだもの--------------------------
	//球とカプセルの衝突判定関数
	bool Collision::HitCheck_Sphere2Capsule(const Sphere3D& sphere, const Capsule3D& capsule)
	{
		// 球の中心とカプセルの中心の線分との距離の平方を計算
		float dist2 = SqDistPointSegment(capsule.startPos, capsule.endPos, sphere.center);
		float radius = sphere.radius + capsule.range;
		return dist2 <= radius * radius;
	}

	// カプセルとカプセルの衝突判定関数
	bool Collision::HitCheck_Capsule2Capsule(const Capsule3D& capsule1, const Capsule3D& capsule2)
	{
		float s, t;
		DirectX::SimpleMath::Vector3 c1, c2;
		// カプセルの中心の線分間の距離の平方を計算
		float dist2 = ClosestPtSegmentSegment(capsule1.startPos, capsule1.endPos, capsule2.startPos, capsule2.endPos, s, t, c1, c2);
		float radius = capsule1.range + capsule2.range;
		return dist2 <= radius * radius;
	}

	// 球とボックスの衝突判定関数
	bool Collision::HitCheck_Sphere2Box(const Sphere3D& sphere, const Box3D& box)
	{
		float sqDist = SqDistPointBox(sphere.center, box);
		return sqDist <= sphere.radius * sphere.radius;
	}
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
		float Collision::SqDistPointSegment(DirectX::SimpleMath::Vector3 a, DirectX::SimpleMath::Vector3 b, DirectX::SimpleMath::Vector3 c)
		{
			DirectX::SimpleMath::Vector3 ab = b - a;
			DirectX::SimpleMath::Vector3 ac = c - a;
			DirectX::SimpleMath::Vector3 bc = c - b;
			float e = ac.Dot(ab);
			if (e <= 0.0f) return ac.Dot(ac);
			float f = ab.Dot(ab);
			if (e >= f) return bc.Dot(bc);
			return ac.Dot(ac) - e * e / f;
		}

		// クランプ関数
		inline float Collision::Clamp(float n, float min, float max)
		{
			if (n < min) return min;
			if (n > max) return max;
			return n;
		}

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
		float Collision::ClosestPtSegmentSegment(DirectX::SimpleMath::Vector3 p1, DirectX::SimpleMath::Vector3 q1
			, DirectX::SimpleMath::Vector3 p2, DirectX::SimpleMath::Vector3 q2
			, float &s, float &t
			, DirectX::SimpleMath::Vector3& c1, DirectX::SimpleMath::Vector3& c2)
		{
			DirectX::SimpleMath::Vector3 d1 = q1 - p1;
			DirectX::SimpleMath::Vector3 d2 = q2 - p2;
			DirectX::SimpleMath::Vector3 r = p1 - p2;
			float a = d1.Dot(d1);
			float e = d2.Dot(d2);
			float f = d2.Dot(r);

			if (a <= FLT_EPSILON && e <= FLT_EPSILON)
			{
				s = t = 0.0f;
				c1 = p1;
				c2 = p2;
				return (c1 - c2).Dot(c1 - c2);
			}
			if (a <= FLT_EPSILON)
			{
				s = 0.0f;
				t = f / e;
				t = Clamp(t, 0.0f, 1.0f);
			}
			else
			{
				float c = d1.Dot(r);
				if (e <= FLT_EPSILON)
				{
					t = 0.0f;
					s = Clamp(-c / a, 0.0f, 1.0f);
				}
				else
				{
					float b = d1.Dot(d2);
					float denom = a * e - b * b;
					if (denom != 0.0f)
					{
						s = Clamp((b * f - c * e) / denom, 0.0f, 1.0f);
					}
					else
					{
						s = 0.0f;
					}
					float tnom = (b * s + f);
					if (tnom < 0.0f)
					{
						t = 0.0f;
						s = Clamp(-c / a, 0.0f, 1.0f);
					}
					else if (tnom > e)
					{
						t = 1.0f;
						s = Clamp((b - c) / a, 0.0f, 1.0f);
					}
					else
					{
						t = tnom / e;
					}
				}
			}
			c1 = p1 + d1 * s;
			c2 = p2 + d2 * t;
			return (c1 - c2).Dot(c1 - c2);
		}

		// 点とボックスの間の最短距離の平方を計算する関数
		float Collision::SqDistPointBox(const DirectX::SimpleMath::Vector3& p, const Box3D& b)
		{
			float point[3] = { p.x, p.y, p.z };
			float min[3] = { b.center.x - b.size.x, b.center.y - b.size.y, b.center.z - b.size.z, };
			float max[3] = { b.center.x + b.size.x, b.center.y + b.size.y, b.center.z + b.size.z, };

			float sqDist = 0.0f;
			for (int i = 0; i < 3; i++)
			{
				float v = point[i];
				if (v < min[i]) sqDist += (min[i] - v) * (min[i] - v);
				if (v > max[i]) sqDist += (v - max[i]) * (v - max[i]);
			}
			return sqDist;
		}

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
		bool Collision::IntersectSegmentTriangle(DirectX::SimpleMath::Vector3 p, DirectX::SimpleMath::Vector3 q, Triangle tri, DirectX::SimpleMath::Vector3* s)
		{
			float distp = p.Dot(tri.p.Normal()) + tri.p.D();
			if (distp < 0.0f) return false;
			float distq = q.Dot(tri.p.Normal()) + tri.p.D();
			if (distq >= 0.0f) return false;
			float denom = distp - distq;
			float t = distp / denom;
			*s = p + t * (q - p);
			float u = s->Dot(tri.edgePlaneBC.Normal()) + tri.edgePlaneBC.D();
			if (fabsf(u) < EPSILON) u = 0.0f;
			if (u < 0.0f || u > 1.0f) return false;
			float v = s->Dot(tri.edgePlaneCA.Normal()) + tri.edgePlaneCA.D();
			if (fabsf(v) < EPSILON) v = 0.0f;
			if (v < 0.0f) return false;
			float w = 1.0f - u - v;
			if (fabsf(w) < EPSILON) w = 0.0f;
			if (w < 0.0f) return false;
			return true;
		}
		//------------------------------------------