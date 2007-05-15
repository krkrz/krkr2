#pragma once

static void init(TTVPPadForm* p)
{
	_create_VCL_obj( &p->StatusBar               , p );
	_create_VCL_obj( &p->ToolBar                 , p );
	_create_VCL_obj( &p->ExecuteButton           , p );
	_create_VCL_obj( &p->MemoPopupMenu           , p );
	_create_VCL_obj( &p->CutMenuItem             , p );
	_create_VCL_obj( &p->CopyMenuItem            , p );
	_create_VCL_obj( &p->PasteMenuItem           , p );
	_create_VCL_obj( &p->N1                      , p );
	_create_VCL_obj( &p->ShowConsoleMenuItem     , p );
	_create_VCL_obj( &p->ShowAboutMenuItem       , p );
	_create_VCL_obj( &p->N2                      , p );
	_create_VCL_obj( &p->ExecuteMenuItem         , p );
	_create_VCL_obj( &p->ShowControllerMenuItem  , p );
	_create_VCL_obj( &p->Memo                    , p );
	_create_VCL_obj( &p->ShowWatchMenuItem       , p );
	_create_VCL_obj( &p->N3                      , p );
	_create_VCL_obj( &p->UndoMenuItem            , p );
	_create_VCL_obj( &p->SaveMenuItem            , p );
	_create_VCL_obj( &p->SaveDialog              , p );
	_create_VCL_obj( &p->CopyImportantLogMenuItem, p );
	_create_VCL_obj( &p->N4                      , p );
	_create_VCL_obj( &p->ShowOnTopMenuItem       , p );
	_create_VCL_obj( &p->ShowScriptEditorMenuItem, p );
}
