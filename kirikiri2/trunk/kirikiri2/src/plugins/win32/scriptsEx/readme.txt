Title: ScriptsEx Plugin
Author: わたなべごう/ゆんゆん探偵

●これはなに？

iTJSDispatch2 の機能をさわるための裏口プラグインです

●使用方法

manual.tjs 参照


●Scripts.propSet/Getについて

iTJSDispatch2のPropSet/Getを生で呼ぶ関数となります。
フラグ指定に注意してください。(propSet時のpfMemberEnsureの指定忘れなど)

使用例：
Plugins.link("ScriptsEx.dll");
with (Scripts) {
	.propSet(Dictionary, /*member*/"testMember", /*value*/1,
		.pfStaticMember|.pfMemberEnsure); // static指定
}
var test = new Dictionary();
Debug.message(typeof test.testMember); // -> "undefined"
Debug.message(typeof Dictionary.testMember); // -> "Integer"



●ライセンス

ライセンスは吉里吉里本体に準拠してください。
