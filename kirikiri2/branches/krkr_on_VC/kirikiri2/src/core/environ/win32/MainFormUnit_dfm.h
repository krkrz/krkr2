#pragma once

static void init(TTVPMainForm* p)
{
	_create_VCL_obj( &p->VerySmallIconImageList      , p );
	_create_VCL_obj( &p->SmallIconImageList          , p );
	_create_VCL_obj( &p->ToolBar                     , p );
	_create_VCL_obj( &p->ShowScriptEditorButton      , p );
	_create_VCL_obj( &p->ShowConsoleButton           , p );
	_create_VCL_obj( &p->ToolButton3                 , p );
	_create_VCL_obj( &p->ExitButton                  , p );
	_create_VCL_obj( &p->EventButton                 , p );
	_create_VCL_obj( &p->PopupMenu                   , p );
	_create_VCL_obj( &p->ShowScriptEditorMenuItem    , p );
	_create_VCL_obj( &p->ShowConsoleMenuItem         , p );
	_create_VCL_obj( &p->N1                          , p );
	_create_VCL_obj( &p->EnableEventMenuItem         , p );
	_create_VCL_obj( &p->N2                          , p );
	_create_VCL_obj( &p->ExitMenuItem                , p );
	_create_VCL_obj( &p->ShowAboutMenuItem           , p );
	_create_VCL_obj( &p->N3                          , p );
	_create_VCL_obj( &p->DumpMenuItem                , p );
	_create_VCL_obj( &p->ToolButton1                 , p );
	_create_VCL_obj( &p->ShowWatchButton             , p );
	_create_VCL_obj( &p->ShowWatchMenuItem           , p );
	_create_VCL_obj( &p->SystemWatchTimer            , p );
	_create_VCL_obj( &p->CopyImportantLogMenuItem    , p );
	_create_VCL_obj( &p->CreateMessageMapFileMenuItem, p );
	_create_VCL_obj( &p->MessageMapFileSaveDialog    , p );
	_create_VCL_obj( &p->ShowOnTopMenuItem           , p );
	_create_VCL_obj( &p->N4                          , p );
	_create_VCL_obj( &p->ShowControllerMenuItem      , p );
	_create_VCL_obj( &p->RestartScriptEngineMenuItem , p );

	p->OnClose =
		boost::bind(boost::mem_fn(&TTVPMainForm::FormClose), p, _1, _2);
	p->OnDestroy =
		boost::bind(boost::mem_fn(&TTVPMainForm::FormDestroy), p, _1);

	p->EventButton->Down = true;
}

