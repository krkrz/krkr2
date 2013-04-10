@loadplugin module="wuvorbis.dll"
@iscript
function stopAllVoices()
{
	// 2 ～ 6 のすべての効果音を停止する
	for(var i = 0; i <= 2; i++) kag.se[i].stop();
}
function playVoice(buf, storage)
{
	// 効果音バッファ buf にて storage を再生する
	// KAG がスキップ処理中の場合は処理を行わない
	if(!kag.skipMode)
	{
		stopAllVoices();
		kag.se[buf].play(%[ storage : storage ]);
	}
}
function createHistoryActionExp(buf, storage)
{
	// メッセージ履歴をクリックしたときに実行する TJS 式を生成する
	return "stopAllVoices(), kag.se[" + buf  +"].play(%[ storage : '" + storage + "' ])";
}
@endscript
@macro name=pv
@hact exp="&createHistoryActionExp(mp.b, mp.s)"
@eval exp="playVoice(mp.b, mp.s)"
@endmacro
@macro name=waitvoices
@ws buf=0
@ws buf=1
@ws buf=2
@endmacro
@macro name=sv
@endhact
@waitvoices cond="kag.autoMode"
@eval exp="stopAllVoices()"
@endmacro
*start|最初
@cm
[pv b=0 s=musicbox]ほげ[l][sv][r]
[pv b=1 s=musicbox]ほげら[l][sv][r]
[pv b=2 s=musicbox]ほげもげ[p][sv]
*start2|次
@cm
[pv b=0 s=musicbox.ogg]ほげ[l][sv][r]
[pv b=1 s=musicbox.ogg]ほげら[l][sv][r]
[pv b=2 s=musicbox.ogg]ほげもげ[p][sv]
[s]

*test
ほげら
[s]

