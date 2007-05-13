@call storage=zoom.ks
@layopt layer=message0 page=fore visible=false
@image storage=../../../../tlg4comp/b.bmp layer=base page=back
;@image storage=uni layer=1 page=fore visible=true
@eval exp="kag.fore.base.face=dfBoth, kag.fore.base.stretchCopy(0, 0, 640, 480, kag.back.base, 320, 240, 320, 240, stFastLinear)"
@l
*loop
@fgzoom storage=uni layer=2 time=2500 accel=0 sl=320 st=240 sw=0 sh=0 dl=100 dt=0 dw=360 dh=480 mode=rect
@wfgzoom canskip=true
*ズーム0|ズームその0
@l
@fgzoom storage=uni layer=2 time=2500 accel=2 sl=100 st=0 sw=360 sh=480 dl=100 dt=100 dw=0 dh=0
@wfgzoom canskip=true
*ズーム1|ズームその1
@l
@bgzoom storage=_24_3 time=2500 accel=2 sl=320 st=240 sw=0 sh=0 dl=0 dt=0 dw=640 dh=480
@wbgzoom canskip=true
*ズーム2|ズームその2
@l
@bgzoom storage=_24_3 basestorage=_24 time=6000 accel=0 sl=0 st=0 sw=640 sh=480 dl=320 dt=240 dw=0 dh=0
@wbgzoom canskip=true
*ズーム3|ズームその3
@l
@bgzoom storage=_24_3 basestorage=_24 time=2500 accel=0 sl=0 st=0 sw=32 sh=24 dl=320 dt=240 dw=320 dh=240
@wbgzoom canskip=true
*ズーム4|ズームその4
@l
@backlay
@image storage=_24_3 layer=base page=back
@trans time=2500 method=crossfade
@wt
@l
@jump target=*loop
