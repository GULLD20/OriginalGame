#pragma once

#include <SimpleMath.h>
#include <vector>

#include "../UI/Score.h"

class DataManager
{
public:
	DataManager();
	~DataManager();

	void Initialize();
	//---------------------------------------------------------

	//ゲーム内データ---------------------------------
	//時間
	float GetTime() const;
	void SetTime(float time);
	//スコア
	std::vector<Score::ScoreData> GetScore();
	void SetScore(const std::vector<Score::ScoreData> &scoreList);
	//クリア判定
	bool GetClear() const;
	void SetClear(bool clear);
	//-----------------------------------------------

	//リセット---------------------------------------
	void Reset();
	//-----------------------------------------------

private:
	///プレイヤーのデータ-----------------------------
	//カメラ用回転行列
	DirectX::SimpleMath::Matrix   m_playerCameraRotation;
	///-----------------------------------------------

	///ゲーム内データ---------------------------------
	//時間
	float                         m_time;
	//スコア
	std::vector<Score::ScoreData> m_scoreList;
	//クリア判定
	bool                          m_clear;
	///-----------------------------------------------
};

