@wait time=300
@image storage="_24" page=fore layer=base
;
*s
@ct
from=left stay=stayback[l]
@backlay
@image storage="_24_3" page=back layer=base
@trans method=scroll from=left stay=stayback time=1000
@wt
[l]
;
@ct
from=top stay=stayback[l]
@backlay
@image storage="_24" page=back layer=base
@trans method=scroll from=right stay=stayback time=1000
@wt
[l]
@jump target=*s
;
@ct
from=right stay=stayback[l]
@backlay
@image storage="_24_3" page=back layer=base
@trans method=scroll from=right stay=stayback time=2000
@wt
[l]
;
@ct
from=bottom stay=stayback[l]
@backlay
@image storage="_24" page=back layer=base
@trans method=scroll from=bottom stay=stayback time=2000
@wt
[l]
;
@ct
from=left stay=stayfore[l]
@backlay
@image storage="_24_3" page=back layer=base
@trans method=scroll from=left stay=stayfore time=2000
@wt
[l]
;
@ct
from=top stay=stayfore[l]
@backlay
@image storage="_24" page=back layer=base
@trans method=scroll from=top stay=stayfore time=2000
@wt
[l]
;
@ct
from=right stay=stayfore[l]
@backlay
@image storage="_24_3" page=back layer=base
@trans method=scroll from=right stay=stayfore time=2000
@wt
[l]
;
@ct
from=bottom stay=stayfore[l]
@backlay
@image storage="_24" page=back layer=base
@trans method=scroll from=bottom stay=stayfore time=2000
@wt
[l]
;
@ct
from=left stay=nostay[l]
@backlay
@image storage="_24_3" page=back layer=base
@trans method=scroll from=left stay=nostay time=2000
@wt
[l]
;
@ct
from=top stay=nostay[l]
@backlay
@image storage="_24" page=back layer=base
@trans method=scroll from=top stay=nostay time=2000
@wt
[l]
;
@ct
from=right stay=nostay[l]
@backlay
@image storage="_24_3" page=back layer=base
@trans method=scroll from=right stay=nostay time=2000
@wt
[l]
;
@ct
from=bottom stay=nostay[l]
@backlay
@image storage="_24" page=back layer=base
@trans method=scroll from=bottom stay=nostay time=2000
@wt
[l]

