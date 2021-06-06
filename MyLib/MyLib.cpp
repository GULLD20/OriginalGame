#include "../pch.h"

#include "MyLib.h"

//3D用-------------------------------------------------------------------

//移動用
//引数
//自分の位置：posと進む距離：vec
DirectX::SimpleMath::Vector3 MyLib::Move(const DirectX::SimpleMath::Vector3 &pos, const DirectX::SimpleMath::Vector3 &vec)
{
	DirectX::SimpleMath::Vector3 movingPos;
	movingPos = pos + vec;

	return movingPos;
}

//相手を発見(視線)
//引数
//自分の球：thisSphere,相手の球：targetSphere,向いている方向のベクトル：forwardVec,視覚(見える範囲)：visual
bool MyLib::Discovery(const Collision::Sphere3D &thisSphere, const  Collision::Sphere3D &targetSphere, const  DirectX::SimpleMath::Vector3 &forwardVec, float visual)
{
	//プレイヤーが敵の索敵範囲に入った時
	if (Collision::HitCheckoxSphere3D(thisSphere, targetSphere))
	{
		//正面に向けたベクトル
		DirectX::SimpleMath::Vector3 forward = forwardVec;
		//自分(this)からターゲット(プレイヤー)に向けたベクトル
		DirectX::SimpleMath::Vector3 enemy_target_Vec = targetSphere.center - thisSphere.center;

		//正規化理由
		//計算が楽
		//大きさが関係なくなる
		//正規化
		forward.Normalize();
		enemy_target_Vec.Normalize();

		//二つのベクトルの内積を求める
		float dot = enemy_target_Vec.Dot(forwardVec);

		//求めた内積のなす角を求める
		//正規化してない場合
		//√を使うため重くなる
		float angle = std::acos(dot);

		//視界の範囲内か判定
		if (angle <= visual && angle >= -visual)
		{
			return true;
		}
	}

	return false;
}

//ターゲットのほうに向く
//引数
//自分の位置：thisPos,相手の位置：targetPos,正面に向けたベクトル：frontVec
DirectX::SimpleMath::Matrix MyLib::LockAt(const DirectX::SimpleMath::Vector3 &thisPos, const  DirectX::SimpleMath::Vector3 &targetPos, const DirectX::SimpleMath::Vector3 &frontVec)
{
	//自分から対象に向けたベクトル
	DirectX::SimpleMath::Vector3 my_target_Vector = targetPos - thisPos;

	//自分から対象に向けたベクトルの正規化
	my_target_Vector.Normalize();

	//二つのベクトルの内積を求める
	float dot = my_target_Vector.Dot(frontVec);
	//求めた内積のなす角を求める
	float angle = std::acos(dot);

	//二つのベクトルの外積を求める
	DirectX::SimpleMath::Vector3 cros = my_target_Vector.Cross(frontVec);

	//外積が0の時(二つのベクトルが重なっているとき)外積の値をDirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f)で固定する
	if (cros.Length() == 0.0f) cros = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);

	//求めた外積と内積をQuaternionに変換
	DirectX::SimpleMath::Quaternion qua = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(-cros, angle);

	//変換したQuaternionを使って回転行列を作成
	DirectX::SimpleMath::Matrix Rotation = DirectX::SimpleMath::Matrix::Transform(DirectX::SimpleMath::Matrix::Identity, qua);

	return Rotation;
}

//カメラの描画範囲内(四角)
//引数
//カメラを中心とした(もしくはプレイヤーキャラ)四角：thisBox,対象の四角：targetBox
bool MyLib::CameraWithInDrawRange(const Collision::Box3D &thisBox, const Collision::Box3D &targetBox)
{
	//描画範囲内
	if (Collision::HitCheckoxBox3D(thisBox, targetBox))
	{
		return true;
	}

	return false;
}
//-----------------------------------------------------------------------

//2D---------------------------------------------------------------------

//-----------------------------------------------------------------------

//その他-----------------------------------------------------------------
//ラープ関数
float MyLib::Lerp(float start, float end, float t)
{
	float c = (end - start) * t + start;
	return c;
}

