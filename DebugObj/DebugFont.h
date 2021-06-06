//======================================================
// File Name : DebugFont.h
// Summary : デバッグフォント
// Date : 2019/4/18
// Author : Takafumi Ban
//======================================================
#pragma once
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
// シングルトンのタイプ
// 0:自身の生ポインタで管理（注：明示的な解放をしないとメモリリークする）
// 1:自身のユニークポインタで管理（注：ユニークポインタの理解推奨）
// 2:静的な実態で管理（注：プログラム終了のタイミングでしか解放できない）
#define DEBUG_FONT_SINGLETON_TYPE 1
class DebugFont
{
public:
	// 文字列最大数サイズ
	static const size_t STRING_SIZE_MAX = 256;
private:
	DebugFont();
public:
	~DebugFont();
public:
	// 生成
	void create(ID3D11Device1* device, ID3D11DeviceContext1* context);
	// 描画
	void draw();
	// リセット
	void reset();
	// 表示情報登録
	void print(float posX, float posY, wchar_t const* const format, ...);
	void print(DirectX::SimpleMath::Vector2 pos, wchar_t const* const format, ...);
#if DEBUG_FONT_SINGLETON_TYPE == 0
public:
	// オブジェクトへのポインタを返す
	static DebugFont* GetInstance()
	{
		if (m_pInstance == nullptr)
		{
			// オブジェクトを生成し、m_instance変数に格納する
			m_pInstance = new DebugFont();
		}
		// オブジェクトへのポインタを返す
		return m_pInstance;
	}
	// オブジェクトへの参照を返す
	static DebugFont& getRefInstance()
	{
		if (m_pInstance == nullptr)
		{
			// オブジェクトを生成し、m_instance変数に格納する
			m_pInstance = new DebugFont();
		}
		// オブジェクトへのポインタを返す
		return *m_pInstance;
	}
	// オブジェクトを破棄する
	static void Dispose()
	{
		if (m_pInstance != nullptr)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}
private:
	static DebugFont* m_pInstance;
#elif DEBUG_FONT_SINGLETON_TYPE == 1
public:
	// オブジェクトへのポインタを返す
	static DebugFont* GetInstance()
	{
		if (m_pInstance.get() == nullptr)
		{
			// オブジェクトを生成し、ポインタをm_pInstance変数に格納する
			m_pInstance.reset(new DebugFont());
		}
		// オブジェクトへのポインタを返す
		return m_pInstance.get();
	}
	// オブジェクトを破棄する
	static void Dispose()
	{
		if (m_pInstance)
			m_pInstance.reset();
	}
private:
	// オブジェクトへのポインタ
	static std::unique_ptr<DebugFont> m_pInstance;
#else
public:
	// オブジェクトへのポインタを返す
	static DebugFont* GetInstance()
	{
		// オブジェクトへのポインタを返す
		return &m_instance;
	}
	// オブジェクトの参照を返す
	static DebugFont& getRefInstance()
	{
		// オブジェクトへのポインタを返す
		return m_instance;
	}
private:
	// オブジェクト
	static DebugFont m_instance;
#endif
private:
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	std::unique_ptr<DirectX::SpriteFont>  m_spriteFont;
	wchar_t*                              m_string;
	DirectX::SimpleMath::Vector2          m_pos;
};
