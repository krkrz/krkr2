/**
 * 表裏画面切り替え処理の対象になるレイヤ
 * ・アクション起動処理
 * ・フェードつき消去機能
 */
class GraphicLayer extends BasicLayer {

	// 処理対象プレイヤー
	player = null;
	
	/**
	 * コンストラクタ
	 * @parma isfore 表画面に属している
	 */
	constructor(player, isfore) {
		::BasicLayer.constructor();
		this.player = player;
	}

	// アクション処理
	function assign(src) {
		::BasicLayer.assign(src);
		hide = src.hide;
		setVisvalue(src.getVisValue());
		player.copyAction(src, this, onActionCompleted);
	}

    // --------------------------------------------
    // アクション系拡張
    // --------------------------------------------

	hide = null; // アクション終了時に消去

	function onActionCompleted() {
		if (hide) {
			setVisible(false);
		}
		player.onActionCompleted(this);
    }

	function getActionResult() {
		return player.getActionResult(this);
	}
	
	function beginAction(elm, hide, nowait) {
		if (hide != null) {
			this.hide = hide;
		}
		return player.beginAction(this, elm, onActionCompleted, nowait);
    }

    function stopAction() {
        player.stopAction(this);
    }

    function reset() {
		hide = false;
		stopAction();
		::BasicLayer.reset();
    }

	_visvalue = 0;
	function getVisible() {
		return _visvalue > 0;
	}
	function setVisible(v) {
		_visvalue = v ? 100 : 0;
		setOpacity(__opacity);
	}

	function getVisvalue() {
		return _visvalue;
	}
	function setVisvalue(v) {
		_visvalue = v;
		setOpacity(__opacity);
	}
	
	__opacity = 255;
	function getOpacity() {
		return __opacity;
	}
	function setOpacity(v) {
		__opacity = v;
		local o = _opacity * _visvalue / 100;
		::BasicLayer.setOpacity(o);
		::BasicLayer.setVisible(o > 0);
	}
};
