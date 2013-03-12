rem krkr.exe を高解像度対応済みマニフェストをついた状態にするバッチファイル
copy krkr.exe tvpwin32.exe
mt.exe -manifest highdpi.manifest -outputresource:tvpwin32.exe;#1
copy /b tvpwin32.exe+"..\..\src\core\environ\win32\optionarea.bin" krkr.exe /y
