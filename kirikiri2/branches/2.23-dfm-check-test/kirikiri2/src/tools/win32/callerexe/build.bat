brc32 -r krkrrel.rc
tasm32 /zi /ml krkrrel.asm
ilink32 -v -aa -Tpe krkrrel.OBJ , krkrrel.exe, , import32.lib ..\..\..\..\bin\win32\tools\krdevui.lib , , krkrrel.RES
copy krkrrel.exe ..\..\..\..\bin\win32\tools
copy krkrrel.tds ..\..\..\..\bin\win32\tools

brc32 -r krkrconf.rc
tasm32 /zi /ml krkrconf.asm
ilink32 -v -aa -Tpe krkrconf.OBJ , krkrconf.exe, , import32.lib ..\..\..\..\bin\win32\tools\krdevui.lib , , krkrconf.RES
copy krkrconf.exe ..\..\..\..\bin\win32\tools
copy krkrconf.tds ..\..\..\..\bin\win32\tools

brc32 -r krkrfont.rc
tasm32 /zi /ml krkrfont.asm
ilink32 -v -aa -Tpe krkrfont.OBJ , krkrfont.exe, , import32.lib ..\..\..\..\bin\win32\tools\krdevui.lib , , krkrfont.RES
copy krkrfont.exe ..\..\..\..\bin\win32\tools
copy krkrfont.tds ..\..\..\..\bin\win32\tools

brc32 -r krkrtpc.rc
tasm32 /zi /ml krkrtpc.asm
ilink32 -v -aa -Tpe krkrtpc.OBJ , krkrtpc.exe, , import32.lib ..\..\..\..\bin\win32\tools\krdevui.lib , , krkrtpc.RES
copy krkrtpc.exe ..\..\..\..\bin\win32\tools
copy krkrtpc.tds ..\..\..\..\bin\win32\tools

brc32 -r krkrsign.rc
tasm32 /zi /ml krkrsign.asm
ilink32 -v -aa -Tpe krkrsign.OBJ , krkrsign.exe, , import32.lib ..\..\..\..\bin\win32\tools\krdevui.lib , , krkrsign.RES
copy krkrsign.exe ..\..\..\..\bin\win32\tools
copy krkrsign.tds ..\..\..\..\bin\win32\tools

brc32 -r krkrlt.rc
tasm32 /zi /ml krkrlt.asm
ilink32 -v -aa -Tpe krkrlt.OBJ , krkrlt.exe, , import32.lib ..\..\..\..\bin\win32\tools\krdevui.lib , , krkrlt.RES
copy krkrlt.exe ..\..\..\..\bin\win32\tools
copy krkrlt.tds ..\..\..\..\bin\win32\tools

call build_userconf.bat

pause