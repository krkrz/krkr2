//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Definition of Messages and Message Related Utilities
//---------------------------------------------------------------------------
#ifndef MsgIntfH
#define MsgIntfH

#include "tjs.h"
#include "tjsMessage.h"

#ifndef TVP_MSG_DECL
	#define TVP_MSG_DECL(name, msg) extern tTJSMessageHolder name;
	#define TVP_MSG_DECL_CONST(name, msg) extern tTJSMessageHolder name;
#endif

#include "MsgImpl.h"


//---------------------------------------------------------------------------
// Message Strings ( these should be localized )
//---------------------------------------------------------------------------
// Japanese localized messages
TVP_MSG_DECL_CONST(TVPAboutString,
	
"吉里吉里[きりきり] 2 実行コア version %1 ( TJS version %2 )\n"
"Compiled on " __DATE__ " " __TIME__ "\n"
"Copyright (C) 1997-2004 W.Dee All rights reserved.\n"
"吉里吉里実行コアの使用/配布/改変は、\n"
"SDK 付属の license.txt に書かれているライセンスに従って行うことができます.\n"
"------------------------------------------------------------------------------\n"
"Thanks for many libraries, contributers and supporters not listible here.\n"
"This software is based in part on the work of Independent JPEG Group.\n"
"Regex++ Copyright (c) 1998-2003 Dr John Maddock\n"
"ERINA-Library Copyright (C) 2001 Leshade Entis, Entis-soft.\n"
"- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n"
"Using \"A C-program for MT19937\"\n"
"\n"
"   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,\n"
"   All rights reserved.\n"
"\n"
"   Redistribution and use in source and binary forms, with or without\n"
"   modification, are permitted provided that the following conditions\n"
"   are met:\n"
"\n"
"     1. Redistributions of source code must retain the above copyright\n"
"        notice, this list of conditions and the following disclaimer.\n"
"\n"
"     2. Redistributions in binary form must reproduce the above copyright\n"
"        notice, this list of conditions and the following disclaimer in the\n"
"        documentation and/or other materials provided with the distribution.\n"
"\n"
"     3. The names of its contributors may not be used to endorse or promote\n"
"        products derived from this software without specific prior written\n"
"        permission.\n"
"\n"
"   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS\n"
"   \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT\n"
"   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR\n"
"   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR\n"
"   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,\n"
"   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,\n"
"   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR\n"
"   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF\n"
"   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING\n"
"   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS\n"
"   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n"
"------------------------------------------------------------------------------\n"
"環境情報\n"
  // important log (environment information, Debug.notice etc.) comes here
);

TVP_MSG_DECL_CONST(TVPVersionInformation,
	
"吉里吉里[きりきり] 2 実行コア version %1 [" __DATE__ " " __TIME__ "] ( TJS version %2 ) "
"Copyright (C) 1997-2004 W.Dee All rights reserved.");

TVP_MSG_DECL_CONST(TVPVersionInformation2,
	"バージョン情報の詳細は Ctrl + F12 で閲覧できます");

TVP_MSG_DECL_CONST(TVPDownloadPageURL,
	"http://kikyou.info/tvp/");

TVP_MSG_DECL(TVPInternalError,
	"内部エラーが発生しました: at %1 line %2");

TVP_MSG_DECL(TVPInvalidParam,
	"不正なパラメータです");

TVP_MSG_DECL(TVPCommandLineParamIgnoredAndDefaultUsed,
	"コマンドラインパラメータ %1 に指定された値 %2 は無効のためデフォルトの設定を用います");

TVP_MSG_DECL(TVPInvalidCommandLineParam,
	"コマンドラインパラメータ %1 に指定された値 %2 は無効です");

TVP_MSG_DECL(TVPNotImplemented,
	"未実装の機能を呼び出そうとしました");

TVP_MSG_DECL(TVPCannotOpenStorage,
	"ストレージ %1 を開くことができません");

TVP_MSG_DECL(TVPCannotFindStorage,
	"ストレージ %1 が見つかりません");

TVP_MSG_DECL(TVPCannotOpenStorageForWrite,
	"ストレージ %1 を書き込み用に開くことができません");

