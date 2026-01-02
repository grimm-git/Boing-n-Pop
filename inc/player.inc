' Player data and functions
' The parameter player% counts always from 1..4
'   player%=0 is invalid

CONST PLAYER_MAX=2
CONST PLAYER_NONE=0
CONST PLAYER_ACTIVE=1
CONST PLAYER_DEAD=2

DIM Float   Player.state(PLAYER_MAX)
DIM Float   Player.X(PLAYER_MAX)
DIM Float   Player.Y(PLAYER_MAX)
DIM Integer Player.inpch(PLAYER_MAX)  'Input channel from controls.inc
DIM Integer Player.lives(PLAYER_MAX)
DIM Integer Player.score(PLAYER_MAX)
DIM Integer Player.col32(PLAYER_MAX)  'RGB value of color
DIM Integer Player.col08(PLAYER_MAX)  'Color register of CLUT

sub Player.init()
  Player.reset
end sub

sub Player.reset()
  STATIC Integer Col32(6)=(rgb(red),rgb(blue),rgb(green),rgb(yellow),rgb(magenta),rgb(cyan),rgb(brown))
  STATIC Integer Col08(6)=(RED,BLUE,GREEN,YELLOW,PURPLE,CYAN,DARKRED)
  LOCAL Integer idx
  
  for idx=0 to PLAYER_MAX-1
    Player.state(idx)=PLAYER_NONE
    Player.inpch(idx)=-1 'not set yet
    Player.lives(idx)=5
    Player.score(idx)=0
    Player.col32(idx)=Col32(idx mod 7)
    Player.col08(idx)=Col08(idx mod 7)
    Player.X(idx)=0
    Player.Y(idx)=0
  next
end sub

sub Player.newgame()
  LOCAL Integer idx
  
  for idx=0 to PLAYER_MAX-1
    if Player.state(idx)=PLAYER_DEAD then Player.state(idx)=PLAYER_ACTIVE
    Player.lives(idx)=5
[DBG] Player.lives(idx)=1
    Player.score(idx)=0
  next
end sub

'player% = 1..
function Player.setInpch(player%,inpch%) as Integer
  LOCAL Integer n,idx=player%-1
  
  for n=0 to PLAYER_MAX-1
    if Player.inpch(n)=inpch% then Player.setInpch=0 : exit function
  next

  Player.inpch(idx)=inpch%
  Player.state(idx)=PLAYER_ACTIVE
  Player.setInpch=1
end function

'player% = 1..
function Player.getInpch(player%) as Integer
  Player.getInpch=Player.inpch(player%-1)
end function

'player% = 1..
sub Player.setState(player%,state%)
  Player.active(player%-1)=state%
end sub

'player% = 1..
sub Player.position(player%,posX%,posY%)
  Player.X(player%-1)=posX%
  Player.Y(player%-1)=posY%
end sub

'player% = 1..
function Player.getX(player%) as Float
  Player.getX=Player.X(player%-1)
end function

'player% = 1..
function Player.getY(player%) as Float
  Player.getY=Player.Y(player%-1)
end function

'player% = 1..
function Player.getColor(player%) as Integer
  LOCAL Integer cmode=cmap.getCMode()

  Player.getColor=choice(cmode=8,map(Player.col08(player%-1)),Player.col32(player%-1))
end function

'player% = 1..
sub Player.setColor(player%,color%)
  Player.col32(player%-1)=color%
end sub

'player% = 1..
function Player.getCLUT(player%) as Integer
  Player.getCLUT=Player.col08(player%-1)
end function

'player% = 1..
function Player.getLives(player%) as Integer
  Player.getLives=Player.lives(player%-1)
end function

'player% = 1..
sub Player.decLives(player%)
  LOCAL Integer idx=player%-1

  if Player.lives(idx)>0 then inc Player.lives(idx),-1
end sub

'player% = 1..
sub Player.kill(player%)
  LOCAL Integer idx=player%-1

  if Player.lives(idx)=0 then Player.State(idx)=PLAYER_DEAD
end sub

function Player.isActive(player%) as Integer
  Player.isActive=choice(Player.State(player%-1)=PLAYER_ACTIVE,1,0)
end function

'player% = 1..
sub Player.setActive(player%,state%)
  Player.active(player%-1)=choice(state%>0,PLAYER_ACTIVE,PLAYER_NONE)
end sub

'player% = 1..
function Player.getScore(player%) as Integer
  Player.getScore=Player.score(player%-1)
end function

'player% = 1..
sub Player.incScore(player%,points%)
  LOCAL Integer idx=player%-1

  inc Player.score(idx),points%
end sub

function Player.getWinner() as Integer
  LOCAL Integer idx,winner=-1

  for idx=0 to PLAYER_MAX-1
    if Player.State(idx)=PLAYER_ACTIVE then
      if winner<>-1 then exit function
      winner=idx
    endif
  next
  playSample 8,5512,1
  Player.getWinner=winner+1
end function

function Player.active() as Integer
  LOCAL Integer idx,n

  for idx=0 to PLAYER_MAX-1
    if Player.State(idx)=PLAYER_ACTIVE then inc n
  next  
  Player.active=n
end function

