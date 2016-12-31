Title: saveStruct Plugin
Author: わたなべごう, miahmie

●これはなに？

Array/Dictionary の saveStruct の処理を、
Unicode ではなく、現在のコードページまたはUTF-8 で出力可能にするものです

toStructStringで結果を文字列で取得することもできます

●使用方法

manual.tjs 参照

新たに整形用オプション値が追加されました。

・ssoIndent : 辞書・配列の階層をインデントでわかりやすく出力します
・ssoConst  : 辞書・配列に(const)をつけて出力します
・ssoSort   : 辞書のキーをソートして出力します
・ssoHidden : 隠しメンバも出力します

過去バージョンの互換のため（Scripts.toStructStringを除き）
ssoConst/ssoIndentはデフォルトでは指定されませんので注意してください。

ssoHiddenは隠しメンバも取得するようになりますが、隠しメンバの機能自体、
simplebinderを使った一部のプラグイン等でしか使用されていないようです。
（用途としては殆ど使い道がないかと思われます）


●その他

{Array/Dictionary}.toStructStringのnewlineオプションの
デフォルト値が変更されています。(\r\n ⇒ \n)
直後にsplitなどで行分割処理を行っている場合は
動作互換に影響がでますので注意してください。


Array/Dictionary の saveStructと違い、
辞書／配列以外のオブジェクトの内容も保存対象になります。
（EnumMembersに対応してないオブジェクトは保存できません）
ただし、結果の内容は常に辞書として記入されるため、
関数やプロパティ等も%[]として保存されることになります。

例えば

(Dictionary.saveStruct2 incontextof global)("global.tjs",,, ssoIndent|ssoSort);

とすると、globalからのオブジェクトツリーを見ることができます。
（Arrayクラスだけは例外的に[]で保存されてしまいますが仕様です）


Scripts.toStructStringでオブジェクト以外のプリミティブな値も文字列化できます。
（文字列を渡すと""で囲まれてエスケープされたものが返ります）
こちらのメソッドはoptionのデフォルト値が
Array/Dictioanaryのそれと違うので注意してください。

使用例：Debug.message("any_variables="+Scripts.toStructString(any_variables));


●ライセンス

ライセンスは吉里吉里本体に準拠してください。
