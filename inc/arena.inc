' Area class
' Code by "Matthias Grimm"

CONST ARENA_GATETIME=25
CONST ARENA_WALL=5

DIM Integer Arena.Gate.Status '0=Gate closed, 1=Gate open
DIM Integer Arena.Gate.Side
DIM Float   Arena.Gate.X
DIM Float   Arena.Gate.Y
DIM Float   Arena.Gate.W
DIM Integer Arena.Gate.Pos
DIM Integer Arena.Blocks(2)   'Block that might block gates

DIM Integer Arena.Cells(11,5)

sub Arena.init()
  LOCAL Integer blk,n,m

  for n=0 to 11
    for m=0 to 5
      Arena.Cells(n,m)=0
    next
  next  

  blk=Blocks.addEdge(VP.W-2*ARENA_WALL)
  Blocks.position(blk,VP.X+VP.W/2,VP.Y+ARENA_WALL,0)
  Blocks.setColor(blk,-1,-1)
  Arena.Blocks(1)=blk

  blk=Blocks.addEdge(VP.H-ARENA_WALL)
  Blocks.position(blk,VP.X+ARENA_WALL,VP.Y+(VP.H+ARENA_WALL)/2,PI/2)
  Blocks.setColor(blk,-1,-1)
  Arena.Blocks(0)=blk

  blk=Blocks.addEdge(VP.H-ARENA_WALL)
  Blocks.position(blk,VP.X+VP.W-ARENA_WALL,VP.Y+(VP.H+ARENA_WALL)/2,PI/2)
  Blocks.setColor(blk,-1,-1)
  Arena.Blocks(2)=blk

  if Game.NumPlayers=2 then
    blk=Blocks.addRect(20,10)
    Blocks.position(blk,VP.X+VP.W/2,VP.Y+VP.H-10,0)
    Blocks.setColor(blk,map(15),map(15))

    blk=Blocks.addTriangle(20,15,15)
    Blocks.position(blk,VP.X+VP.W/2,VP.Y+VP.H-18,PI/4)
    Blocks.setColor(blk,map(15),map(15))
  endif
end sub

'Draw only dynamic parts of the Arena
sub Arena.draw()
  LOCAL Integer x,y,p

  box VP.X  ,VP.Y  ,ARENA_WALL,VP.H,0,0,rgb(BLUE)
  box VP.X+VP.W-ARENA_WALL,VP.Y,ARENA_WALL,VP.H,0,0,rgb(BLUE)
  box VP.X  ,VP.Y  ,VP.W,ARENA_WALL,0,0,rgb(BLUE)

  x=Arena.Gate.X : y=Arena.Gate.Y : p=Arena.Gate.Pos
  
  select case Arena.Gate.Side
  case 0,2  'left and right
    box x,y-p,5,p,0,,rgb(BLACK)
  case 1    'top
    box x,y  ,p,5,0,,rgb(BLACK)
  end select 
end sub

sub Arena.setGate() 
    Arena.Gate.Side=int(RND()*3)
    Arena.Gate.Status=0
    Arena.Gate.Pos=0
    Arena.Gate.W=4*Ball.R

    select case Arena.Gate.Side
    case 0  'left
      Arena.Gate.X=VP.X
      Arena.Gate.Y=VP.Y+RND()*VP.H/3+Arena.Gate.W+10
    case 1  'top
      Arena.Gate.X=VP.X+RND()*(VP.W-Arena.Gate.W-20)+10
      Arena.Gate.Y=VP.Y
    case 2  'right
      Arena.Gate.X=VP.X+VP.W-5
      Arena.Gate.Y=VP.Y+RND()*VP.H/3+Arena.Gate.W+10
    end select
end sub

function Arena.openGate() as Integer
  STATIC Integer one
  STATIC Float ti=TIMER

  if one=0 then one=1 : Arena.setGate
  if Arena.Gate.Status=0 then
    if TIMER-ti>ARENA_GATETIME then
      ti=TIMER : inc Arena.Gate.Pos
      if Arena.Gate.Pos>=Arena.Gate.W then
        Arena.Gate.Status=1
        Blocks.lock Arena.Blocks(Arena.Gate.Side)
        one=0
      endif
    endif
  endif
  Arena.openGate=Arena.Gate.Status
end function

function Arena.closeGate() as Integer
  STATIC Float ti=TIMER

  if Arena.Gate.Status=1 then
    if TIMER-ti>ARENA_GATETIME then
      ti=TIMER : inc Arena.Gate.Pos,-1
      if Arena.Gate.Pos<Arena.Gate.W*0.7 then
        Blocks.unlock Arena.Blocks(Arena.Gate.Side)
      endif
      if Arena.Gate.Pos<=0 then
        Arena.Gate.Status=0
      endif
    endif
  endif
  Arena.closeGate=Arena.Gate.Status
end function

