@if exp="typeof(global.bubble_object) == 'undefined'"
@iscript

/*
	泡をふらせるプラグイン
*/

class BubbleGrain
{
	// 泡粒のクラス

	var fore; // 表画面の泡粒オブジェクト
	var back; // 裏画面の泡粒オブジェクト
	var xvelo; // 横速度
	var yvelo; // 縦速度
	var xaccel; // 横加速
	var l, t; // 横位置と縦位置
	var ownwer; // このオブジェクトを所有する BubblePlugin オブジェクト
	var spawned = false; // 泡粒が出現しているか
	var window; // ウィンドウオブジェクトへの参照

	function BubbleGrain(window, n, owner)
	{
		// BubbleGrain コンストラクタ
		this.owner = owner;
		this.window = window;

		fore = new Layer(window, window.fore.base);
		back = new Layer(window, window.back.base);

		fore.absolute = 2000000-1; // 重ね合わせ順序はメッセージ履歴よりも奥
		back.absolute = fore.absolute;

		fore.hitType = htMask;
		fore.hitThreshold = 256; // マウスメッセージは全域透過
		back.hitType = htMask;
		back.hitThreshold = 256;

		fore.loadImages("bubble_" + n); // 画像を読み込む
		back.assignImages(fore);
		fore.setSizeToImageSize(); // レイヤのサイズを画像のサイズと同じに
		back.setSizeToImageSize();
		xvelo = 0; // 横方向速度
		yvelo = -(n*0.6 + 1.9 + Math.random() * 0.2); // 縦方向速度
		xaccel = Math.random(); // 初期加速度
	}

	function finalize()
	{
		invalidate fore;
		invalidate back;
	}

	function spawn()
	{
		// 出現
		l = Math.random() * window.primaryLayer.width; // 横初期位置
		t = window.primaryLayer.height; // 縦初期位置
		spawned = true;
		fore.setPos(l, t);
		back.setPos(l, t); // 裏画面の位置も同じに
		fore.visible = owner.foreVisible;
		back.visible = owner.backVisible;
	}

	function resetVisibleState()
	{
		// 表示・非表示の状態を再設定する
		if(spawned)
		{
			fore.visible = owner.foreVisible;
			back.visible = owner.backVisible;
		}
		else
		{
			fore.visible = false;
			back.visible = false;
		}
	}

	function move()
	{
		// 泡粒を動かす
		if(!spawned)
		{
			// 出現していないので出現する機会をうかがう
			if(Math.random() < 0.002) spawn();
		}
		else
		{
			l += xvelo;
			t += yvelo;
			xvelo += xaccel;
			xaccel += (Math.random() - 0.5) * 0.3;
			if(xvelo>=1.5) xvelo=1.5;
			if(xvelo<=-1.5) xvelo=-1.5;
			if(xaccel>=0.2) xaccel=0.2;
			if(xaccel<=-0.2) xaccel=-0.2;
			if(t <= -fore.height)
			{
				t = window.primaryLayer.height;
				l = Math.random() * window.primaryLayer.width;
			}
			fore.setPos(l, t);
			back.setPos(l, t); // 裏画面の位置も同じに
		}
	}

	function exchangeForeBack()
	{
		// 表と裏の管理情報を交換する
		var tmp = fore;
		fore = back;
		back = tmp;
	}
}

class BubblePlugin extends KAGPlugin
{
	// 泡を振らすプラグインクラス

	var bubbles = []; // 泡粒
	var timer; // タイマ
	var window; // ウィンドウへの参照
	var foreVisible = true; // 表画面が表示状態かどうか
	var backVisible = true; // 裏画面が表示状態かどうか

	function BubblePlugin(window)
	{
		super.KAGPlugin();
		this.window = window;
	}

	function finalize()
	{
		// finalize メソッド
		// このクラスの管理するすべてのオブジェクトを明示的に破棄
		for(var i = 0; i < bubbles.count; i++)
			invalidate bubbles[i];
		invalidate bubbles;

		invalidate timer if timer !== void;

		super.finalize(...);
	}

	function init(num, options)
	{
		// num 個の泡粒を出現させる
		if(timer !== void) return; // すでに泡粒はでている

		// 泡粒を作成
		for(var i = 0; i < num; i++)
		{
			var n = intrandom(0, 4); // 泡粒の大きさ ( ランダム )
			bubbles[i] = new BubbleGrain(window, n, this);
		}
		bubbles[0].spawn(); // 最初の泡粒だけは最初から表示

		// タイマーを作成
		timer = new Timer(onTimer, '');
		timer.interval = 80;
		timer.enabled = true;

		foreVisible = true;
		backVisible = true;
		setOptions(options); // オプションを設定
	}

	function uninit()
	{
		// 泡粒を消す
		if(timer === void) return; // 泡粒はでていない

		for(var i = 0; i < bubbles.count; i++)
			invalidate bubbles[i];
		bubbles.count = 0;

		invalidate timer;
		timer = void;
	}

	function setOptions(elm)
	{
		// オプションを設定する
		foreVisible = +elm.forevisible if elm.forevisible !== void;
		backVisible = +elm.backvisible if elm.backvisible !== void;
		resetVisibleState();
	}

	function onTimer()
	{
		// タイマーの周期ごとに呼ばれる
		var bubblecount = bubbles.count;
		for(var i = 0; i < bubblecount; i++)
			bubbles[i].move(); // move メソッドを呼び出す
	}

	function resetVisibleState()
	{
		// すべての泡粒の 表示・非表示の状態を再設定する
		var bubblecount = bubbles.count;
		for(var i = 0; i < bubblecount; i++)
			bubbles[i].resetVisibleState(); // resetVisibleState メソッドを呼び出す
	}

	function onStore(f, elm)
	{
		// 栞を保存するとき
		var dic = f.bubbles = %[];
		dic.init = timer !== void;
		dic.foreVisible = foreVisible;
		dic.backVisible = backVisible;
		dic.bubbleCount = bubbles.count;
	}

	function onRestore(f, clear, elm)
	{
		// 栞を読み出すとき
		var dic = f.bubbles;
		if(dic === void || !+dic.init)
		{
			// 泡はでていない
			uninit();
		}
		else if(dic !== void && +dic.init)
		{
			// 泡はでていた
			init(dic.bubbleCount, %[ forevisible : dic.foreVisible, backvisible : dic.backVisible ] );
		}
	}

	function onStableStateChanged(stable)
	{
	}

	function onMessageHiddenStateChanged(hidden)
	{
	}

	function onCopyLayer(toback)
	{
		// レイヤの表←→裏情報のコピー
		// このプラグインではコピーすべき情報は表示・非表示の情報だけ
		if(toback)
		{
			// 表→裏
			backVisible = foreVisible;
		}
		else
		{
			// 裏→表
			foreVisible = backVisible;
		}
		resetVisibleState();
	}

	function onExchangeForeBack()
	{
		// 裏と表の管理情報を交換
		var bubblecount = bubbles.count;
		for(var i = 0; i < bubblecount; i++)
			bubbles[i].exchangeForeBack(); // exchangeForeBack メソッドを呼び出す
	}
}

kag.addPlugin(global.bubble_object = new BubblePlugin(kag));
	// プラグインオブジェクトを作成し、登録する

@endscript
@endif
; マクロ登録
@macro name="bubbleinit"
@eval exp="bubble_object.init(17, mp)"
@endmacro
@macro name="bubbleuninit"
@eval exp="bubble_object.uninit()"
@endmacro
@macro name="bubbleopt"
@eval exp="bubble_object.setOptions(mp)"
@endmacro
@return
