/**
 * キャラクタ用レイヤ
 * LEVEL に応じた座標補正機能を持つ
 */
class CharacterGraphicLayer extends EnvGraphicLayer {

	yoffset = null; // yoffset情報
	
	/**
	 * コンストラクタ
	 * @param owner オブジェクト情報をもってる親
	 * @param isfore 表画面か裏画面か
	 */
	constructor(owner,isfore) {
		yoffset = owner.yoffset;
		::EnvGraphicLayer.constructor(owner, isfore);
	}

	function assign(src) {
		//dm("アサイン処理:" + yoffset);
		::EnvGraphicLayer.assign(src);
		this.yoffset = src.yoffset;
	}

	_level = null;
	function getLevel() {
		return _level;
	}

	function setLevel(v) {
		_level = v;
		recalcPosition();
	}
	
	function recalcPosition() {
		//dm("キャラ表示:" + name + ":" + yoffset + ":"  +level + ":" + left + ":" + top);
		// レベル別座標補正処理。とりあえず適当で画面中央に向かって縮小処理してある
		local zoom = 100;
		local levelYoffset = 0;
		if (level in owner.env.levels) {
			local levelInfo = owner.env.levels[level];
			if ("zoom"  in levelInfo) {
				zoom = levelInfo.zoom;
			}
			if ("yoffset" in levelInfo) {
				levelYoffset = levelInfo.yoffset;
			}
		}
		setRealPosition(owner.env.xmax + (left * zoom / 100) - width / 2, 
						owner.env.ymax + ((yoffset - top) * zoom / 100) - height + levelYoffset);
	}
};

/**
 * キャラクタレイヤ
 * ポーズの指定他
 */
class EnvCharacter extends EnvLevelLayer {

	/// 初期化名
	initName = null;

    /// 初期化情報
	poses = null;

    // 画像ファイル直接指定
	image = null;
    // 個別オフセット補正
	_yoffset = null;
	function getYoffset() {
		if (_yoffset != null) {
			return _yoffset;
		}
		if ("yoffset" in init) {
			return init.yoffset;
		}
		return env.yoffset;
	}
	function setYoffset(v) {
		_yoffset = v;
	}
    
    /// ポーズ
	_pose = null;
	function getPose() {
		if (_pose != null) {
			return _pose;
		}
		if ("defaultPose" in init) {
			return init.defaultPose;
		}
		return "通常";
	}
	function setPose(v) {
		_pose = v;
    };

    /// 服装
	dress = null;
	/// 表情
	face = null;

	// 表情とポーズの対応表
	facePoseMap = {};

	charCommands = null;
	
    /**
     * コンストラクタ
     * @param env 環境オブジェクト
     * @param name キャラクタ名
     * @param init キャラクタ初期化データ
     */
    function constructor(env, name, initName, init) {
		::EnvLevelLayer.constructor(emv, name);
        
        //dm("キャラクタ登録:" + name); 

        this.initName = initName;
		this.init     = init;
        if (init != null) {
			if ("poses" in init) {
				poses = init.poses;
			}
			// 表情ポーズ同期機能
			if ("facePose" in init) {
				// 表情からポーズに対するマップを作成する
				foreach(name, value in poses) {
					foreach (face, fvalue in values.faces) {
						facePoseMap[face] = name;
					}
				}
			}
		} else {
			init = {};
		}

		charCommands = {
			image   = setImage,
			dress   = setDress,
			face    = setFace,
			front   = setFront,
			back    = setBack,
			level   = setLevel,
			pos     = setPosition,
			facewin = function(param, elm) { setDisp(FACE); redraw = true; },
			faceon = null,
			faceoff = null,
		};
	}
    
	// 表情描画処理を行う
	redrawFace = null;

    /**
     * 直接画像指定
     */
    function setImage(imageName, elm) {
		if (imageName != image || isClear()) {
			image   = imageName;
			if ("yoffset" in elm) {
				setYoffset(elm.yoffset);
			}
			setDisp(SHOW);
			redraw  = true;
			if ("poseTrans" in init) {
				setAutoTrans(init.poseTrans);
			} else if ("charTrans" in init) {
				setAutoTrans(init.charTrans);
			} else if ("poseTrans" in env.envinfo) {
				setAutoTrans(env.envinfo.poseTrans);
			} else if ("charTrans" in env.envinfo) {
				setAutoTrnas(env.envinfo.charTrans);
			}
		}
    }
    
