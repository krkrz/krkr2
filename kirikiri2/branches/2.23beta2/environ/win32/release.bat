copy tvpwin32.bpr tvpwin32.~bpr /y
perl basefix.pl
cd ..\..\base\win32
call makestub.bat
cd ..\..\visual\IA32
call compile.bat
cd ..\..\visual\glgen
call gengl.bat
cd ..\..\msg\win32
call gen_optdesc_res.bat
cd ..\..\environ\win32
"c:\Program Files\Borland\CBuilder5\Bin\bcb" tvpwin32.bpr -b -omakeout.txt
call krkrcp.bat

