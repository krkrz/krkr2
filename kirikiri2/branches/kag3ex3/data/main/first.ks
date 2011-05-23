@loadplugin module=wuvorbis.dll
@loadplugin module=extrans.dll
@loadplugin module=csvparser.dll
@call storage="world.ks"
@call storage="macro.ks"

; 選択肢配置領域の指定
[selopt normal=select_normal over=select_over left=100 top=80 width=600 height=360]

[iscript]

// 全部見たことにするフラグ XXX 最終的に削除すること!!!
//tf.allseen = true;

// クリアフラグ強制ON XXX 最終的に削除すること!!!
sf.clear = true;

// バージョン
sf.software_version = "1.00";

// メーカー
sf.software_maker = "なぞ";

// ソフト名
sf.software_title = "なぞゲーム";

// 年
sf.software_year = "2006";

// ロード対象ページ
sf.loadpage = 1 if sf.loadpage === void;

/**
 * 環境設定初期化処理
 */
function initialEnvironment()
{
    kag.noeffect     = false;
    kag.textspeed    = kag.chSpeeds.normal / (kag.chSpeeds.slow / 10);
    kag.autospeed    = kag.autoModePageWaits.medium / (kag.autoModePageWaits.slow / 10);
    kag.allskip      = false;
    kag.bgmvolume    = 80;
    kag.sevolume     = 100;
    kag.voicevolume  = 100;
    kag.setVoiceOn('voice0', true);
    kag.setVoiceOn('voice1', true);
    kag.setVoiceOn('voice2', true);
    kag.setVoiceOn('voice3', true);
    kag.setVoiceOn('voice4', true);
    kag.setVoiceOn('voice5', true);
}

// 初回起動時は環境を初期化する
if (sf.first_start != 'comp') {
    initialEnvironment();
    sf.first_start = 'comp';
}

[endscript]

[call storage="logo.ks" target="*logo"]
[jump storage=title.ks]
