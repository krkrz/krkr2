Author: 渡邊剛(合資会社ワムソフト) 
Date: 2009/4/13

●概要

squirrel で疑似スレッド処理を実現するライブラリです。

●使い方

詳細は manual.nut を参照してください

●組み込み

処理を組み込むためには、自分のシステムにあわせた
ラッパー処理を一部独自実装する必要があります。

◇非同期ファイルロードの実装

ファイルを非同期に読み込む処理を行うために以下のメソッドを実装してください。
※ squirrel 標準機構の sqstd_loadfile 他は全く使ってません

-----------------------------------------------------------------------------
/**
 * ファイルを非同期に開く
 * @param filename スクリプトファイル名
 * @return ファイルハンドラ
 */
extern void *sqobjOpenFile(const SQChar *filename);

/**
 * ファイルが開かれたかどうかのチェック
 * @param handler ファイルハンドラ
 * @param dataPtr データ格納先アドレス(出力)
 * @param dataSize データサイズ(出力)
 * @return ロード完了していたら true
 */
extern bool sqobjCheckFile(void *handler, const char **dataAddr, int *dataSize);

/**
 * ファイルを閉じる
 * @param handler ファイルハンドラ
 */
extern void sqobjCloseFile(void *handler);
----------------------------------------------------------------------------

◇グローバルVM関係メソッドの実装

SQObject や SQThread は、特定のグローバル Squirrel VM に依存します。
これを取得するための以下のメソッドを実装します

----------------------------------------------------------
namespace sqobject{
  extern HSQUIRRELVM　init();        /// < VM初期化
  extern void done();                /// < VM破棄
  extern HSQUIRRELVM　getGlobalVM(); /// < グローバルVM取得
}
----------------------------------------------------------

◇オブジェクト参照処理の実装

ObjectInfo のメンバ obj から Object や Thread の
ネイティブ C++ インスタンスを取得する以下のメソッドを実装します。

-----------------------------------------------
namespace sqobject{
  Thread *ObjectInfo::getThread();
  Object *ObjectInfo::getObject();
}
-----------------------------------------------

◇オブジェクト登録処理の実装

オブジェクトをクラスとして登録するための以下のメソッドを実装します。
プログラムはこれと、Thread::registGlobal() を呼び出すことで
メソッドを登録できます

-----------------------------------------------
namespace sqobject{
  void Object::registerClass();
  void Thread::registerClass();
}
-----------------------------------------------

◇独自オブジェクトの実装と登録

sqobject::Object を単一継承する形でオブジェクトを作成してください。
そうすることで、Object クラスに拡張されている、プロパティやデルゲートの
機能を使うことができるほか、疑似スレッドの wait 対象としてオブジェクト
を扱うことができます。

※オブジェクトの継承処理は独自に実装する必要があります

◇実行処理の実装

■初期化

1. sqobject::init() を呼び出す
2. print関数登録他必要な処理
3. クラス登録

  Object::registerClass();
  Thread::registerClass();
  Thread::registerGlobal();

  他必要に応じてクラスを登録

■実行処理実装

疑似スレッドを稼働させるにはメインループ中で以下の処理を呼び出してください。

-----------------------------------------------
/**
 * @param diff 経過時間
 * @return 動作中のスレッドの数
 */	
int Thread::main(int diff);
-----------------------------------------------

基本構造は次のようにするのが妥当です。

while(true) {
  イベント処理
　Thread::main(時間差分)
　画面更新処理
};

時間の概念はシステム側で任意に選択できます。
一般的にはフレーム数か、ms 指定を使います。これで指定した値が
wait() 命令に渡す数値パラメータの意味になります。

■終了処理の実装

1. Thread::done() でスレッドの情報を教師得破棄
2. roottable の情報を全クリア
3. sqobject::done() 呼び出して VM を解放

◇実装サンプルコード

 sqfunc.cpp     シンプルな継承/メンバ関数処理の実装例
 sqplusfunc.cpp	SQPlusを使う場合の実装例

●スクリプトの呼び出し

C++側からスクリプトを起動する場合は、Thread::fork() を使うか、
squirrel の API を使って、グローバル関数 fork() を呼び出すようにしてください。

sqplus(改)での例
---------------------------------------------------------------
SqPlus::SquirrelFunction<int>("fork")(NULL, _SC("file.nut"));
---------------------------------------------------------------

※スレッド実行時の注意

個別のスクリプトは、待ち状態になるまで実行を中断しないため、
容易に busy loop になります。定期的に wait() または suspend() 
をいれてシステムに処理を戻すようにする必要があります。
原則としては、画面更新が必要なタイミングで wait() を
行えば良いことになります。

●ライセンス

squirrel 同様 zlibライセンスに従って利用してください。

/*
 * copyright (c)2009 http://wamsoft.jp
 * zlib license
 */
