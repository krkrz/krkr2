/**
 * メッセージ窓クラス
 */
class MessageWindow extends Object {

	// ベースのテキストレイヤ
	msg = null;
	// 背景の塗りつぶし
	back = null;
	
	/**
	 * コンストラクタ
	 */
	constructor(x,y,w,h) {
		msg  = MessageLayer(x,y,w,h);
		msg.setLevel(4096);
		back = FillRect(x,y,w,h,4095,0x80000088); // XXX
		back.setLevel(4095);
	}

	/**
	 * テキストの表示
	 */
	function write(text) {
		msg.write(text);
	}

	/**
	 * テキストの消去
	 */
	function clear() {
		msg.clear();
	}

	/**
	 * 名前プレートの表示
	 */
	function showNamePlate(name) {
	}

	/**
	 * カーソルの表示
	 */
	function showCursor(cursor) {
		//msg.showCursor(cursor);
	}

	/**
	 * 表示状態
	 */
	function show(show) {
		print("メッセージ窓表示状態:" + show + "\n");
		msg.show(show);
		back.show(show);
	}
}
