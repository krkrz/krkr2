; フォームのテスト
@position vertical=true
@image storage=_24 layer=base visible=true
@wait time=200
*first
@er
@nowait
@backlay
@current page=back layer=message0
f.data1 :
あいう[edit length=200 name="f.data1" bgcolor=0x000000 color=0xffffff opacity=64]えお
f.data2 :
[edit length=200 name="f.data2"]あいうえお
[checkbox name="f.data3" bgcolor=0x000000 color=0xffffff opacity=64] f.data3
[checkbox name="f.data4"] f.data4

[link target=*ok]　　OK　　[endlink] [link target=*cancel]キャンセル[endlink]
@endnowait
@trans vague=1000 rule=trans1 time=1000
@wt
@current page=fore layer=message0
@s

*ok
@commit
@er
OKが選択されました。
@call target=*dispvars
@l
@jump target=*first

*cancel
@er
キャンセルが選択されました。
@call target=*dispvars
@l
@jump target=*first

*dispvars
f.data1 : [emb exp=f.data1]
f.data2 : [emb exp=f.data2]
f.data3 : [emb exp=f.data3]
f.data4 : [emb exp=f.data4]
@return
