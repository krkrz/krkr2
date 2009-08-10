'
' VBScript による記述。
' VBScript には引数ありのコンストラクタの概念がない罠（涙)
' 仕方がないので tjs で記述した生成メソッドを呼び出してます。
'
Dim win, base
Set win = createWindow()
Call win.setInnerSize(400,400)
Set base = createLayer(win, null)
Call base.setSize(400,400)
Call base.fillRect(0,0,400,400,&hff00ff)
base.visible = true
win.visible = true
