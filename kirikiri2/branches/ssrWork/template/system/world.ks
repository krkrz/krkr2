@if exp="typeof(global.world_object) == 'undefined'"
@iscript

KAGLoadScript('world.tjs');

kag.addPlugin(global.world_object = new KAGWorldPlugin(kag));
if (kag.debugLevel >= tkdlSimple) {
    dm("ƒ[ƒ‹ƒhŠÂ‹«İ’èŠ®—¹");
}

@endscript
@endif

@return
