@loadplugin module=wuvorbis.dll
@call storage=gvolume.ks
@gvolmenu name="Œø‰Ê‰¹‚»‚Ì‚P(&1)" control=0
@gvolmenu name="Œø‰Ê‰¹‚»‚Ì‚Q(&2)" control=1
[playse buf=0 storage=d:\mss\musicbox.ogg loop=false]\
[ws buf=0 canskip=true][l]\
[playse buf=0 storage=d:\mss\pianohi.ogg loop=false]\
[playse buf=1 storage=d:\mss\musicbox.ogg loop=false]\
[ws buf=0 canskip=true][stopse buf=0][stopse buf=1][l]\
[fadeinse buf=0 storage=d:\mss\trk.ogg loop=true time=6000]\
[wf buf=0 canskip=true][l]\
[fadese buf=0 time=6000 volume=50]\
[wf buf=0 canskip=true][l]\
[fadeinse buf=0 storage=d:\mss\trk.ogg loop=true time=6000]\
[wf buf=0 canskip=true]\
*save|save
[l]\
[seopt buf=0 volume=100]\
[l]\
[fadeoutse buf=0 time=6000]\
[wf buf=0][l]\
[playse buf=0 storage=d:\mss\musicbox.ogg loop=false]\
