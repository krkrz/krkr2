●これはなに？

Irrlicht ベースの drawdevice 実装のテストコードです

●コンパイル方法

DirectX9 SDK December 2004 を入れてパスを通しておく

※DirectX8 のヘッダがある最終版
http://www.microsoft.com/downloads/details.aspx?familyid=4E825A37-0C94-4421-9EC8-156E52525D11&displaylang=en

コンパイル時に設定しておく環境変数

IRRLICHT_HOME  irrlicht の SDK が展開されてるフォルダ名

SWF制御用

コンパイルにとりあえず必要なもの

tu-testbed/		http://sourceforge.net/projects/tu-testbed/

適宜コンパイルが通るように要調整

●テスト方法

(1) コンパイルすると drawdeviceirrlicht.dll が
    $(IRRLICHT_HOME)\bin\Win32-VisualStudio に配置される

(2)  $(IRRLICHT_HOME)\bin\Win32-VisualStudio に以下を配置
 
  krkr.exe
  sample → data にリネーム

(3) 該当フォルダで krkr.exe を実行

　※構成プロパティのデバッグに以下を指定すれば良いです

  コマンド        : $(Outdir)\krkr.exe
  作業ディレクトリ: $(Outdir)

●注意点

　TJS 側にデータを読み込む口がまだないため、
　サンプルが強制的に dll 中に埋め込まれてます。
　データは SDK のファイルを相対パスで参照しています。
