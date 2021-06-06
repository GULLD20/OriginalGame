#pragma once
class Params
{
public:
	//敵の種類
	enum EnemyTyp
	{
		Goblin = 0,
		LongRange,
		LongRangeRowling,
		Stamp,
		Rowling
	};
public:
	///プレイヤーのステータス;
	//攻撃力
	const static float PLAYER_ATTACK;
	//体力
	const static float PLAYER_HP;
	//速度(1秒間に進む速度)
	const static float PLAYER_SPEED;
	//当たり判定の半径
	const static float PLAYER_RANGE;

public:
	///ゴブリンのステータス;
	//攻撃力
	const static float GOBLIN_ATTACK;
	//体力
	const static float GOBLIN_HP;
	//速度(1秒間に進む速度)
	const static float GOBLIN_SPEED;
	//当たり判定の半径
	const static float GOBLIN_RANGE;

public:
	///遠距離攻撃の敵のステータス
	//攻撃力
	const static float LONGRANGE_ATTACK;
	//体力
	const static float LONGRANGE_HP;
	//速度(1秒間に進む速度)
	const static float LONGRANGE_SPEED;
	//当たり判定の半径
	const static float LONGRANGE_RANGE;
public:
	///回転する遠距離攻撃の敵のステータス
	//攻撃力
	const static float LONGRANGEROWLING_ATTACK;
	//体力
	const static float LONGRANGEROWLING_HP;
	//速度(1秒間に進む速度)
	const static float LONGRANGEROWLING_SPEED;
	//当たり判定の半径
	const static float LONGRANGEROWLING_RANGE;

public:
	///押しつぶす敵のステータス
	//攻撃力
	const static float STAMP_ATTACK;
	//体力
	const static float STAMP_HP;
	//速度(1秒間に進む速度)
	const static float STAMP_SPEED;
	//当たり判定の半径
	const static float STAMP_RANGE;

public:
	///回転攻撃の敵のステータス
	//攻撃力
	const static float ROWLING_ATTACK;
	//体力
	const static float ROWLING_HP;
	//速度(1秒間に進む速度)
	const static float ROWLING_SPEED;
	//当たり判定の半径
	const static float ROWLING_RANGE;
};

