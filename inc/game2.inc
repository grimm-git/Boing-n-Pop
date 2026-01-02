' Game objects
' Code by "Matthias Grimm"

CONST STATE_INTRO=0
CONST STATE_CONFIG=1
CONST STATE_DEMO=2
CONST STATE_GAME=3
CONST STATE_DEAD=4
CONST STATE_PLAYER=5
CONST STATE_LEVEL=6
CONST STATE_OVER=7
CONST STATE_CUTSCENE=8
CONST STATE_VICTORY=9

CONST PAGE_DISPLAY=0
CONST PAGE_BUFFER=1
CONST PAGE_LEVEL=2
CONST PAGE_SPRITES=3

CONST REQ_NEWBALL=0

CONST STS_GATEOPEN=32


DIM Integer Game.State      ' Default=STATE_INTRO
DIM Integer Game.Level      ' levels start with 1
DIM Integer Game.Requests   ' bitfield
DIM Float   Game.frameTimer
DIM Float   Game.gameTimer
DIM Integer Game.NumPlayers
DIM Float   Game.frameTime=1

DIM Float   DBG.updateTime


sub Game.init()
  Game.gameTimer=TIMER

  Controls.init
  Nunchuk.init
  Level.init
  Ball.init
  Player.init MAX_PLAYER   'Game specific include
  Panel.init MAX_PLAYER
end sub

sub Game.loadAssets()
end sub

sub Game.new
  Game.State=STATE_INTRO
end sub

sub Game.start
  mode 8,8
  Game.loadAssets
  Level.prepare Game.Level

  page display PAGE_DISPLAY
  page write PAGE_BUFFER
end sub

sub Game.clrScreen
  Game.frameTimer=TIMER
  cls
end sub

sub Game.draw
  STATIC Integer stage

  Level.draw()
  Panel.draw()

'  text 10,10,BIN$(Game.Requests,64),"L",,1,rgb(white)

  if tst(Game.Requests,REQ_NEWBALL) then
    if NOT tst(Game.Requests,STS_GATEOPEN) then
      if Level.openGate()=1 then
        set(Game.Requests,STS_GATEOPEN)
        select case Level.Gate.Side
        case 0
          Ball.reset(0,Level.Gate.Y-Level.Gate.W/2)
          Ball.vX=3 : Ball.vY=-G_GRAVITY-1
        case 1
          Ball.reset(Level.Gate.X+Level.Gate.W/2,0)
          Ball.vY=1
        case 2
          Ball.reset(Screen.W,Level.Gate.Y-Level.Gate.W/2)
          Ball.vX=-3 : Ball.vY=-G_GRAVITY-1
        end select
        Ball.move()
      endif
    elseif Level.closeGate()=0 then
      clr(Game.Requests,STS_GATEOPEN)
      clr(Game.Requests,REQ_NEWBALL)
    endif
  endif
end sub

sub Game.swapPage
  DBG.updateTime=TIMER-Game.frameTimer
  Game.showFPS
  page copy PAGE_BUFFER to PAGE_DISPLAY,B
  Game.frameTime=TIMER-Game.frameTimer
end sub

Sub Game.showFPS
  LOCAL Integer gameTime=TIMER-Game.gameTimer
  text SCREEN.W-6, 7,str$(int(gameTime/10))+"0","R",7,,,-1
  text SCREEN.W-6,17,str$(int(1000/Game.frameTime)),"R",7,,,-1
  text SCREEN.W-6,27,str$(int(DBG.updateTime*10)/10,2,2)+"ms","R",7,,,-1
  text SCREEN.W-6,39,str$(cfr(1),2,2),"R",7,,,-1
end Sub

function cfr(value!) as Float
  cfr=choice(Game.frameTime<13.3,value!,Game.frameTime/13.3*value!)
end function

sub set(arg%,bit%)
  arg%=arg% OR 1<<bit%
end sub

sub clr(arg%,bit%)
  arg%=arg% AND INV(1<<bit%)
end sub

function tst(arg%,bit%) AS Integer
  tst=(arg% AND 1<<bit%)
end function