TVP_MSG_DECL(TVPStorageInArchiveNotFound,
	"ストレージ %1 がアーカイブ %2 の中に見つかりません");

TVP_MSG_DECL(TVPInvalidPathName,
	"パス名 %1 は無効な形式です");

TVP_MSG_DECL(TVPUnsupportedMediaName,
	"\"%1\" は対応していないメディアタイプです");

TVP_MSG_DECL(TVPCannotUnbindXP3EXE,
	"%1 に結合されたアーカイブを発見できませんでした");

TVP_MSG_DECL(TVPCannotFindXP3Mark,
	"%1 は XP3 アーカイブではないか、対応できない形式です");

TVP_MSG_DECL(TVPMissingPathDelimiterAtLast,
	"パス名の最後には '>' または '/' を指定してください (吉里吉里２ 2.19 beta 14 よりアーカイブの区切り記号が '#' から '>' に変わりました)");

TVP_MSG_DECL(TVPFilenameContainsSharpWarn,
	"(注意) '#' がファイル名 \"%1\" に含まれています。アーカイブの区切り文字は吉里吉里２ 2.19 beta 14 より'#' から '>' に変わりました。"
	"もしアーカイブの区切り文字のつもりで '#' を使用した場合は、お手数ですが '>' に変えてください");

TVP_MSG_DECL(TVPCannotGetLocalName,
	"ストレージ名 %1 をローカルファイル名に変換できません");

TVP_MSG_DECL(TVPReadError,
	"読み込みエラーです");

TVP_MSG_DECL(TVPWriteError,
	"書き込みエラーです");

TVP_MSG_DECL(TVPSeekError,
	"シークに失敗しました");

TVP_MSG_DECL(TVPTruncateError,
	"ファイルの長さを切り詰めるのに失敗しました");

TVP_MSG_DECL(TVPInsufficientMemory,
	"メモリ確保に失敗しました");

TVP_MSG_DECL(TVPUncompressionFailed,
	"ファイルの展開に失敗しました");

TVP_MSG_DECL(TVPCompressionFailed,
	"ファイルの圧縮に失敗しました");

TVP_MSG_DECL(TVPCannotWriteToArchive,
	"アーカイブにデータを書き込むことはできません");

TVP_MSG_DECL(TVPUnsupportedCipherMode,
	"%1 は未対応の暗号化形式か、データが壊れています");

TVP_MSG_DECL(TVPUnsupportedModeString,
	"認識できないモード文字列の指定です(%1)");

TVP_MSG_DECL(TVPUnknownGraphicFormat,
	"%1 は未知の画像形式です");

TVP_MSG_DECL(TVPCannotSuggestGraphicExtension,
	"%1 について適切な拡張子を持ったファイルを見つけられませんでした");

TVP_MSG_DECL(TVPMaskSizeMismatch,
	"マスク画像のサイズがメイン画像のサイズと違います");

TVP_MSG_DECL(TVPProvinceSizeMismatch,
	"領域画像 %1 はメイン画像とサイズが違います");

TVP_MSG_DECL(TVPImageLoadError,
	"画像読み込み中にエラーが発生しました/%1");

TVP_MSG_DECL(TVPJPEGLoadError,
	"JPEG 読み込み中にエラーが発生しました/エラーコード %1");

TVP_MSG_DECL(TVPPNGLoadError,
	"PNG 読み込み中にエラーが発生しました/%1");

TVP_MSG_DECL(TVPERILoadError,
	"ERI 読み込み中にエラーが発生しました/%1");

TVP_MSG_DECL(TVPTLGLoadError,
	"TLG 読み込み中にエラーが発生しました/%1");

TVP_MSG_DECL(TVPInvalidImageSaveType,
	"無効な保存画像形式です(%1)");

TVP_MSG_DECL(TVPInvalidOperationFor8BPP,
	"8bpp 画像に対しては行えない操作を行おうとしました");

TVP_MSG_DECL(TVPSpecifyWindow,
	"Window クラスのオブジェクトを指定してください");

TVP_MSG_DECL(TVPSpecifyLayer,
	"Layer クラスのオブジェクトを指定してください");

