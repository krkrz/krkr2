
#include "Screen.h"

// main関数なしに強引に何かをさせるトリック
TScreen* Screen;
static __THogeInstance<TScreen, &Screen> __TScreenInstance;

// アプリケーション実行時のスクリーンの状態を表すオブジェクト
/*
class TScreen
{
private:
	std::vector<TForm*> m_Forms;
	std::vector<HCURSOR> m_Cursors;
public:
	PROPERTY_R(int, Width);
	PROPERTY_R(int, Height);
	PROPERTY_R(int, FormCount);
	PROPERTY(TCursor, Cursor);

	PROPERTY_ARY_R(TForm*, Forms);
	PROPERTY_ARY(HCURSOR, Cursors);
};
*/

//----------------------------------------------------------------------------
//	PROPERTY_R(int, Width);
// スクリーンの幅を返す
int TScreen::getWidth() const
{
	return wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
}

//----------------------------------------------------------------------------
//	PROPERTY_R(int, Height);
// スクリーンの高さを返す
int TScreen::getHeight() const
{
	return wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
}

//----------------------------------------------------------------------------
//	PROPERTY(int, FormCount);
int TScreen::getFormCount() const
{
	return m_Forms.size();
}
//----------------------------------------------------------------------------
//	PROPERTY(TCursor, Cursor);
// アプリケーション全体に対するマウスカーソルを設定・取得する

//----------------------------------------------------------------------------
//	PROPERTY_ARY_R(TForm*, Forms);
// アプリケーション内ですでに作成されたすべてのフォームのリストを返す
TForm* TScreen::getForms( int index ) const
{
	return m_Forms[index];
}

//----------------------------------------------------------------------------
//	PROPERTY_ARY(HCURSOR, Cursors);
// アプリケーション内で使用可能なカーソルのリストを設定・取得する
HCURSOR TScreen::getCursors( int index ) const
{
	return m_Cursors[index];
}
void TScreen::setCursors(int index, HCURSOR h)
{
}
