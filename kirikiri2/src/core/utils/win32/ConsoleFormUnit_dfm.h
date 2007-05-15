#pragma once

static void init(TTVPConsoleForm* p)
{
	_create_VCL_obj( &p->StatusBar               , p );
	_create_VCL_obj( &p->PopupMenu               , p );
	_create_VCL_obj( &p->CopyMenuItem            , p );
	_create_VCL_obj( &p->N1                      , p );
	_create_VCL_obj( &p->ShowScriptEditorMenuItem, p );
	_create_VCL_obj( &p->ShowAboutMenuItem       , p );
	_create_VCL_obj( &p->ShowControllerMenuItem  , p );
	_create_VCL_obj( &p->ShowWatchMenuItem       , p );
	_create_VCL_obj( &p->ExprComboBox            , p );
	_create_VCL_obj( &p->ToolBar                 , p );
	_create_VCL_obj( &p->ExecButton              , p );
	_create_VCL_obj( &p->LogPanel                , p );
	_create_VCL_obj( &p->SelectAllMenuItem       , p );
	_create_VCL_obj( &p->CopyImportantLogMenuItem, p );
	_create_VCL_obj( &p->N2                      , p );
	_create_VCL_obj( &p->AutoShowOnErrorMenuItem , p );
	_create_VCL_obj( &p->N3                      , p );
	_create_VCL_obj( &p->ShowOnTopMenuItem       , p );
	_create_VCL_obj( &p->FontDialog              , p );
	_create_VCL_obj( &p->SelectFontMenuItem      , p );
	_create_VCL_obj( &p->ShowConsoleMenuItem     , p );
}