TVP_MSG_DECL(TVPCannotCreateEmptyLayerImage,
	"画像サイズの横幅あるいは縦幅を 0 以下の数に設定することはできません");

TVP_MSG_DECL(TVPCannotSetPrimaryInvisible,
	"プライマリレイヤは不可視にできません");

TVP_MSG_DECL(TVPCannotMovePrimary,
	"プライマリレイヤは移動できません");

TVP_MSG_DECL(TVPCannotSetParentSelf,
	"自分自身を親とすることはできません");

TVP_MSG_DECL(TVPCannotMoveNextToSelfOrNotSiblings,
	"自分自身の前後や親の異なるレイヤの前後に移動することはできません");

TVP_MSG_DECL(TVPCannotMovePrimaryOrSiblingless,
	"プライマリレイヤや兄弟の無いレイヤは前後に移動することはできません");

TVP_MSG_DECL(TVPInvalidImagePosition,
	"レイヤ領域に画像の無い領域が発生しました");

TVP_MSG_DECL(TVPCannotSetModeToDisabledOrModal,
	"すでにモーダルなレイヤの親レイヤ、あるいは不可視/無効なレイヤをモーダルにすることはできません");

TVP_MSG_DECL(TVPNotDrawableLayerType,
	"この type のレイヤでは描画や画像読み込みや画像サイズ/位置の変更/取得はできません");

TVP_MSG_DECL(TVPSourceLayerHasNoImage,
	"転送元レイヤは画像を持っていません");

TVP_MSG_DECL(TVPUnsupportedLayerType,
	"%1 はこの type のレイヤでは使用できません");

TVP_MSG_DECL(TVPNotDrawableFaceType,
	"%1 ではこの face に描画できません");

TVP_MSG_DECL(TVPSrcRectOutOfBitmap,
	"転送元がビットマップ外の領域を含んでいます");

TVP_MSG_DECL(TVPCannotChangeFocusInProcessingFocus,
	"フォーカス変更処理中はフォーカスを新たに変更することはできません");

TVP_MSG_DECL(TVPWindowHasNoLayer,
	"ウィンドウにレイヤがありません");

TVP_MSG_DECL(TVPWindowHasAlreadyPrimaryLayer,
	"ウィンドウにはすでにプライマリレイヤがあります");

TVP_MSG_DECL(TVPSpecifiedEventNeedsParameter,
	"イベント %1 にはパラメータが必要です");

TVP_MSG_DECL(TVPSpecifiedEventNeedsParameter2,
	"イベント %1 にはパラメータ %2 が必要です");

TVP_MSG_DECL(TVPSpecifiedEventNameIsUnknown,
	"イベント名 %1 は未知のイベント名です");

TVP_MSG_DECL(TVPOutOfRectangle,
	"矩形外を指定されました");

TVP_MSG_DECL(TVPInvalidMethodInUpdating,
	"画面更新中はこの機能を実行できません");

TVP_MSG_DECL(TVPCannotCreateInstance,
	"このクラスはインスタンスを作成できません");

TVP_MSG_DECL(TVPUnknownWaveFormat,
	"%1 は対応できない Wave 形式です");

TVP_MSG_DECL(TVPSpecifyMenuItem,
	"MenuItem クラスのオブジェクトを指定してください");

TVP_MSG_DECL(TVPCurrentTransitionMustBeStopping,
	"現在のトランジションを停止させてから新しいトランジションを開始してください");

TVP_MSG_DECL(TVPTransHandlerError,
	"トランジションハンドラでエラーが発生しました : %1");

TVP_MSG_DECL(TVPTransAlreadyRegistered,
	"トランジション %1 は既に登録されています");

TVP_MSG_DECL(TVPCannotFindTransHander,
	"トランジションハンドラ %1 が見つかりません");

TVP_MSG_DECL(TVPSpecifyTransitionSource,
	"トランジション元を指定してください");

TVP_MSG_DECL(TVPCannotLoadRuleGraphic,
	"ルール画像 %1 を読み込むことができません");

TVP_MSG_DECL(TVPSpecifyOption,
	"オプション %1 を指定してください");

