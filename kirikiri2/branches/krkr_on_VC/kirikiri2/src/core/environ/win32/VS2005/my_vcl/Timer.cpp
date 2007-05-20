#include "ExtCtrls.hpp"

class MyTimer : public wxTimer
{
public:
	TTimer * timer;
	MyTimer(TTimer * t) { timer = t; }
	void Notify()
	{
		if ( timer->OnTimer )
			timer->OnTimer(timer);
	}
};

//----------------------------------------------------------------------------
TTimer::TTimer(TComponent* Owner) : TComponent(Owner)
{
	m_wxTimer = new MyTimer(this);
}

//----------------------------------------------------------------------------
TTimer::~TTimer()
{
	delete m_wxTimer;
}

//----------------------------------------------------------------------------
//PROPERTY_VAR1(int, Interval);
// インターバルセット即タイマ起動が仕様上正しいかは分かりませんが、
// krkr.eXeのコードを健全に動作させるには必要です。
void TTimer::setInterval(int v)
{
	m_Interval = v;
	m_wxTimer->Start(m_Interval);
}

//----------------------------------------------------------------------------
//PROPERTY(bool,Enabled);
void TTimer::setEnabled(bool v)
{
	if ( v )
		m_wxTimer->Start(m_Interval);
	else
		m_wxTimer->Stop();

}

bool TTimer::getEnabled() const
{
	return m_wxTimer->IsRunning();
}

//	boost::function1<void, System::TObject*> OnTimer;

