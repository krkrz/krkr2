/**
 * レイヤ系基底クラス
 * 画像表示が実装されている
 */
class EnvLayer extends EnvLevelLayer {

    /// 描画中画像
	imageFile = null;

	// タイル描画処理 XXX 一応残しておく
	tileFile = null;

	_tilex = null;
	function getTilex() {
		return _tilex;
	}
	function setTilex(v) {
		_tilex = v;
		updateFlag = true;
	}

	_tiley = null;
	function getTiley() {
		return _tiley;
	}
	function setTiley(v) {
		_tiley = v;
		updateFlag = true;
	}
	
	envLayerCommands = null;

	function constructor(env, name) {
		::EnvLevelLayer.constructor(env, name);
		envLayerCommands = {
			file = setImageFile,
			tile = setTileFile,
			loop = null,
			alpha = null,
			width = function(param, elm) { setWidth(param); },
			height = function(param, elm) { setHeight(param); }
		};
	}
	
	function initFile(elm) {
		if ("type" in elm)    type = null;
		if ("opacity" in elm) props.opacity <- 255;
		if ("rotate" in elm)  props.rotate  <- 0;
		if ("zoom" in elm)    props.zoom    <- 100;
		if ("afx" in elm)     delete props.afx;
		if ("afy" in elm)     delete props.afy;
	}
	
	// 画像ファイルを指定
	function setImageFile(file, elm) {

		initFile(elm);
		
		tileFile  = null;
		imageFile = {};
		imageFile.storage    <- file;
		if ("lightcolor" in elm)
			imageFile.lightcolor <- elm.lightcolor;
		if ("lighttype" in elm)
			imageFile.lighttype  <- elm.lighttype;
		if ("brightness" in elm)
			imageFile.brightness <- elm.brightness;
		if ("contrast" in elm)
			imageFile.contrast   <- elm.contrast;
		
		// 背景指定時に座標指定がなければ場所情報を初期化する
		if (!("xpos" in elm) && !("left" in elm)) {
			delete props.left;
		}
		if (!("ypos" in elm) && !("left" in elm)) {
			delete props.top;
        }

		setDisp(SHOW);
		redraw = true;

		// 記録
		player.setCGFlag(file);
		
        return true;
    }

	// 塗りつぶしイメージファイルを指定
	function setTileFile(file, elm) {

		initFile(elm);
		
		imageFile = null;
		tileFile = file;

		if ("tilex" in elm) { tilex = elm.tilex.tointeger(); }
		if ("tiley" in elm) { tiley = elm.tiley.tointeger(); }
		
		// 背景指定時に座標指定がなければ場所情報を初期化する
		if (!("xpos" in elm) && !("left" in elm)) {
			delete props.left;
		}
		if (!("ypos" in elm) && !("left" in elm)) {
			delete props.top;
        }

		setDisp(SHOW);
		redraw = true;

        return true;
    }
	
    /**
	 * コマンドの実行
	 * @param cmd コマンド
	 * @param param パラメータ
	 * @param elm 他のコマンドも含む全パラメータ
     * @return 実行が行われた場合 true
     */
    function doCommand(cmd, param, elm) {

		// 共通コマンド
		if (::EnvLevelLayer.doCommand(cmd, param, elm)) {
			return true;
        }
		// レイヤ共通コマンド
		if (cmd in envLayerCommands) {
			local func = envLayerCommands[cmd];
			if (func != null) {
				func(param, elm);
			}
            return true;
        }
		
		return false;
    }

	function drawLayer(layer) {
		if (isShowBU()) {
			if (tileFile != null) {
				//dm("タイル画像");
				try {
					layer.setTileImage(tileFile);
					// 座標補正
					if (layer.tileLayer != null) {
						if (_width == null ) {
							_width = layer.tileLayer.width;
						}
						if (_height == null) {
							_height = layer.tileLayer.height;
						}
					}
				} catch (e) {
					local text = tileFile + ":タイル画像がロードできません";
					player.errorImage(text);
				}
			} else if (imageFile != null) {
				// 画像のロード
				layer.clearTile();
				try {
					layer.loadImages(imageFile);
					_width  = layer.width;
					_height = layer.height;
				} catch (e) {
					local text = imageFile.storage + ":画像がロードできません";
					player.errorImage(text);
				}
			}
		}
	}

	function updateLayer(layer) {
		super.updateLayer(layer);
		if (tileFile != null) {
			layer.setTilePos(tilex, tiley);
		}
	}
}