TVP_MSG_DECL(TVPTransitionLayerSizeMismatch,
	"トランジション元(%1)とトランジション先(%2)のレイヤのサイズが一致しません");

TVP_MSG_DECL(TVPTransitionMutualSource,
	"トランジション元のトランジション元が自分自身です");

TVP_MSG_DECL(TVPKAGNoLine,
	"読み込もうとしたシナリオファイル %1 は空です");

TVP_MSG_DECL(TVPKAGCannotOmmitFirstLabelName,
	"シナリオファイルの最初のラベル名は省略できません");

TVP_MSG_DECL(TVPKAGLabelNotFound,
	"シナリオファイル %1 内にラベル %2 が見つかりません");

TVP_MSG_DECL(TVPKAGInlineScriptNotEnd,
	"[endscript] または @endscript が見つかりません");

TVP_MSG_DECL(TVPKAGSyntaxError,
	"タグの文法エラーです");

TVP_MSG_DECL(TVPKAGMacroEntityNotAvailable,
	"マクロエンティティはマクロ外では使用できません");

TVP_MSG_DECL(TVPKAGCallStackUnderflow,
	"return タグが call タグと対応していません ( return タグが多い )");

TVP_MSG_DECL(TVPKAGReturnLostSync,
	"シナリオファイルに変更があったため return の戻り先位置を特定できません");

TVP_MSG_DECL(TVPKAGSpecifyKAGParser,
	"KAGParser クラスのオブジェクトを指定してください");

TVP_MSG_DECL(TVPKAGMalformedSaveData,
	"栞データが異常です");

TVP_MSG_DECL(TVPLabelOrScriptInMacro,
	"ラベルや iscript はマクロ中に記述できません");

TVP_MSG_DECL(TVPUnknownMacroName,
	"マクロ \"%1\" は登録されていません");

TVP_MSG_DECL(TVPWindowHasAlreadyVideoOverlayObject,
	"ウィンドウはすでにビデオオブジェクトを持っています");

//---------------------------------------------------------------------------
// Utility Functions
//---------------------------------------------------------------------------
TJS_EXP_FUNC_DEF(ttstr, TVPFormatMessage, (const tjs_char *msg, const ttstr & p1));
TJS_EXP_FUNC_DEF(ttstr, TVPFormatMessage, (const tjs_char *msg, const ttstr & p1,
	const ttstr & p2));
TJS_EXP_FUNC_DEF(void, TVPThrowExceptionMessage, (const tjs_char *msg));
TJS_EXP_FUNC_DEF(void, TVPThrowExceptionMessage, (const tjs_char *msg,
	const ttstr &p1, tjs_int num));
TJS_EXP_FUNC_DEF(void, TVPThrowExceptionMessage, (const tjs_char *msg, const ttstr &p1));
TJS_EXP_FUNC_DEF(void, TVPThrowExceptionMessage, (const tjs_char *msg,
	const ttstr & p1, const ttstr & p2));

TJS_EXP_FUNC_DEF(ttstr, TVPGetAboutString, ());
TJS_EXP_FUNC_DEF(ttstr, TVPGetVersionInformation, ());
TJS_EXP_FUNC_DEF(ttstr, TVPGetVersionString, ());

#define TVPThrowInternalError \
	TVPThrowExceptionMessage(TVPInternalError, TJS_W("" __FILE__),  __LINE__)
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// version retrieving
//---------------------------------------------------------------------------
extern tjs_int TVPVersionMajor;
extern tjs_int TVPVersionMinor;
extern tjs_int TVPVersionRelease;
extern tjs_int TVPVersionBuild;
//---------------------------------------------------------------------------
extern void TVPGetVersion();
/*
	implement in each platforms;
	fill these four version field.
*/
//---------------------------------------------------------------------------
TJS_EXP_FUNC_DEF(void, TVPGetSystemVersion, (tjs_int &major, tjs_int &minor,
	tjs_int &release, tjs_int &build));
TJS_EXP_FUNC_DEF(void, TVPGetTJSVersion, (tjs_int &major, tjs_int &minor,
	tjs_int &release));
//---------------------------------------------------------------------------


#endif
