//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Definition of Messages and Message Related Utilities
//---------------------------------------------------------------------------
// 2006/7/2
//     TVPCannnotLinkPluginWhilePluginLinking added.
//---------------------------------------------------------------------------
#ifndef MsgImplH
#define MsgImplH

#include "tjsMessage.h"

#ifndef TVP_MSG_DECL
	#define TVP_MSG_DECL(name, msg) extern tTJSMessageHolder name;
#endif

//---------------------------------------------------------------------------
// Message Strings ( these should be localized )
//---------------------------------------------------------------------------
// Japanese localized messages
TVP_MSG_DECL(TVPScriptExceptionRaised,
	"スクリプトで例外が発生しました");

TVP_MSG_DECL(TVPHardwareExceptionRaised,
	"ハードウェア例外が発生しました");

TVP_MSG_DECL(TVPMainCDPName,
	"スクリプトエディタ (メイン)");

TVP_MSG_DECL(TVPExceptionCDPName,
	"スクリプトエディタ (例外通知)");

TVP_MSG_DECL(TVPCannnotLocateUIDLLForFolderSelection,
	"フォルダ/アーカイブの選択画面を表示しようとしましたが "
		"krdevui.dll が見つからないので表示できません.\n"
		"実行するフォルダ/アーカイブはコマンドラインの引数として指定してください");

TVP_MSG_DECL(TVPInvalidUIDLL,
	"krdevui.dll が異常か、バージョンが一致しません");

TVP_MSG_DECL(TVPInvalidBPP,
	"無効な色深度です");

TVP_MSG_DECL(TVPCannotLoadPlugin,
	"指定されたプラグイン %1 を読み込めません");

TVP_MSG_DECL(TVPNotValidPlugin,
	"%1 は有効なプラグインではありません");

TVP_MSG_DECL(TVPPluginUninitFailed,
	"プラグインの解放に失敗しました");

TVP_MSG_DECL(TVPCannnotLinkPluginWhilePluginLinking,
	"プラグインの接続中に他のプラグインを接続することはできまません");

TVP_MSG_DECL(TVPNotSusiePlugin,
	"異常な Susie プラグインです");

TVP_MSG_DECL(TVPSusiePluginError,
	"Susie プラグインでエラーが発生しました/エラーコード %1");

TVP_MSG_DECL(TVPCannotReleasePlugin,
	"指定されたプラグインは使用中のため解放できません");

TVP_MSG_DECL(TVPNotLoadedPlugin,
	"%1 は読み込まれていません");

TVP_MSG_DECL(TVPCannotAllocateBitmapBits,
	"ビットマップ用メモリを確保できません/%1(size=%2)");

TVP_MSG_DECL(TVPScanLineRangeOver,
	"スキャンライン %1 は範囲(0～%2)を超えています");

TVP_MSG_DECL(TVPPluginError,
	"プラグインでエラーが発生しました/%1");

TVP_MSG_DECL(TVPInvalidCDDADrive,
	"指定されたドライブでは CD-DA を再生できません");

TVP_MSG_DECL(TVPCDDADriveNotFound,
	"CD-DA を再生できるドライブが見つかりません");

TVP_MSG_DECL(TVPMCIError,
	"MCI でエラーが発生しました : %1");

TVP_MSG_DECL(TVPInvalidSMF,
	"有効な SMF ファイルではありません : %1");

TVP_MSG_DECL(TVPMalformedMIDIMessage,
	"指定されたメッセージは MIDI メッセージとして有効な形式ではありません");

TVP_MSG_DECL(TVPCannotInitDirectSound,
	"DirectSound を初期化できません : %1");

TVP_MSG_DECL(TVPCannotCreateDSSecondaryBuffer,
	"DirectSound セカンダリバッファを作成できません : %1/%2");

TVP_MSG_DECL(TVPInvalidLoopInformation,
	"ループ情報 %1 は異常です");

TVP_MSG_DECL(TVPNotChildMenuItem,
	"指定されたメニュー項目はこのメニュー項目の子ではありません");

TVP_MSG_DECL(TVPCannotInitDirectDraw,
	"DirectDraw を初期化できません : %1");

TVP_MSG_DECL(TVPCannotFindDisplayMode,
	"適合する画面モードが見つかりません : %1");

TVP_MSG_DECL(TVPCannotSwitchToFullScreen,
	"フルスクリーンに切り替えられません : %1");

TVP_MSG_DECL(TVPInvalidPropertyInFullScreen,
	"フルスクリーン中では操作できないプロパティを設定しようとしました");

TVP_MSG_DECL(TVPInvalidMethodInFullScreen,
	"フルスクリーン中では操作できないメソッドを呼び出そうとしました");

TVP_MSG_DECL(TVPCannotLoadCursor,
	"マウスカーソル %1 の読み込みに失敗しました");

TVP_MSG_DECL(TVPCannotLoadKrMovieDLL,
	"ビデオ/Shockwave Flash を再生するためには krmovie.dll / krflash.dll "
		"が必要ですが 読み込むことができません");

TVP_MSG_DECL(TVPInvalidKrMovieDLL,
	"krmovie.dll/krflash.dll が異常か 対応できないバージョンです");

TVP_MSG_DECL(TVPErrorInKrMovieDLL,
	"krmovie.dll/krflash.dll 内でエラーが発生しました/%1");

TVP_MSG_DECL(TVPWindowAlreadyMissing,
	"ウィンドウはすでに存在していません");

TVP_MSG_DECL(TVPPrerenderedFontMappingFailed,
	"レンダリング済みフォントのマッピングに失敗しました : %1");

TVP_MSG_DECL_CONST(TVPConfigFailOriginalFileCannotBeRewritten,
	"%1 に書き込みできません。ソフトウェアが実行中のままになっていないか、あるいは"
		"書き込み権限があるかどうかを確認してください");

TVP_MSG_DECL(TVPConfigFailTempExeNotErased,
	"%1 の終了を確認できないため、これを削除できませんでした(このファイルは削除して結構です)");

TVP_MSG_DECL_CONST(TVPExecutionFail,
	"%1 を実行できません");

TVP_MSG_DECL(TVPPluginUnboundFunctionError,
	"プラグインから関数 %1 を要求されましたが、その関数は本体内に存在しません。プラグインと"
			"本体のバージョンが正しく対応しているか確認してください");

TVP_MSG_DECL(TVPExceptionHadBeenOccured,
	" = (例外発生)");

TVP_MSG_DECL(TVPConsoleResult,
	"コンソール : ");


//---------------------------------------------------------------------------



#endif
