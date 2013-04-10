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
@trace exp="kag.fore.layers[0].loadedProvinceActions"
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
[link target=*load]栞をたどる[endlink][r]
[link target=*save]栞をはさむ[endlink][r]
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
@return

*hide
; メッセージを消す
@hidemessage
@jump target=*menu

*history
; メッセージ履歴を見る
@showhistory
@jump target=*menu

*load
; 栞をたどる
; emb exp= .... については TJS Tips を参照
@er
@nowait
[link target=*lt0][emb exp="kag.getBookMarkPageName(0)"][endlink][r]
[link target=*lt1][emb exp="kag.getBookMarkPageName(1)"][endlink][r]
[link target=*lt2][emb exp="kag.getBookMarkPageName(2)"][endlink][r]
[link target=*lt3][emb exp="kag.getBookMarkPageName(3)"][endlink][r]
[link target=*lt4][emb exp="kag.getBookMarkPageName(4)"][endlink][r]
[link target=*menu]戻る[endlink][r]
@endnowait
@s

*lt0
@load place=0 ask=true
@jump target=*menu
*lt1
@load place=1 ask=true
@jump target=*menu
*lt2
@load place=2 ask=true
@jump target=*menu
*lt3
@load place=3 ask=true
@jump target=*menu
*lt4
@load place=4 ask=true
@jump target=*menu

*save
; 栞をはさむ
; emb exp= .... については TJS Tips を参照
@er
@nowait
[link target=*st0][emb exp="kag.getBookMarkPageName(0)"][endlink][r]
[link target=*st1][emb exp="kag.getBookMarkPageName(1)"][endlink][r]
[link target=*st2][emb exp="kag.getBookMarkPageName(2)"][endlink][r]
[link target=*st3][emb exp="kag.getBookMarkPageName(3)"][endlink][r]
[link target=*st4][emb exp="kag.getBookMarkPageName(4)"][endlink][r]
[link target=*menu]戻る[endlink][r]
@endnowait
@s

*st0
@save place=0 ask=true
@jump target=*menu
*st1
@save place=1 ask=true
@jump target=*menu
*st2
@save place=2 ask=true
@jump target=*menu
*st3
@save place=3 ask=true
@jump target=*menu
*st4
@save place=4 ask=true
@jump target=*menu


*gotostart
; 「最初に戻る」
@gotostart ask=true
@jump target=*menu