    /**
     * ポーズの設定
     */
    function setPose(poseName, elm) {

		if (poseName in poses) {
			local info = poses[poseName];
			if (poseName != pose || isClear()) {
				pose = poseName;
				setYoffset(info.yoffset);
				setDisp(SHOW);
				redraw = true;

				if ("trans" in info) {
					setAutoTrans(info.trans);
				} else if ("poseTrans" in init) {
					setAutoTrans(init.poseTrans);
				} else if ("charTrans" in init) {
					setAutoTrans(init.charTrans);
				} else if ("poseTrans" in env.envinfo) {
					setAutoTrans(env.envinfo.poseTrans);
				} else if ("charTrans" in env.envinfo) {
					setAutoTrnas(env.envinfo.charTrans);
				}
				
                // 表情表示取得
				if (isShowFace() &&
					(getval(elm,"faceon") || getval(env, "showFaceMode")) &&
					!getval(elm,"faceoff")) {
					redrawFace = true;
				}
            }
			// 服装初期化処理
			if (dress != null && (!("dresses" in info) || !(dress in info.dresses))) {
				dress = null;
			}
			// 表情初期化処理
			if (face != null && (!("faces" in info) || !(face in info.faces))) {
				face = null;
			}
        }

        // イメージを破棄
		image = null;
		// Y座標はポーズ設定したときに初期値に戻す
		if ("defaultYpos" in env) {
			setTop(env.defaultYpos);
		}
    }

    /**
     * 服装の設定
     */
    function setDress(dressName, elm) {
		if (dressName != dress || isClear()) {
			dress = dressName;
			setDisp(SHOW);
			redraw = true;

			if ("dressTrans" in init) {
				setAutoTrans(init.dressTrans);
			} else if ("charTrans" in init) {
				setAutoTrans(init.charTrans);
			} else if ("dressTrans" in env.envinfo) {
				setAutoTrans(env.envinfo.dressTrans);
			} else if ("charTrans" in env.envinfo) {
				setAutoTrnas(env.envinfo.charTrans);
			}

			// 表情表示取得
			if (isShowFace() && (getval(elm,"faceon") || getval(env,"showFaceMode")) && !getval(elm,"faceoff")) {
				redrawFace = true;
			}
		}
		image = null;
    }
    /**
     * 表情の設定
      */
    function _setFace(faceName, elm) {
		if (faceName != face || isClear()) {
			face = faceName;
			setDisp(SHOW);
			redraw = true;

			if ("faceTrans" in init) {
				setAutoTrans(init.faceTrans);
			} else if ("charTrans" in init) {
				setAutoTrans(init.charTrans);
			} else if ("faceTrans" in env.envinfo) {
				setAutoTrans(env.envinfo.faceTrans);
			} else if ("charTrans" in env.envinfo) {
				setAutoTrnas(env.envinfo.charTrans);
			}

			// 表情表示取得
			if (isShowFace() && (getval(elm,"faceon") || getval(env,"showFaceMode")) && !getval(elm,"faceoff")) {
				redrawFace = true;
			}
		}
		image = null;
    }

	function setFace(cmd,elm) {
		if (facePoseMap != null) {
			if (cmd in facePoseMap) {
				local p = facePoseMap.cmd;
				setPose(p,elm);
				_setFace(cmd,elm);
			}
		} else {
			_setFace(cmd,elm);
		}
	}
	
    /**
     * 場所表示用トランジション設定
     */
    function setPositionTrans(info) {
		if ("trans" in info) {
			setAutoTrans(info.trans);
		} else if ("positionTrans" in env.envinfo) {
			setAutoTrans(env.envinf.positionTrans);
		}
    }

	// 表示位置指定
	posName = null;
	
