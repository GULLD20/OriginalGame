#include "../../pch.h"

#include "Params.h"

///プレイヤーのステータス;
//攻撃力
const float Params::PLAYER_ATTACK = 20.0f;
//体力
const float Params::PLAYER_HP = 400.0f;
//速度(1秒間に進む速度)
const float Params::PLAYER_SPEED =30.0f;
//当たり判定の半径
const float Params::PLAYER_RANGE = 0.5f;

///ゴブリンのステータス;
//攻撃力
const float Params::GOBLIN_ATTACK = 15.0f;
//体力
const float Params::GOBLIN_HP = 250.0f;
//速度(1秒間に進む速度)
const float Params::GOBLIN_SPEED = 20.0f;
//当たり判定の半径
const float Params::GOBLIN_RANGE = 0.5f;

///遠距離攻撃の敵のステータス
//攻撃力
const float Params::LONGRANGE_ATTACK = 40.0f;
//体力 
const float Params::LONGRANGE_HP = 200.0f;
//速度 (1秒間に進む速度)
const float Params::LONGRANGE_SPEED = 0.0f;
//当たり判定の半径
const float Params::LONGRANGE_RANGE = 0.5f;

///回転する遠距離攻撃の敵のステータス
//攻撃力
const float Params::LONGRANGEROWLING_ATTACK = 40.0f;
//体力 
const float Params::LONGRANGEROWLING_HP = 200.0f;
//速度 (1秒間に進む速度)
const float Params::LONGRANGEROWLING_SPEED = 0.0f;
//当たり判定の半径
const float Params::LONGRANGEROWLING_RANGE = 0.5f;

///押しつぶす敵のステータス
//攻撃力
const float Params::STAMP_ATTACK = 25.0f;
//体力
const float Params::STAMP_HP = 300.0f;
//速度(1秒間に進む速度)
const float Params::STAMP_SPEED = 30.0f;
//当たり判定の半径
const float Params::STAMP_RANGE = 0.5f;

///回転攻撃の敵のステータス
//攻撃力
const float Params::ROWLING_ATTACK = 20.0f;
//体力
const float Params::ROWLING_HP = 100.0f;
//速度(1秒間に進む速度)
const float Params::ROWLING_SPEED = 40.0f;
//当たり判定の半径
const float Params::ROWLING_RANGE = 1.0f;