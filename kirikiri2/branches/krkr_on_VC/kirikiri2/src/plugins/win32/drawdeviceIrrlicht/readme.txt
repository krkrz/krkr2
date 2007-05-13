●これはなに？

Irrlicht ベースの drawdevice 実装のテストコードです

●コンパイル方法

コンパイル時に設定しておく環境変数

IRRLICHT_HOME  irrlicht の SDK が展開されてるフォルダ名

その他コンパイル必要なもの（外部ライブラリ）

tu-testbed/		http://sourceforge.net/projects/tu-testbed/
zlib/			http://www.zlib.net/
libjpeg/		http://www.ijg.org/

※適宜コンパイルが通るように要調整
  調整済みのものをコミットするかどうかは考え中

●テスト方法

(0) $(IRRLICHT_HOME)\bin\Win32-VisualStudio に以下を配置
 
  krkr.exe
  sample → data にリネーム

(1) コンパイルすると drawdeviceirrlicht.dll が
    $(IRRLICHT_HOME)\bin\Win32-VisualStudio に配置される

(2) 該当フォルダの krkr.exe を実行

　※構成プロパティのデバッグに以下を指定して「実行」でOK

  コマンド        : $(Outdir)\krkr.exe
  作業ディレクトリ: $(Outdir)

  実行時に必要なもの
　・irrlicht.dll           （Irrlicht のDLL)
  ・drawdeviceirrlicht.dll （吉里吉里プラグイン）

●メモ

ファイルがわかれるのはめんどうなので、
コンパイルしなおしてプラグイン側にリンクしてしまうかも。
ついでに日本語対応化？ 
freetype をつかった日本語化実装の存在は確認
マルチプラットホーム化の必要性はないので、
gdi+ 実装を考慮するのもありかも？