    /**
     * 表示位置の設定
     */
    function setPosition(cmd, elm) {

		local istrans = false;
		
		posName = getTo(cmd);
		if (!("positions" in env) || !(posName in env.positions)) {
			return;
		}
		local info = env.positions[posName];
		local istrans = false;
		
		if ("xpos" in info || "ypos" in info) {
			if (getDisp() == CLEAR) {
				setDisp(SHOW);
				istrans = true;
			}
			local posFrom = getFrom(cmd);
			if (posFrom != null && (posFrom in env.positions[posFrom])) {
				local fromInfo = env.positions[posFrom];
				if ("xpos" in info) {
					setPropFromTo("left", env.xmax, "xpos" in fromInfo ? fromInfo.xpos : null, info.xpos, elm);
				}
				if ("ypos" in info) {
					setPropFromTo("top", env.ymax, "ypos" in fromInfo ? fromInfo.ypos : null, info.ypos, elm);
				}
			} else {
				if ("xpos" in info) {
					setPropFromTo("left", env.xmax, null, info.xpos, elm);
				}
				if ("ypos" in info) {
					setPropFromTo("top", env.ymax, null, info.ypos, elm);
				}
			}
			// 未初期化時デフォルト
			if (!("left" in props)) {
				dm("デフォルトのX位置を指定");
				if ("defaultXpos" in env) {
					setLeft(env.defaultXpos);
				}
			}
			if (!("top" in props)) {
				if ("defaultYpos" in env) {
					setTop(env.defaultYpos);
				}
			}
		}

		if ("disp" in info) {
			setDisp(info.disp);
			istrans = true;
		}

		if ("level" in info) {
			setLevel(info.level);
			redraw = true;
			istrans = true;
		}

		if (istrans) {
			setPositionTrans(info);
		}
	}

    /**
     * ポーズ指定を優先処理する
     */
    function doPoseCommand(cmd, param, elm) {

        // ポーズコマンド
        if (cmd == "pose") {
            setPose(param, elm);
            return true;
        }

        // 顔ポーズマップが存在する場合
		if (facePoseMap != null) {
			if (cmd in facePoseMap) {
				local p = facePoseMap[cmd];
                setPose(p,elm);
                _setFace(cmd,elm);
                return true;
            }
        }
        
        // ポーズ指定
        if (poses != null) {
			if (cmd in poses[cmd]) {
				setPose(cmd,elm);
				return true;
			}
        }
        
        return false;
    }
    
    /**
     * コマンドの実行
     * @param cmd コマンド
     * @param param パラメータ
     * @param elm 他のコマンドも含む全パラメータ
     * @return 実行が行われた場合 true
     */
    function doCommand(cmd, param, elm) {

		if (::EnvImage.doCommand(cmd, param, elm)) {
			return true;
        }

		if (cmd in charCommands) {
			local func = charCommands[cmd];
			if (func != null) {
				func(param, elm);
			}
            return true;
        }
        
        local info;
        local find = false;

		if (poses != null && pose != null && image == null) {
			
			if (pose in poses) {
				local poseInfo = poses[pose];
				if ("dresses" in poseInfo && cmd in poseInfo.dresses) {
					//dm("服装を設定");
					find = true;
					setDress(cmd,elm);
				} else if ("faces" in poseInfo && cmd in poseInfo.faces) {
					//dm("表情を設定");
					find = true;
					setFace(cmd,elm);
                }
            } else {
				dm("ポーズ情報がありません:" + pose + ":" + cmd);
            }
        }

		local posName = getTo(cmd);
		if (!find && "positions" in env && posName in env.positions) {
			setPosition(cmd, elm);
			find = true;
        }            

        return find;
    };

