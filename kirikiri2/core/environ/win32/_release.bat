if "%1" == "" (
	echo do not run this file directly.
	echo run release.bat instead.
	pause
	exit
)

set TOP_DIR=%~f1

cd %TOP_DIR%\base\win32
call makestub.bat

cd %TOP_DIR%\visual\IA32
call compile.bat

cd %TOP_DIR%\visual\glgen
call gengl.bat

cd %TOP_DIR%\msg\win32
call gen_optdesc_res.bat

cd %TOP_DIR%\environ\win32
copy tvpwin32.bpr tvpwin32.~bpr /y
perl basefix.pl

cd %TOP_DIR%\msg
copy svn_revision.h svn_revision.~h /y
perl update_svn_revision.pl

cd %TOP_DIR%\environ\win32
"c:\Program Files\Borland\CBuilder5\Bin\bcb" tvpwin32.bpr -b -omakeout.txt

call krkrcp.bat

copy tvpwin32.~bpr tvpwin32.bpr /y

cd %TOP_DIR%\msg
copy svn_revision.~h svn_revision.h /y

