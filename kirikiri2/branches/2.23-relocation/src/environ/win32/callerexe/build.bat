brc32 -r krkrrel.rc
tasm32 /zi /ml krkrrel.asm
ilink32 -v -aa -Tpe krkrrel.OBJ , krkrrel.exe, , import32.lib ..\krdevui.lib , , krkrrel.RES
copy krkrrel.exe ..
copy krkrrel.tds ..

brc32 -r krkrconf.rc
tasm32 /zi /ml krkrconf.asm
ilink32 -v -aa -Tpe krkrconf.OBJ , krkrconf.exe, , import32.lib ..\krdevui.lib , , krkrconf.RES
copy krkrconf.exe ..
copy krkrconf.tds ..

brc32 -r krkrfont.rc
tasm32 /zi /ml krkrfont.asm
ilink32 -v -aa -Tpe krkrfont.OBJ , krkrfont.exe, , import32.lib ..\krdevui.lib , , krkrfont.RES
copy krkrfont.exe ..
copy krkrfont.tds ..

brc32 -r krkrtpc.rc
tasm32 /zi /ml krkrtpc.asm
ilink32 -v -aa -Tpe krkrtpc.OBJ , krkrtpc.exe, , import32.lib ..\krdevui.lib , , krkrtpc.RES
copy krkrtpc.exe ..
copy krkrtpc.tds ..

brc32 -r krkrsign.rc
tasm32 /zi /ml krkrsign.asm
ilink32 -v -aa -Tpe krkrsign.OBJ , krkrsign.exe, , import32.lib ..\krdevui.lib , , krkrsign.RES
copy krkrsign.exe ..
copy krkrsign.tds ..

brc32 -r krkrlt.rc
tasm32 /zi /ml krkrlt.asm
ilink32 -v -aa -Tpe krkrlt.OBJ , krkrlt.exe, , import32.lib ..\krdevui.lib , , krkrlt.RES
copy krkrlt.exe ..
copy krkrlt.tds ..

call build_userconf.bat

pause