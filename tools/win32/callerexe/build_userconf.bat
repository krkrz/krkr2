brc32 -r userconf.rc
bcc32 -O2 -v -c userconf_main.c
tasm32 /zi /ml userconf.asm
ilink32 -v -aa -Tpe userconf.OBJ userconf_main.obj, userconf.exe, , import32.lib, , userconf.RES

copy userconf.exe ..\..\..\..\bin\win32\enduser-tools
copy userconf.tds ..\..\..\..\bin\win32\enduser-tools