//乱数を取得する関数
//乱数の最小値：minimumNum、乱数の最大値：maxNum
//int型用
int MyLib::MersenneTwisterI(const int &minimumNum, const int &maxNum)
{
	//乱数を返す関数(メルセンヌ・ツイスタ法)

	//ランダムな値を入手(これだけだと実行速度が遅い)
	std::random_device rd;
	std::mt19937 mt(rd());
	//整数
	std::uniform_int_distribution<int> mtRand(minimumNum, maxNum);

	return mtRand(mt);
}

//float型用
float MyLib::MersenneTwisterF(const float &minimumNum, const float &maxNum)
{
	//乱数を返す関数(メルセンヌ・ツイスタ法)

	//ランダムな値を入手(これだけだと実行速度が遅い)
	std::random_device rd;
	std::mt19937 mt(rd());
	//小数点
	std::uniform_real_distribution<float> mtRand(minimumNum, maxNum);

	return mtRand(mt);
}

//テキスト形式のファイルの読み込み
//引数
//ファイルの位置 + ファイルの名前：fileName
std::vector<std::string> MyLib::LoadFail_TextForm(const char* fileName)
{
	//テキスト系のファイルのデータを格納する
	std::vector<std::string> csvData;
	//文字を一時的に格納する
	std::string temporary;
	//テキスト系のファイル自体を格納
	std::ifstream ifs;

	//ファイルの読み込み
	ifs.open(fileName);

	//開けていないとき
	if (!ifs.is_open())
		return csvData;

	//読み込んだファイルのデータを1行ずつ保存
	while (getline(ifs, temporary))
	{
		//データが入っているか確認
		if (!temporary.empty())
		{
			csvData.push_back(temporary);
		}
	}

	return csvData;
}

//テキスト系のデータの保存
//引数
//ファイルの位置 + ファイルの名前：fileName , 保存するデータ：data
template<typename T>
void MyLib::SaveData(const char* fileName, const std::vector<T> &dataList)
{
	std::ofstream ofs(fileName);

	for (int i = 0; i < dataList.size(); i++)
	{
		ofs << dataList[i] << std::endl;
	}
}

//データを複数のファイルに保存する(呼び出すたびにファイルが増える)
//引数
//ファイルの位置 + ファイルの名前：fileName , ファイル形式：fileFormat , 保存するデータ：data
template<typename T>
void MyLib::SaveDataMultipleFiles(const std::string &fileName, const std::string &fileFormat, const std::vector<T> &dataList)
{
	std::string dot = ".";
	//何個目のファイルか指定するデータを入れる
	std::string num;
	//ファイルを操作するために必要
	std::ofstream ofs;
	//何個目のファイルか指定する
	int i = 0;
	//ファイルがあるか判断する
	bool open = true;

	while(open)
	{
		//int型のデータをstd::string型に変換する
		num = std::to_string(i);
		//検索するファイル名を決める
		std::string name1 = fileName + num + dot + fileFormat;
		//ファイルを開く
		std::ifstream ifs(name1);
		//ファイルが開いているか確認
		open = ifs.is_open();
		//ファイルが開いていなかったら(存在していなかったら)
		if(!open)
		{
			//ファイルの作成
			ofs.open(name1);
			break;
		}
		i++;
	}

	//データの保存
	for(int i = 0; i < dataList.size(); i++)
	{
		ofs << dataList[i] << std::endl;
	}
}

//文字の分割
//引数
//文字が格納されている変数(std::string型)：str,区切る文字：delimiter
std::vector<std::string> MyLib::SeparateString(const std::string &str, const char delimiter)
{
	//分割した文字を格納する変数
	std::vector<std::string> elems;
	//受け取った文字を格納する変数(getlineで使うため)
	std::stringstream ss(str);
	//分割した文字を一時的に格納する変数
	std::string temporary;

	//分割できなくなるまで繰り返す
	while (getline(ss, temporary, delimiter))
	{
		//データが入っているなら
		if (!temporary.empty())
		{
			elems.push_back(temporary);
		}
	}
	return elems;
}