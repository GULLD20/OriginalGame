///個人ライブラリー------------------------------
//必須---------
//環境
//DirectXTK
//インクルード
//<SimpleMath>
//"Colition.h"
//<string>
//<sstream>
//<fstream>
//<vector>
//<random>
//-------------
//内容
//3D用
//移動：Move
//位置と進む方向に向けたベクトルを受け取る
//ベクトルに合わせて移動
//相手を発見(視線)：Discovery
//一定の範囲内に相手がいるときに自分(this)の視界の中(-visual～visualの間)にいれば発見(trueを返す)
//
//2D用
//
//その他
//ラープ関数：Lerp
//ラープ処理
//乱数の取得 int型：MersenneTwisterI
//乱数の取得 float型：MersenneTwisterF
//メルセンヌツイスター方で乱数取得
//引数の数値の間を乱数で取得する
//テキスト系のファイル(確認済み：text,csv)の読み込み：LoadFail_TextForm
//受け取ったファイル名を元にテキスト系ファイルの読み込みを行いう
//データの保存：SaveData
//指定したテキスト系ファイルにデータを保存
//文字の分割：SeparateString
//受け取ったストリング型のデータを受け取ったchar型のデータで区切る
///----------------------------------------------

#pragma once

//インクルード
#include <SimpleMath.h>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <random>

//当たり判定
#include "Collision.h"
class MyLib
{
public:
	//3D用-------------------------------------------------------------------

	//移動用
	//引数
	//自分の位置：posと進む距離：vec
	static DirectX::SimpleMath::Vector3 Move(const DirectX::SimpleMath::Vector3 &pos, const DirectX::SimpleMath::Vector3 &vec);

	//相手を発見(視線)
	//引数
	//自分の球：thisSphere,相手の球：targetSphere,向いている方向のベクトル：forwardVec,視覚(見える範囲)：visual
	static bool Discovery(const Collision::Sphere3D &thisSphere, const  Collision::Sphere3D &targetSphere, const  DirectX::SimpleMath::Vector3 &forwardVec, float visual);

	//ターゲットのほうに向く
	//引数
	//自分の位置：thisPos,相手の位置：targetPos,正面に向けたベクトル：frontVec
	static DirectX::SimpleMath::Matrix LockAt(const DirectX::SimpleMath::Vector3 &thisPos, const  DirectX::SimpleMath::Vector3 &targetPos, const DirectX::SimpleMath::Vector3 &frontVec);

	//カメラの描画範囲内(四角)
	//引数
	//カメラを中心とした(もしくはプレイヤーキャラ)四角：thisBox,対象の四角：targetBox
	static bool CameraWithInDrawRange(const Collision::Box3D &thisBox, const Collision::Box3D &targetBox);
	//-----------------------------------------------------------------------

	//2D---------------------------------------------------------------------

	//-----------------------------------------------------------------------

	//その他-----------------------------------------------------------------
	//ラープ関数
	static float Lerp(float start, float end, float t);

	//乱数を取得する関数
	//乱数の最小値：minimumNum、乱数の最大値：maxNum
	//int型用
	static int MersenneTwisterI(const int &minimumNum, const int &maxNum);

	//float型用
	static float MersenneTwisterF(const float &minimumNum, const float &maxNum);

	//テキスト形式のファイルの読み込み
	//引数
	//ファイルの位置 + ファイルの名前：fileName
	static std::vector<std::string> LoadFail_TextForm(const char* fileName);

	//テキスト系のデータの保存
	//引数
	//ファイルの位置 + ファイルの名前：fileName , 保存するデータ：data
	template<typename T>
	static void SaveData(const char* fileName, const std::vector<T> &dataList);

	//データを複数のファイルに保存する(呼び出すたびにファイルが増える)
	//引数
	//ファイルの位置 + ファイルの名前：fileName , ファイル形式：fileFormat , 保存するデータ：data
	template<typename T>
	static void SaveDataMultipleFiles(const std::string &fileName, const std::string &fileFormat, const std::vector<T> &dataList);

	//文字の分割
	//引数
	//文字が格納されている変数(std::string型)：str,区切る文字：delimiter
	static std::vector<std::string> SeparateString(const std::string &str, const char delimiter);
	//-----------------------------------------------------------------------
};