@eval exp="f.hoge=1"
@tempsave
@eval exp="f.hoge=2"
@tempload
@eval exp="f.loadnum = 0"
@eval exp="(Dictionary.assignStruct incontextof f)(Scripts.evalStorage(System.exePath + kag.saveDataLocation + '/' + kag.dataName + f.loadnum + 'u.kdt'))"
