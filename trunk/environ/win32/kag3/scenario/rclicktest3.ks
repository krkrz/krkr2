*first|最初
@startanchor
[rclick name="サブルーチンのテスト" call=true target=*sub1]\
右クリックのテストなりー[l]
なりー[l]

*a
[link target=*hogera]選択肢１[endlink]
[link]選択肢２[endlink]
[s]

*hogera
あ[l]
@jump target=*a

*sub1
@locksnapshot
; ↑スナップショットをロックする
@tempsave
; ↑一時的に状態を保存
@history output=false
; ↑メッセージ履歴への出力を無効に
@mapdisable layer=0 page=fore
; ↑クリッカブルマップをもし使っている場合はこのようにして無効化する
;@locklink
; ↑ 選択肢選択のロック
@backlay
@layopt layer=message1 page=back visible=true
; ↑このサブルーチン内ではメッセージレイヤ1を使う
@layopt layer=message0 page=back visible=false
@current layer=message1 page=back
@position left=0 top=0 width=640 height=480
@eval exp="f.r_first=true"
; ↑このルーチンに入ったときにだけトランジションを行うように
;
*menu
@er
@nowait
[link target=*hide]メッセージを消す[endlink][r]
[link target=*history]メッセージ履歴を見る[endlink][r]
[link exp="kag.loadBookMarkFromFileWithAsk()"]栞をたどる[endlink][r]
[link exp="kag.saveBookMarkToFileWithAsk()"]栞をはさむ[endlink][r]
[link target=*gotostart]最初に戻る[endlink][r]
[link target=*ret]戻る[endlink][r]
@endnowait
@current layer=message1 page=fore
@if exp="f.r_first"
@trans time=500 rule=trans1 vague=128
@wt
@endif
@eval exp="f.r_first=false"
@s

*ret
; サブルーチンから戻る
@tempload bgm=false se=false backlay=true
@trace exp="kag.back.layers[0].loadedProvinceActions"
@trans time=500 rule=trans1 vague=128
@wt
@unlocksnapshot
@return

*hide
; メッセージを消す
@hidemessage
@jump target=*menu

*history
; メッセージ履歴を見る
@showhistory
@jump target=*menu

*gotostart
; 「最初に戻る」
@gotostart ask=true
@jump target=*menu
