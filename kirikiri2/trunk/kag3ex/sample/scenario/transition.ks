*start|スタート

[linemode mode=page]

[init]World 機能におけるトランジション記述

; 個別にトランジション指定
; タグが閉じた時点で以下の処理を行う
; 1. 全体を裏画面にコピー
; 2. 対象のレイヤだけを裏画面で更新
; 3. トランジション
[道路 trans=normal][wt]個別更新
[あい 制服 中 trans=normal][wt]個別更新

[あい びょんびょん]
びょんびょんアクション

; 全体へのトランジション指定
; トランジション開始を宣言。
; ・現在の画面が裏画面になる
; ・描画対象が裏画面になる
[begintrans]

; 個別の状態変更。すべて裏画面に適用される
[夕]
[あい ガクガク]

; トランジション処理実行
; これ以降、描画対象が表に戻る
[endtrans trans=normal][wt]
一気に更新

[env 夜 trans=wave][wt]wave
[env 夕 trans=mosaic][wt]mosaic
[env 昼 trans=turn][wt]turn
[env 夜 trans=rotatezoom][wt]rotatezoom
[env 夕 trans=rotatevanish][wt]rotatevanish
[env 昼 trans=rotateswap][wt]rotateswap
[env 夜 trans=ripple][wt]ripple

[jump storage=first.ks target=*start]
