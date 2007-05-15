#pragma once

static void init(TTVPWindowForm* p)
{
	_create_VCL_obj( &p->ScrollBox                , p );
	_create_VCL_obj( &p->MainMenu                 , p );
	_create_VCL_obj( &p->PopupMenu                , p );
	_create_VCL_obj( &p->ShowControllerMenuItem   , p );
	_create_VCL_obj( &p->ShowWatchMenuItem        , p );
	_create_VCL_obj( &p->ShowConsoleMenuItem      , p );
	_create_VCL_obj( &p->ShowScriptEditorMenuItem , p );
	_create_VCL_obj( &p->ShowAboutMenuItem        , p );
	_create_VCL_obj( &p->TabMenuItem              , p );
	_create_VCL_obj( &p->ShitTabMenuItem          , p );
	_create_VCL_obj( &p->UpdateRectDebugMenuItem  , p );
	_create_VCL_obj( &p->DumpLayerStructorMenuItem, p );
	_create_VCL_obj( &p->CopyImportantLogMenuItem , p );
	_create_VCL_obj( &p->AltEnterMenuItem         , p );

	p->OnKeyPress =
		boost::bind(boost::mem_fn(&TTVPWindowForm::FormKeyPress), p, _1, _2);
	p->OnKeyDown =
		boost::bind(boost::mem_fn(&TTVPWindowForm::FormKeyDown), p, _1, _2, _3);
	p->OnKeyUp =
		boost::bind(boost::mem_fn(&TTVPWindowForm::FormKeyUp), p, _1, _2, _3);
}
