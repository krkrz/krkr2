echo off
del tvpgl_ia32.lib
del *.obj
for %%d in (*.nas) do nasmw -g -f obj %%d
for %%d in (*.obj) do tlib tvpgl_ia32.lib + %%d
perl summary.pl
del *.bak
pause
