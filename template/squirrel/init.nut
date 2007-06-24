// 画面サイズ定義
SCWIDTH  <- 640;
SCHEIGHT <- 448;

f <- {};     // ゲーム変数
sf <- {};    // システム変数
EMPTY <- {}; // 空の辞書

// 初期化処理
dofile("util.nut");
dofile("Object.nut");
dofile("BasicLayer.nut");
dofile("msgwindow.nut");
dofile("screenlayer.nut");
dofile("translayer.nut");

dofile("action.nut");
dofile("player.nut");

dofile("GraphicLayer.nut");
dofile("GamePlayer.nut");

dofile("EnvObject.nut");
dofile("EnvBase.nut");
dofile("EnvImage.nut");
dofile("EnvLevelLayer.nut");
dofile("EnvLayer.nut");
dofile("EnvBackLayer.nut");
dofile("EnvStageLayer.nut");
dofile("EnvEventLayer.nut");
dofile("EnvSimpleLayer.nut");
dofile("EnvCharacter.nut");
dofile("EnvBGM.nut");
dofile("EnvSE.nut");
dofile("Environment.nut");
dofile("ScenePlayer.nut");

dofile("main.nut");
