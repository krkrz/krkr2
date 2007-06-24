/**
 * 画面切り替え処理レイヤ
 */
class TransLayer {

    // XXX 本来システム側から与えるべき
    width = 640;
    height = 448;

    info=null;       //< 種別情報
    time=0;          //< 切り替えにかかる時間
    color=0x000000;  //< 塗りつぶし色
    paint = true;    //< 塗りつぶし処理
    
	first=true;   //< 初回フラグ
    now=0;        //< 実行時刻
    count=0;      //< 現在のカウント
    maxCount=0;   //< 最大カウント

    // 塗りつぶし領域記録用
    rects = [];

    function clear() {
        for (local i=rects.len() - 1; i>=0; i--) {
            rects.remove(i);
        }
    }
    
    /**
     * 全面塗りつぶし
     */
    function fill() {
        clear();
        if (paint) {
			rects.append(FillRect(0,0,width,height,0,color));
        }
    }

    /**
     * 塗りつぶし領域追加
     */
    function fillRect(x, y, w, h) {
        if (w > 0 && h > 0) {
			rects.append(FillRect(x, y, w, h, 0, color));
        }
    }

    // -----------------------------------------

    // ワイプの幅を指定
    wipe_size = 16;

    /*
     * 左右ワイプ初期化
     */
    function initWipeLR(elm) {
		wipe_size = getval(elm,"size",16);
        maxCount  = ((width+wipe_size-1)/wipe_size)+wipe_size-1;
    }

    /**
     * 左から右
     */
    function wipeltor_effect(count) {
        local cnt = count;
        for (local x=0; x<width; x+=wipe_size) {
            local size = (cnt > wipe_size) ? wipe_size : (cnt < 0) ? 0 : cnt;
            if (paint) {
                fillRect(x, 0, size, height);
            } else {
                fillRect(x + size, 0, wipe_size - size, height);
            }
            cnt--;
        }
    }

    /**
     * 右から左
     */
    function wipertol_effect(count) {
        local cnt = count;
        for (local x=width; x>0; x-=wipe_size) {
            local size = (cnt > wipe_size) ? wipe_size : (cnt < 0) ? 0 : cnt;
            if (paint) {
                fillRect(x-size, 0, size, height);
            } else {
                fillRect(x-wipe_size, 0, wipe_size - size, height);
            }
            cnt--;
        }
    }
    
    types = null;
    
    /**
     * タイマ駆動部
     */
    function update(diff) {
		if (now != null) {
	        if (first) {
    	        count = -1;
        	    now = 0;
            	first = false;
	        } else {	
    	        now += diff;
	        }
	        // 描画処理
	        local newcount;
	        if ((newcount = (now * maxCount)/time) > count) {
                clear();
                if (newcount >= maxCount) {
                    newcount = maxCount;
	            }
                info.effect(newcount);
        	    count = newcount;
	        } 
	        // 終了処理
    	    if (count >= maxCount) {
        	    done();
	            trigger("layertrans");
	        }
		}
    }

    /**
     * 描画処理
     * @param elm
     * @param 種別
     * @param time 時間
     * @param color 色指定
     */
    function start(elm, type, time, color) {
        if (time != null) {
            this.time = time;
        }
        if (color != null) {
			this.color = color;
			paint = true;
        } else {
            paint = false;
        }
        if (type != null && type in types) {
            info = types[type];
        }
        if (info == null) {
            info = types["wipeltor"];
        }
        info.init(elm);
		if (time == 0) {
			done();
		} else {
			first = true;
			now = 0;
		}
    }

    /**
     * 終了
     */
    function done() {
        now = null;
        fill();
    }

    /**
     * 強制終了
     */
    function stop() {
        done();
        paint = false;
        fill();
    }

	constructor() {
		types = {
            // 左から右にワイプ
			wipeltor = {
				init   = initWipeLR.bindenv(this),
				effect = wipeltor_effect.bindenv(this)
                },
            wipertol = {
                init   = initWipeLR.bindenv(this),
                effect = wipertol_effect.bindenv(this)
                }
        };
    }
}