	/**
	 * 立ち絵画像の取得
	 * @param levelName 現在の表示レベル
	 * @param pose      ポーズ指定
	 */
	function _getPoseImage(levelName, pose) {
		local poseInfo;
		if (poses != null && pos in poses) {
			local poseInfo = poses[pose];
			local imageFile;
			
			// 顔描画の場合は msgImage / msgFaceImage を優先
			local imageName;
			if (env.faceLevelName == levelName && "msgImage" in poseInfo) {
				imageName = poseInfo.msgImage;
			} else if ("image" in poseInfo) {
				imageName = poseInfo.image;
			}
			if (imageName == null) {
				return;
			}

			imageName = replace(imageName, "LEVEL", levelName);
			
			// 服装指定が無い場合はデフォルトを参照
			local dressName;
			if (dress != null && "dresses" in poseInfo && dress in poseInfo.dresses) {
				dressName = poseInfo.dresses[dress];
            } 
			if (dressName == null && "defaultDress" in poseInfo) {
				dressName = poseInfo.defaultDress;
			}

            // 表情指定が無い場合はデフォルトを参照
            local faceName;
			if (face != null && "faces" in poseInfo && face in poseInfo.faces) {
				faceName = poseInfo.faces[face];
            }
			if (faceName == null && "defaultFace" in poseInfo) {
				faceName = poseInfo.defaultFace;
			}

            //dm("dress:", dressName, "face:", faceName);
            
			// 顔合成型立ち絵
			// 服装情報で上書き
			if (dressName != null) {
				imageFile = replace(imageName, "DRESS", dressName);
			} else {
				imageFile = imageName;
			}
			// 表情で上書き
			if (faceName != null) {
				imageFile = replace(imageFile, "FACE", faceName);
			}
			return imageFile;
		}
	}
	
	/**
	 * 立ち絵画像の取得
	 * @param levelName 現在の表示レベル
	 */
	function getPoseImage(levelName) {
		local imageName = _getPoseImage(levelName, pose);
		if ((imageName == null || !isExistImageName(imageName)) && "defaultPose" in init && pose != init.defaultPose) {
			imageName = _getPoseImage(levelName, init.defaultPose);
		}
		return imageName;
	}

    /**
     * 現在の立ち絵の描画
     * @param layer 描画対象レイヤ
     */
	function drawLayer(layer) {
		if (isShowBU()) {

			// レベルで補正
			local levelName = level;
			if (level in env.levels) {
				if ("name" in env.levels[level]) {
					levelName = env.levels[level].name;
				}
			}

			// 画像を決定
			local imageName;
			if (image != null) {
				imageName = replace(image, "LEVEL", levelName);
			} else {
				imageName = getPoseImage(levelName);
			}

			// 画像のロード
			try {
				local elm = { storage=imageName };
				// 色補正
				local timeInfo;
				if ((timeInfo = env.getCurrentTime()) != null) {
					if ("charLightColor" in timeInfo) {
						elm.lightcolor = timeInfo.charLightColor;
					}
					if ("charLightType" in timeInfo) {
						elm.lighttype  = timeInfo.charLightType;
					}
					if ("charBrigtness" in timeInfo) {
						elm.brightness = timeInfo.charBrightness;
					}
					if ("charContrast" in timeInfo) {
						elm.contrast   = timeInfo.charContrast;
					}
				}
				layer.loadImages(elm);
			} catch (e) {
				player.errorImage(imageName + ":立ち絵画像がロードできません");
				return;
			}
			// サイズ補正
			//layer.setSizeToImageSize();
			_width  = layer.width;
			_height = layer.height;
		}
	}

	/**
	 * 新レイヤ生成
	 */
	function createLayer(isfore) {
		return CharacterGraphicLayer(this, isfore);
	}
	
    /**
     * レイヤ状態更新処理
     * エモーション処理を追加
     */
	function updateLayer(layer) {
		::EnvImage.updateLayer(layer);
		// 表情描画
		if (redrawFace) {
			// 表情と名前の再描画指定 XXXXX
			redrawFace = false;
        }
    }

	// ------------------------------------------------------
	
	/**
	 * タグ処理
	 * @param elm コマンドパラメータ
	 */
	function command(elm) {
		local e = clone elm;
		foreach (name, value in e) {
			if (doPoseCommand(name, value, e)) {
				delete e[name];
			}
		}
		::EnvImage.command(e);
	}

	/**
	 * 表示処理用の自動トランジションの指定
	 */
	function setDispAutoTrans() {
		if ("dispTrans" in init) {
			setAutoTrans(init.dispTrans);
		} else if ("charDispTrans" in env.envinfo) {
			setAutoTrans(env.envinfo.charDispTrans);
		}
	}

	// フェード指定のデフォルト値
	function getFadeValue() {
		if ("fadeValue" in init) {
			return init.fadeValue;
		} else {
			if ("charFadeValue" in env.envinfo) {
				return env.envinfo.charFadeValue;
			} else {
				return env.getFadeValue();
			}
		}
    }
};
