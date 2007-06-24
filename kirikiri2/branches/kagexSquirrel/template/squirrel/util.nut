//
// ユーティリティ定義
// 

/**
 * グローバル関数 replace
 * 文字列の部分置換を行います
 * @param str
 * @param from
 * @param to
 */
//function replace(str,from,to)

/**
 * グローバル関数 iskanji
 * 漢字の1文字目かどうか判定します
 * @param ch 文字
 */
//function iskanji(ch)

/**
 * メッセージ表示
 */
function dm(msg)
{
	print(msg + "\n");
}

/**
 * 評価処理
 * @param eval 条件式
 * @return 判定結果
 */
function eval(eval) {
	local script = "return " + eval;
	return compilestring(script)();
}

/**
 * テーブルから name で示される要素を取り出す
 * @param table 処理対象のテーブル
 * @param name  項目の名前
 * @parem デフォルト値
 */
function getval(table, name, ...) 
{
	if (name in table) {
		return table[name];
	} else if (vargc > 0) {
		return vargv[0];
	}
}

/**
 * テーブルから name で示される要素を数値またはnull としてとりだす
 * @param table 処理対象のテーブル
 * @param name  項目の名前
 */
function getint(table, name, ...) 
{
	if (name in table) {
		return table[name].tointeger();
	} else if (vargc > 0) {
		return vargv[0];
	}
}

/**
 * 値から数値をとりだす。元が null の場合はそのまま
 * @param value 処理対象の値
 */
function toint(value, ...)
{
	if (value != null) {
		return value.tointeger();
	} else if (vargc > 0) {
		return vargv[0];
	}
}

/**
 * テーブルから name で示される要素を数値またはnull としてとりだす
 * @param table 処理対象のテーブル
 * @param name  項目の名前
 */
function getfloat(table, name, ...) 
{
	if (name in table) {
		return table[name].tofloat();
	} else if (vargc > 0) {
		return vargv[0];
	}
}

/**
 * 値から数値をとりだす。元が null の場合はそのまま
 * @param value 処理対象の値
 */
function tofloat(value, ...)
{
	if (value != null) {
		return value.tofloat();
	} else if (vargc > 0) {
		return vargv[0];
	}
}

/**
 * 乱数生成
 */
function random(a)
{
	local d = rand().tofloat() / RAND_MAX * a;
    return  d.tointeger() ;
}

/**
 * 色変換処理
 */
function colorConv(col)
{
    local a = (col >> 24) & 0xff;
    local r = (col >> 16) & 0xff;
    local g = (col >> 8)  & 0xff;
    local b = (col)       & 0xff;
	return (r * 128 / 255) << 24 | (g * 128 / 255) << 16 | (b * 128 / 255) << 8 | (a * 128 / 255);
}

/**
 * 継承状態の確認
 * @param target 調査対象
 * @param source 元になるクラス
 * @return target が source を継承していたら true
 */
function inherited(target, source)
{
	local p = target.parent;
	while (p != null) {
		if (p == source) {
			return true;
		}
		p = p.parent;
	}
	return false
}

/**
 * 配列からデータを削除する
 * @param array 配列
 * @value 削除する値
 */
function removeFromArray(array, value)
{
	local i = 0;
	while (i < array.len()) {
		if (array[i] == value) {
			array.remove(i);
		} else {
			i++;
		}
	}
}
