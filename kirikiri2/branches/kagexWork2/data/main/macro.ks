; 各シーンの初期化用
[macro name=initscene]
[clearlayers]
[stopse buf=1]
[stopse buf=2]
[stopbgm]
[stopvideo]
[video visible=false]
[history enabled=true]
[rclick enabled=true jump=false]
[scenestart storage=%storage linemode=1 erafterpage=true craftername=true]
[endmacro]
@return
