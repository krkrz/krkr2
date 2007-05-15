#pragma once

#include "vcl_base.h"
#include "Component.h"
#include "Messages.hpp"
#include "Graphics.hpp"
#include <wx/app.h>

//----------------------------------------------------------------------------
#define __classid(x) (&typeid(x))
typedef const type_info TMetaClass;

class TControl;
class TApplication : public TComponent
{
public:
	TApplication( TComponent* Owner=NULL );
	virtual ~TApplication();
	void Initialize( void );
	void Terminate( void );
	void ShowException(Exception*);
	void CreateForm( const type_info *, void *);
	void Run(void);
	void ProcessMessages();
	void HandleMessage();
	void BringToFront();
	void Minimize();
	void Restore();

	int MessageBox(const char*, char*, int);
	int HintMouseMessage(TControl*, TMessage);
	void CancelHint();

	HWND Handle;
	AnsiString Title;

	PROPERTY_VAR0( int, HintHidePause );
	PROPERTY_VAR0( bool, ShowHint );
	PROPERTY_VAR0( bool, ShowMainForm );
	PROPERTY_VAR0( TIcon*, Icon );

	boost::function2<void, System::TObject*, bool &> OnIdle;
	boost::function1<void, System::TObject*> OnActivate;
	boost::function1<void, System::TObject*> OnDeactivate;
	boost::function1<void, System::TObject*> OnMinimize;
	boost::function1<void, System::TObject*> OnRestore;
};
extern TApplication* Application;
