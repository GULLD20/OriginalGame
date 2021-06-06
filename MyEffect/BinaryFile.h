/// <summary>
/// バイナリファイルを扱うクラス
/// </summary>
#pragma once

#include <memory>

class BinaryFile
{
public:
	// ファイル名を指定してロード
	static BinaryFile LoadFile(const wchar_t* fileName);

	BinaryFile();
	// ムーブコンストラクタ
	BinaryFile(BinaryFile&& in);

	// アクセッサ
	char* GetData() const { return m_dataList.get(); }
	unsigned int GetSize()const { return m_size; }

protected:
	// データ
	std::unique_ptr<char[]> m_dataList;
	// サイズ
	unsigned int            m_size;
};
