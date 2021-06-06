#include "../pch.h"
#include "DataManager.h"
#include "../Character/Player/Weapon/WeaponType.h"

DataManager::DataManager()
	:m_time(0.0f)
{

}


DataManager::~DataManager()
{

}

//初期化処理
void DataManager::Initialize()
{
	m_time = 0.0f;
	m_clear = false;
}
//---------------------------------------------------------



//ゲーム内データ---------------------------------
//時間
float DataManager::GetTime() const
{
	//リザルトシーンに
	return m_time;
}

void DataManager::SetTime(float time)
{
	m_time = time;
}

//スコア
std::vector<Score::ScoreData> DataManager::GetScore()
{
	//タイトルシーンで参照
	return m_scoreList;
}

void DataManager::SetScore(const std::vector<Score::ScoreData>& scoreList)
{
	m_scoreList = scoreList;
}

//クリア判定
bool DataManager::GetClear() const
{
	return m_clear;
}

void DataManager::SetClear(bool clear)
{
	m_clear = clear;
}

//-----------------------------------------------


//リセット---------------------------------------
void DataManager::Reset()
{

}
//-----------------------------------------------