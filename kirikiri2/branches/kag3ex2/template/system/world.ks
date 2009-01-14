@if exp="typeof(global.world_object) == 'undefined'"
@iscript

KAGLoadScript('world.tjs');

kag.addPlugin(global.world_object = new KAGWorldPlugin(kag));
if (kag.debugLevel >= tkdlSimple) {
    dm("ワールド環境設定完了");
}

// 立ち絵表示確認用ウインドウ機能
if (debugWindowEnabled) {
	KAGLoadScript('standview.tjs');
}

@endscript
@endif

@return
