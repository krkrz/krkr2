tasm32 /zi /ml startup.asm

brc32 -r userconf.rc
bcc32 -O2 -v -c userconf_main.c find_kirikiri.c util.c
ilink32 -v -aa -Tpe startup.OBJ userconf_main.obj find_kirikiri.obj util.obj, userconf.exe, , import32.lib, , userconf.RES
copy userconf.exe ..\..\..\..\bin\win32\enduser-tools
copy userconf.tds ..\..\..\..\bin\win32\enduser-tools

bcc32 -O2 -v -c xp3_stub.c find_kirikiri.c util.c
ilink32 -v -aa -Tpe startup.OBJ xp3_stub.obj find_kirikiri.obj util.obj, xp3_stub.exe, , import32.lib, ,

brc32 -r krkrrel.rc
bcc32 -O2 -v -c -okrkrrel_main.obj -DPROC_NAME=UIExecReleaser callerlib.c 
ilink32 -v -aa -Tpe startup.OBJ krkrrel_main.obj, krkrrel.exe, , import32.lib, , krkrrel.RES
copy krkrrel.exe ..\..\..\..\bin\win32\tools
copy krkrrel.tds ..\..\..\..\bin\win32\tools

brc32 -r krkrconf.rc
bcc32 -O2 -v -c -okrkrconf_main.obj -DPROC_NAME=UIExecConfig callerlib.c 
ilink32 -v -aa -Tpe startup.OBJ krkrconf_main.obj, krkrconf.exe, , import32.lib, , krkrconf.RES
copy krkrconf.exe ..\..\..\..\bin\win32\tools
copy krkrconf.tds ..\..\..\..\bin\win32\tools

brc32 -r krkrfont.rc
bcc32 -O2 -v -c -okrkrfont_main.obj -DPROC_NAME=UIExecFontMaker callerlib.c 
ilink32 -v -aa -Tpe startup.OBJ krkrfont_main.obj, krkrfont.exe, , import32.lib, , krkrfont.RES
copy krkrfont.exe ..\..\..\..\bin\win32\tools
copy krkrfont.tds ..\..\..\..\bin\win32\tools

brc32 -r krkrtpc.rc
bcc32 -O2 -v -c -okrkrtpc_main.obj -DPROC_NAME=UIExecTPC callerlib.c 
ilink32 -v -aa -Tpe startup.OBJ krkrtpc_main.obj, krkrtpc.exe, , import32.lib, , krkrtpc.RES
copy krkrtpc.exe ..\..\..\..\bin\win32\tools
copy krkrtpc.tds ..\..\..\..\bin\win32\tools

brc32 -r krkrsign.rc
bcc32 -O2 -v -c -okrkrsign_main.obj -DPROC_NAME=UIExecSign callerlib.c 
ilink32 -v -aa -Tpe startup.OBJ krkrsign_main.obj, krkrsign.exe, , import32.lib, , krkrsign.RES
copy krkrsign.exe ..\..\..\..\bin\win32\tools
copy krkrsign.tds ..\..\..\..\bin\win32\tools

brc32 -r krkrlt.rc
bcc32 -O2 -v -c -okrkrlt_main.obj -DPROC_NAME=UIExecLoopTuner callerlib.c 
ilink32 -v -aa -Tpe startup.OBJ krkrlt_main.obj, krkrlt.exe, , import32.lib, , krkrlt.RES
copy krkrlt.exe ..\..\..\..\bin\win32\tools
copy krkrlt.tds ..\..\..\..\bin\win32\tools




pause
