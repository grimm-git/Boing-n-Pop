
CONST FRUIT_MAX=15

CONST FSTATE_FREE=0
CONST FSTATE_SHOW=1
CONST FSTATE_HIDE=2
CONST FSTATE_WIGGLE=3
CONST FSTATE_POP=4
CONST FSTATE_READY=5

DIM Float Fruit.Type(FRUIT_MAX)
DIM Float Fruit.CellX(FRUIT_MAX)
DIM Float Fruit.CellY(FRUIT_MAX)
DIM Float Fruit.Life(FRUIT_MAX)  'Life time in Timer ticks
DIM Float Fruit.Age(FRUIT_MAX)   'Age, calcuated from Life time
DIM Float Fruit.Event(FRUIT_MAX) 'Wiggle events, calculated from Life time
DIM Float Fruit.State(FRUIT_MAX)

DIM Integer Fruit.anim(FRUIT_MAX)
DIM Integer Fruit.aimg(FRUIT_MAX)

DIM Integer Fruit.show(5)=(150,0,1,3,5,3)
DIM Integer Fruit.hide(5)=(150,3,5,3,1,0)
DIM Integer Fruit.wiggle(8)=(75,4,3,2,3,4,3,2,3)
DIM Integer Fruit.pop(16)=(75,0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7)

' Apple,Cherry,Klee,Banana,Blueberry,Citrus,Strawberry,Orange,Wine,Birne,Himbere,Plum
DIM Integer Fruit.value(11)=(100,1000,1500,42,1242,357,900,750,420,250,600,500)

Sub Fruit.init()
  LOCAL Integer n

  for n=0 to FRUIT_MAX-1
    Fruit.State(n)=FSTATE_FREE
  next
end sub

sub Fruit.update()
  STATIC Float interval=TIMER
  STATIC Float newFruit=TIMER+2000
  LOCAL Integer n

  if TIMER>newfruit then
    newFruit=TIMER+5000+RND*5000
    Fruit.create
  endif

  if TIMER>interval then
    for n=0 to FRUIT_MAX-1
      if Fruit.State(n)=FSTATE_FREE then continue for

      select case Fruit.State(n)
      case FSTATE_SHOW
        interval=TIMER+Fruit.show(0)
        Fruit.playAnim n,Fruit.show(),FSTATE_READY

      case FSTATE_HIDE
        interval=TIMER+Fruit.hide(0)
        Fruit.playAnim n,Fruit.hide(),FSTATE_FREE
        if Fruit.State(n)=FSTATE_FREE then Arena.Cells(Fruit.CellX(n),Fruit.CellY(n))=0
                
      case FSTATE_WIGGLE
        interval=TIMER+Fruit.wiggle(0)
        Fruit.playAnim n,Fruit.wiggle(),FSTATE_READY

      case FSTATE_POP
        interval=TIMER+Fruit.pop(0)
        Fruit.playAnim n,Fruit.pop(),FSTATE_FREE
        if Fruit.State(n)=FSTATE_FREE then Arena.Cells(Fruit.CellX(n),Fruit.CellY(n))=0
      
      case FSTATE_READY
        if TIMER>Fruit.Event(n) then
          Fruit.State(n)=FSTATE_WIGGLE
          Fruit.Event(n)=TIMER+Fruit.Life(n)/3+100
        endif
        if TIMER>Fruit.Age(n) then
          Fruit.State(n)=FSTATE_HIDE
        endif

      end select
    next    
  endif
end sub

sub Fruit.create()
  LOCAL Integer n,ix,iy
  LOCAL Integer estop

  for n=0 to FRUIT_MAX-1
    if Fruit.State(n)=FSTATE_FREE then
      do
        ix=int(12*RND) : iy=int(6*RND)
        inc estop : if estop>10 then exit sub 'too many attempts to find a free cell, try next time
      loop until Arena.Cells(ix,iy)=0
      Arena.Cells(ix,iy)=1        

      Fruit.State(n)=FSTATE_SHOW
      Fruit.Type(n)=int(RND*12)
      Fruit.CellX(n)=ix
      Fruit.CellY(n)=iy
      Fruit.Life(n)=5000+RND*60000
      Fruit.Age(n)=TIMER+Fruit.Life(n)
      Fruit.Event(n)=TIMER+Fruit.Life(n)/3+100
      Fruit.anim(n)=1
      Fruit.aimg(n)=0
      exit for
    endif
  next
end sub

sub Fruit.playAnim(no%,anim%(),state%)
  inc Fruit.anim(no%)
  if Fruit.anim(no%) > bound(anim%()) then
    Fruit.State(no%)=state%
    Fruit.anim(no%)=1
  else
    Fruit.aimg(no%)=anim%(Fruit.anim(no%))
  endif
end sub

sub Fruit.draw()
  LOCAL Integer type,seq
  LOCAL Integer n,x,y

  for n=0 to FRUIT_MAX-1
    if Fruit.State(n)=FSTATE_FREE then continue for

    x=(Screen.W-12*50)/2+Fruit.CellX(n)*50   
    y=VP.Y+5+Fruit.CellY(n)*50
    type=Fruit.Type(n) : seq=Fruit.aimg(n)

    if Fruit.State(n)=FSTATE_POP then
      blit seq*50,350,x,y,50,50,PAGE_SPRITES,&B100  'stars
      blit type*50,300,x,y,50,50,PAGE_SPRITES,&B100 'points
    else
      blit type*50,seq*50,x,y,50,50,PAGE_SPRITES,&B100
    endif
  next
end sub

sub Fruit.hit()
  LOCAL Integer n
  LOCAL Float d=Ball.R+20
  LOCAL Float bx,x,by,y

  bx=(Screen.W-12*50)/2+25 : by=VP.Y+30

  for n=0 to FRUIT_MAX-1
    if Fruit.State(n)=FSTATE_FREE then continue for
    if Fruit.State(n)=FSTATE_POP then continue for

    x=bx+Fruit.CellX(n)*50 : y=by+Fruit.CellY(n)*50 
    if Math.distance(Ball.X,Ball.Y,x,y)<d then
      Fruit.State(n)=FSTATE_POP

      if Ball.Owner>0 then Player.incScore(Ball.Owner,Fruit.value(Fruit.Type(n)))
    endif
  next
end sub

sub Fruit.trigWiggle(no%)
  if Fruit.State(no%)=FSTATE_READY then Fruit.State(no%)=FSTATE_WIGGLE
end sub




