@wait time=200
@macro name=cl
@call *
@endmacro
*test1|セーブ1
@startanchor
@ct
サブルーチンのテスト[l]
@disablestore
disablestoreしてみた[l]
@cl target=*test2
サブルーチンから戻った subtest_test1[l]
0 -> 5[copybookmark from=0 to=5][l]
erase 5[erasebookmark place=5][s]


*test2|セーブ2
@ct
サブルーチンのテスト[l]
@load place=0 ask=true
@gotostart ask=true
@store enabled=false
store enabled=false[l]
@store enabled=true
store enabled=true[l]
@save place=4 ask=true
@cl target=*test3 storage=subtest2.ks
サブルーチンから戻った subtest_test2[l]
@return

